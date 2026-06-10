#!/usr/bin/env bash
set -Eeuo pipefail

script_dir="$(cd -- "$(dirname -- "${BASH_SOURCE[0]}")" && pwd)"
cd "$script_dir"

compose_project="${COMPOSE_PROJECT_NAME:-v30-map-voting-reforger}"
service="${SERVICE:-reforger}"
build=true

while [ "$#" -gt 0 ]; do
	case "$1" in
		--no-build)
			build=false
			;;
		*)
			echo "Unknown option: $1" >&2
			echo "Usage: $0 [--no-build]" >&2
			exit 2
			;;
	esac
	shift
done

if [ "$build" = true ]; then
	docker compose -p "$compose_project" build "$service"
fi

docker compose -p "$compose_project" up -d "$service"
docker compose -p "$compose_project" ps "$service"

echo "Deploy complete. Follow logs with: docker compose -p $compose_project logs -f $service"
