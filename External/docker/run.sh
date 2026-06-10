#!/usr/bin/env bash
set -x
umask 0002
./steamcmd.sh
trap 'exit 0' INT
source ./.venv/bin/activate
cd "${REFORGER_DIR}"
python3 ./watchdog.py --argv-json "${REFORGER_PROFILE_DIR}/args.json"
