#!/usr/bin/env bash
set -x
cd "${STEAMCMD_DIR}"
mapfile -t args < <(envsubst < "${STEAMCMD_ARGV_PATH}")
./steamcmd.sh "${args[@]}"
