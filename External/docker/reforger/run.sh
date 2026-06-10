#!/usr/bin/env bash
set -x
umask 0002
cd "${REFORGER_SERVER_DIR}"
reforger_argv_text="$(envsubst < "${REFORGER_ARGV_PATH}")"
read -r -a reforger_argv <<< "$reforger_argv_text"

runner=()
if [ "$REFORGER_NICE_ENABLED" = "true" ]; then
	if nice -n "$REFORGER_NICE_LEVEL" true 2>/dev/null; then
		echo "Running with nice = ${REFORGER_NICE_LEVEL}..."
		runner=(nice -n "$REFORGER_NICE_LEVEL")
	else
		echo "$$REFORGER_NICE_ENABLED is set to true, but failed to set it, probably SYS_NICE capability is missing!"
	fi
fi

if [ "${REFORGER_VOTING_ENABLED}" = "true" ]; then
	template_path="${REFORGER_CONFIG_TEMPLATE_PATH:-${REFORGER_PROFILE_DIR}/config.template.json}"
	if [ ! -f "$template_path" ]; then
		if [ -f "${REFORGER_CONFIG_PATH}" ]; then
			cp "${REFORGER_CONFIG_PATH}" "$template_path"
		else
			template_path="${REFORGER_DIR}/config.template.json"
		fi
	fi

	exec "${runner[@]}" python3 "${REFORGER_DIR}/voting/runserver.py" \
		--profile "${REFORGER_PROFILE_PATH}" \
		--config "$template_path" \
		--output "${REFORGER_CONFIG_PATH}" \
		"./ArmaReforgerServer" \
		-- \
		"${reforger_argv[@]}" \
		"$@"
fi

exec "${runner[@]}" ./ArmaReforgerServer "${reforger_argv[@]}" "$@"
