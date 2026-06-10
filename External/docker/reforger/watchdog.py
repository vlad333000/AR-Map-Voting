#!/usr/bin/env python3
"""
watchdog_fastapi.py

A subprocess watchdog with a FastAPI heartbeat API.

Features:
- Loads argv for a binary from a JSON file containing an array of strings.
- Starts the process, waits for first heartbeat (startup readiness).
- Monitors heartbeats and process exit; restarts on timeout or exit.
- Exposes FastAPI endpoints:
    POST /heartbeat
    GET  /status
    POST /shutdown   (optional)
- Uses uvicorn (in-process) with graceful shutdown.
- Kills the whole process group on POSIX for proper cleanup.

Example argv JSON:
  ["./my_app", "--port", "9000"]

Run:
  python watchdog_fastapi.py --argv-json args.json --bind 0.0.0.0 --port 8080
"""

from __future__ import annotations

import argparse
import asyncio
import json
import os
import signal
import subprocess
import sys
import time
from dataclasses import dataclass
from typing import Any, Optional, Sequence, Annotated
import secrets

import uvicorn
import fastapi
from fastapi import FastAPI, HTTPException, Depends, Request
from fastapi.security import HTTPBearer, HTTPAuthorizationCredentials
import logging
from pydantic import BaseModel, Field
from contextlib import asynccontextmanager


# ---------------------- Models ----------------------

class SystemProfileDataStats(BaseModel):
    FPS: float
    FrameTimeMs: float
    RenderTimeMs: float
    GPUTimeMs: float
    SimulationTimeMs: float

class SystemStats(BaseModel):
    FPS: float
    FrameTimeS: float
    TickCount: int
    MemoryAllocationCount: int
    MemoryAllocationKB: int
    ProfileData: Optional[SystemProfileDataStats] = None

class RestApiStats(BaseModel):
    DownloadTrafficKBPerS: float
    UploadTrafficKBPerS: float

class ReplicationStats(BaseModel):
    Running: bool
    RoundTripTimeInMs: Optional[Annotated[list[float], Field(min_length=3, max_length=3)]] = None
    PacketLoss: Optional[Annotated[list[float], Field(min_length=3, max_length=3)]] = None

class PlayerManagerStats(BaseModel):
    PlayerCount: int
    AllPlayerCount: int

class WorldStats(BaseModel):
    WorldTime: float
    NumEntities: Optional[int] = None
    Timestamp: str
    TimeScale: float
    TimeSlice: float
    FixedTimeSlice: float
    PhysicsTimeSlice: float

class AIWorldStats(BaseModel):
    CurrentAmountOfLimitedAIs: int
    CurrentNumOfActiveAIs: int

class HeartbeatStats(BaseModel):
    System: SystemStats
    RestApi: RestApiStats
    Replication: ReplicationStats
    PlayerManager: Optional[PlayerManagerStats] = None
    World: Optional[WorldStats] = None
    AIWorld: Optional[AIWorldStats] = None

class HeartbeatIn(BaseModel):
    Time: str
    TimeUnix: int
    Stats: Optional[HeartbeatStats] = None


class HeartbeatOut(BaseModel):
    ok: bool = True
    ts_monotonic: float
    generation: int
    watched_pid: Optional[int]


class StatusOut(BaseModel):
    generation: int
    watched_pid: Optional[int]
    process_alive: bool
    last_heartbeat_monotonic: float
    seconds_since_heartbeat: Optional[float]
    startup_received: bool


# ---------------------- State ----------------------

@dataclass
class Config:
    argv_json: str
    bind: str
    port: int
    startup_timeout: float
    heartbeat_timeout: float
    kill_grace: float
    backoff_min: float
    backoff_max: float
    token: Optional[str]
    custom_bearer_token: Optional[str]
    require_bearer_token: bool


class WatchState:
    def __init__(self) -> None:
        self._lock = asyncio.Lock()

        self.generation: int = 0
        self.proc: Optional[subprocess.Popen[Any]] = None
        self.pid: Optional[int] = None

        self.last_hb: float = 0.0
        self.startup_event = asyncio.Event()
        self.shutdown_event = asyncio.Event()

    async def reset_for_new_generation(self, gen: int) -> None:
        async with self._lock:
            self.generation = gen
            self.last_hb = 0.0
            self.pid = None
            self.proc = None
            self.startup_event = asyncio.Event()  # reset
            # shutdown_event persists

    async def set_process(self, proc: subprocess.Popen[Any]) -> None:
        async with self._lock:
            self.proc = proc
            self.pid = proc.pid

    async def clear_process(self) -> None:
        async with self._lock:
            self.proc = None
            self.pid = None

    async def record_heartbeat(self) -> tuple[int, Optional[int], float]:
        now = time.monotonic()
        async with self._lock:
            self.last_hb = now
            if not self.startup_event.is_set():
                self.startup_event.set()
            return self.generation, self.pid, now

    async def snapshot(self) -> tuple[int, Optional[int], Optional[subprocess.Popen[Any]], float, bool, bool]:
        async with self._lock:
            return (
                self.generation,
                self.pid,
                self.proc,
                self.last_hb,
                self.startup_event.is_set(),
                self.shutdown_event.is_set(),
            )


STATE = WatchState()


# ---------------------- Helpers ----------------------

def load_argv(path: str) -> list[str]:
    with open(path, "r", encoding="utf-8") as f:
        data = json.load(f)
    if not isinstance(data, list) or not data or not all(isinstance(x, str) for x in data):
        raise ValueError("argv JSON must be a non-empty array of strings, e.g. [\"/path/app\",\"--flag\",\"x\"]")
    return data


def spawn_process(argv: Sequence[str]) -> subprocess.Popen[Any]:
    # New process group so we can terminate the whole tree on POSIX
    if os.name != "nt":
        return subprocess.Popen(list(argv), bufsize=0, preexec_fn=os.setsid)
    else:
        creationflags = subprocess.CREATE_NEW_PROCESS_GROUP  # type: ignore[attr-defined]
        return subprocess.Popen(list(argv), bufsize=0, creationflags=creationflags)


def _posix_killpg(pid: int, sig: int) -> None:
    os.killpg(pid, sig)


async def terminate_process(proc: subprocess.Popen[Any], kill_grace: float) -> None:
    if proc.poll() is not None:
        return

    try:
        if os.name != "nt":
            _posix_killpg(proc.pid, signal.SIGTERM)
        else:
            proc.terminate()
    except Exception:
        try:
            proc.terminate()
        except Exception:
            return

    deadline = time.monotonic() + kill_grace
    while time.monotonic() < deadline:
        if proc.poll() is not None:
            return
        await asyncio.sleep(0.05)

    try:
        if os.name != "nt":
            _posix_killpg(proc.pid, signal.SIGKILL)
        else:
            proc.kill()
    except Exception:
        try:
            proc.kill()
        except Exception:
            pass


# ---------------------- Watch loop ----------------------

async def watch_loop(cfg: Config) -> None:
    argv = load_argv(cfg.argv_json)
    backoff = cfg.backoff_min

    while True:
        gen = (await STATE.snapshot())[0] + 1
        await STATE.reset_for_new_generation(gen)

        if (await STATE.snapshot())[5]:  # shutdown?
            return

        argvi = argv.copy()
        if cfg.custom_bearer_token:
            cfg.token = cfg.custom_bearer_token
            argvi.append("-v30-serverwatchdog-bearer-token")
            argvi.append(cfg.token)
        elif cfg.require_bearer_token:
            cfg.token = secrets.token_urlsafe(32)
            argvi.append("-v30-serverwatchdog-bearer-token")
            argvi.append(cfg.token)

        # spawn
        try:
            proc = spawn_process(argvi)
        except FileNotFoundError:
            print(f"[watchdog] ERROR: binary not found: {argvi[0]}", file=sys.stderr)
            await STATE.shutdown_event.set()  # type: ignore[misc]
            return
        except Exception as e:
            print(f"[watchdog] ERROR: spawn failed: {e}", file=sys.stderr)
            await asyncio.sleep(min(backoff, cfg.backoff_max))
            backoff = min(backoff * 2, cfg.backoff_max)
            continue

        await STATE.set_process(proc)
        print(f"[watchdog] gen={gen} started pid={proc.pid} argv={argvi}")

        # wait for first heartbeat (or exit / shutdown)
        start_deadline = time.monotonic() + cfg.startup_timeout
        while True:
            _, _, _, _, startup_ok, shutting_down = await STATE.snapshot()
            if shutting_down:
                print("[watchdog] shutdown requested; terminating process...")
                await terminate_process(proc, cfg.kill_grace)
                await STATE.clear_process()
                return

            rc = proc.poll()
            if rc is not None:
                print(f"[watchdog] gen={gen} exited before first heartbeat rc={rc}")
                break

            if startup_ok:
                print(f"[watchdog] gen={gen} first heartbeat received")
                break

            if time.monotonic() >= start_deadline:
                print(f"[watchdog] gen={gen} STARTUP TIMEOUT (no heartbeat in {cfg.startup_timeout}s)")
                break

            await asyncio.sleep(0.05)

        # if unhealthy start, restart
        _, _, _, _, startup_ok, _ = await STATE.snapshot()
        if proc.poll() is not None or not startup_ok:
            await terminate_process(proc, cfg.kill_grace)
            await STATE.clear_process()
            await asyncio.sleep(min(backoff, cfg.backoff_max))
            backoff = min(backoff * 2, cfg.backoff_max)
            continue

        # healthy start => reset backoff
        backoff = cfg.backoff_min

        # monitor loop
        while True:
            _, _, _, last_hb, _, shutting_down = await STATE.snapshot()
            if shutting_down:
                print("[watchdog] shutdown requested; terminating process...")
                await terminate_process(proc, cfg.kill_grace)
                await STATE.clear_process()
                return

            rc = proc.poll()
            if rc is not None:
                print(f"[watchdog] gen={gen} exited rc={rc}; restarting")
                break

            now = time.monotonic()
            if last_hb > 0.0 and (now - last_hb) > cfg.heartbeat_timeout:
                print(f"[watchdog] gen={gen} HEARTBEAT TIMEOUT: {now - last_hb:.2f}s; restarting")
                break

            await asyncio.sleep(0.2)

        # await terminate_process(proc, cfg.kill_grace)
        await STATE.clear_process()
        await asyncio.sleep(min(backoff, cfg.backoff_max))
        backoff = min(backoff * 2, cfg.backoff_max)


# ---------------------- FastAPI app ----------------------


def create_app(cfg: Config) -> FastAPI:
    @asynccontextmanager
    async def lifespan(app: FastAPI):
        # Startup
        # Start the watcher as a background task inside the same event loop.
        app.state.watch_task = asyncio.create_task(watch_loop(cfg))

        yield

        # Shutdown
        STATE.shutdown_event.set()
        task: asyncio.Task | None = getattr(app.state, "watch_task", None)
        if task:
            try:
                await asyncio.wait_for(task, timeout=5.0)
            except Exception:
                # If it won't stop cleanly, just proceed with uvicorn shutdown
                pass

        # Best-effort terminate current process
        _, _, proc, _, _, _ = await STATE.snapshot()
        if proc is not None:
            await terminate_process(proc, cfg.kill_grace)
            await STATE.clear_process()

    app = FastAPI(title="Watchdog", version="1.0", lifespan=lifespan)
    bearer = HTTPBearer(auto_error=False)

    def get_token(creds: HTTPAuthorizationCredentials | None = Depends(bearer)) -> HTTPAuthorizationCredentials | None:
        if cfg.token is None:
            return creds
        if creds is None or creds.scheme.lower() != "bearer":
            raise HTTPException(status_code=fastapi.status.HTTP_401_UNAUTHORIZED, detail="Missing bearer token")
        token = creds.credentials
        if token != cfg.token:
            raise HTTPException(status_code=fastapi.status.HTTP_401_UNAUTHORIZED, detail="Invalid token")
        return creds

    @app.post("/heartbeat", response_model=HeartbeatOut)
    async def heartbeat(hb: HeartbeatIn, token: HTTPAuthorizationCredentials | None = Depends(get_token)) -> HeartbeatOut:
        gen, watched_pid, now = await STATE.record_heartbeat()


        #print(hb.model_dump_json(indent=2))
        return HeartbeatOut(ts_monotonic=now, generation=gen, watched_pid=watched_pid)

    @app.get("/status", response_model=StatusOut)
    async def status() -> StatusOut:
        gen, pid, proc, last_hb, startup_ok, _ = await STATE.snapshot()
        alive = (proc is not None and proc.poll() is None)
        since = None
        if last_hb > 0.0:
            since = max(0.0, time.monotonic() - last_hb)
        return StatusOut(
            generation=gen,
            watched_pid=pid,
            process_alive=alive,
            last_heartbeat_monotonic=last_hb,
            seconds_since_heartbeat=since,
            startup_received=startup_ok,
        )

    @app.post("/shutdown")
    async def shutdown() -> dict[str, Any]:
        STATE.shutdown_event.set()
        return {"ok": True}

    class IgnoreUpgradeFilter(logging.Filter):
        def filter(self, record):
            return "Unsupported upgrade request" not in record.getMessage()

    class IgnoreSuccessAccessLog(logging.Filter):
        def filter(self, record):
            return record.args[1] != "POST" or record.args[2] != "/heartbeat" or record.args[4] != 200

    logging.getLogger("uvicorn.error").addFilter(IgnoreUpgradeFilter())
    logging.getLogger("uvicorn.access").addFilter(IgnoreSuccessAccessLog())

    return app


# ---------------------- Entrypoint ----------------------

def parse_args() -> Config:
    p = argparse.ArgumentParser(description="FastAPI+uvicorn watchdog with heartbeat restart.")
    p.add_argument("--argv-json", required=True, help="Path to JSON file containing argv array for the binary.")
    p.add_argument("--bind", default="127.0.0.1", help="Bind address for FastAPI server.")
    p.add_argument("--port", type=int, default=8080, help="Port for FastAPI server.")
    p.add_argument("--startup-timeout", type=float, default=120.0, help="Seconds to wait for first heartbeat.")
    p.add_argument("--heartbeat-timeout", type=float, default=60.0, help="Seconds since last heartbeat before restart.")
    p.add_argument("--kill-grace", type=float, default=3.0, help="Seconds to wait after SIGTERM before SIGKILL.")
    p.add_argument("--backoff-min", type=float, default=1.0, help="Min restart backoff seconds.")
    p.add_argument("--backoff-max", type=float, default=30.0, help="Max restart backoff seconds.")
    p.add_argument("--custom-bearer-token", default=None, help="Optional shared secret required in heartbeat payload.")
    p.add_argument("--require-bearer-token", type=bool, default=False)
    a = p.parse_args()

    return Config(
        argv_json=a.argv_json,
        bind=a.bind,
        port=a.port,
        startup_timeout=a.startup_timeout,
        heartbeat_timeout=a.heartbeat_timeout,
        kill_grace=a.kill_grace,
        backoff_min=a.backoff_min,
        backoff_max=a.backoff_max,
        token = None,
        custom_bearer_token=a.custom_bearer_token,
        require_bearer_token=a.require_bearer_token
    )


def main() -> int:
    cfg = parse_args()
    app = create_app(cfg)

    # Run uvicorn programmatically (single process). For multi-worker, typically run:
    #   uvicorn watchdog_fastapi:app --workers N
    # but that complicates "single watcher controlling one child". Keep 1 worker here.
    uvicorn.run(app, host=cfg.bind, port=cfg.port, log_level="info", use_colors=True)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
