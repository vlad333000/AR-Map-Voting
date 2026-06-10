# V30 Map Voting Docker Example

This folder contains an example Docker Compose setup for running an Arma Reforger server with V30 Map Voting.

The container installs the server with SteamCMD, starts it through the included watchdog script, and keeps the server profile, logs, addons, and downloaded server files in Docker volumes.

> [!IMPORTANT]
> This is an example local Compose setup. Review and edit the server config before using it for a public server.

## Requirements

- Docker
- Docker Compose v2 (`docker compose`)
- UDP port `2001` available on the host

## Start

From this folder:

```sh
docker compose -p v30-map-voting-reforger build
docker compose -p v30-map-voting-reforger up -d
```

Or use the helper script:

```sh
./deploy.sh
```

On Windows PowerShell:

```powershell
.\deploy.ps1
```

To start without rebuilding:

```sh
./deploy.sh --no-build
```

```powershell
.\deploy.ps1 -NoBuild
```

## Files To Edit Before Building

These files are copied into the image during build:

- `reforger/config.json` - default Arma Reforger server config and config template.
- `reforger/argv.txt` - command line arguments passed to `ArmaReforgerServer`.
- `reforger/voting/profile/config.json` - V30 Map Voting config.
- `steamcmd/argv.txt` - SteamCMD install/update commands.
- `docker-compose.yml` - ports, image name, build args, and volumes.

After changing these source files, rebuild the image:

```sh
docker compose -p v30-map-voting-reforger build
docker compose -p v30-map-voting-reforger up -d
```

## Runtime Data

Compose defines these volume entries. Docker stores them as project-scoped named volumes, so their real names usually include the Compose project prefix.

- `reforger_profile` mounted at `/home/reforger/reforger/profile`
- `reforger_logs` mounted at `/home/reforger/reforger/logs`
- `reforger_addons` mounted at `/home/reforger/reforger/addons`
- `reforger_server` mounted at `/home/reforger/reforger/server`

Important runtime paths inside the container:

- `/home/reforger/reforger/profile/config.json` - active generated server config.
- `/home/reforger/reforger/profile/config.template.json` - template used by map voting to generate `config.json`.
- `/home/reforger/reforger/profile/argv.txt` - server launch arguments.
- `/home/reforger/reforger/profile/V30/MapVoting/config.json` - map voting config.
- `/home/reforger/reforger/logs` - server logs.
- `/home/reforger/reforger/addons` - downloaded/profile addons.
- `/home/reforger/reforger/server` - Arma Reforger server files installed by SteamCMD.

## Edit Runtime Configs

Open a shell in the running container:

```sh
docker compose -p v30-map-voting-reforger exec reforger bash
```

Then edit files in `/home/reforger/reforger/profile`, for example:

```sh
nano /home/reforger/reforger/profile/config.template.json
nano /home/reforger/reforger/profile/argv.txt
nano /home/reforger/reforger/profile/V30/MapVoting/config.json
```

Restart after runtime config changes:

```sh
docker compose -p v30-map-voting-reforger restart reforger
```

## Logs

Follow container output:

```sh
docker compose -p v30-map-voting-reforger logs -f reforger
```

Server log files are stored in the `reforger_logs` volume and mounted inside the container at:

```text
/home/reforger/reforger/logs
```

To see where Docker stores a volume on the host:

```sh
docker volume inspect v30-map-voting-reforger_reforger_logs
docker volume inspect v30-map-voting-reforger_reforger_profile
```

## Common Commands

Show status:

```sh
docker compose -p v30-map-voting-reforger ps
```

Stop:

```sh
docker compose -p v30-map-voting-reforger down
```

Rebuild and recreate:

```sh
docker compose -p v30-map-voting-reforger build --no-cache
docker compose -p v30-map-voting-reforger up -d --force-recreate
```

Update server files through the normal startup flow:

```sh
docker compose -p v30-map-voting-reforger restart reforger
```

Remove the container but keep volumes:

```sh
docker compose -p v30-map-voting-reforger down
```

Remove the container and all named volumes:

```sh
docker compose -p v30-map-voting-reforger down -v
```

> [!WARNING]
> `docker compose down -v` deletes the profile, logs, addons, and installed server files stored in the Compose volumes.
