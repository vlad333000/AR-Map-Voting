# FROM cm2network/steamcmd:steam-trixie
FROM debian:trixie-slim AS steam_build

ARG UID=1000
ARG GID=1000
ARG USER=reforger
ARG GROUP=reforger

ENV USER=${USER}
ENV GROUP=${GROUP}
ENV HOMEDIR="/home/${USER}"
ENV STEAMCMDDIR="${HOMEDIR}/steamcmd"

RUN set -x \
    && apt-get update \
    && DEBIAN_FRONTEND="noninteractive" apt-get install -y --no-install-recommends --no-install-suggests \
        lib32stdc++6 \
        lib32gcc-s1 \
        ca-certificates \
        nano \
        curl \
        locales \
    && sed -i -e 's/# en_US.UTF-8 UTF-8/en_US.UTF-8 UTF-8/' /etc/locale.gen \
    && dpkg-reconfigure --frontend=noninteractive locales \
    && groupadd -g "${GID}" "${GROUP}" \
    && useradd -u "${UID}" -g "${GID}" -m "${USER}" \
    && su "${USER}" -c \
        "mkdir -p \"${STEAMCMDDIR}\" \
                && curl -fsSL 'https://steamcdn-a.akamaihd.net/client/installer/steamcmd_linux.tar.gz' | tar xvzf - -C \"${STEAMCMDDIR}\" \
                && \"./${STEAMCMDDIR}/steamcmd.sh\" +quit \
                && ln -s \"${STEAMCMDDIR}/linux32/steamclient.so\" \"${STEAMCMDDIR}/steamservice.so\" \
                && mkdir -p \"${HOMEDIR}/.steam/sdk32\" \
                && ln -s \"${STEAMCMDDIR}/linux32/steamclient.so\" \"${HOMEDIR}/.steam/sdk32/steamclient.so\" \
                && ln -s \"${STEAMCMDDIR}/linux32/steamcmd\" \"${STEAMCMDDIR}/linux32/steam\" \
                && mkdir -p \"${HOMEDIR}/.steam/sdk64\" \
                && ln -s \"${STEAMCMDDIR}/linux64/steamclient.so\" \"${HOMEDIR}/.steam/sdk64/steamclient.so\" \
                && ln -s \"${STEAMCMDDIR}/linux64/steamcmd\" \"${STEAMCMDDIR}/linux64/steam\" \
                && ln -s \"${STEAMCMDDIR}/steamcmd.sh\" \"${STEAMCMDDIR}/steam.sh\"" \
    && ln -s "${STEAMCMDDIR}/linux64/steamclient.so" "/usr/lib/x86_64-linux-gnu/steamclient.so" \
    && rm -rf /var/lib/apt/lists/*

ENV REFORGER_NICE_ENABLED=false
ENV REFORGER_NICE_LEVEL=-20
ENV REFORGER_DIR="${HOMEDIR}/reforger"
ENV REFORGER_PROFILE_PATH="${REFORGER_DIR}"
ENV REFORGER_PROFILE_DIR="${REFORGER_PROFILE_PATH}/profile"
ENV REFORGER_ADDONS_DIR="${REFORGER_PROFILE_PATH}/addons"
ENV REFORGER_SERVER_DIR="${REFORGER_PROFILE_PATH}/server"
ENV REFORGER_CONFIG_PATH="${REFORGER_PROFILE_DIR}/config.json"
ENV REFORGER_CONFIG_TEMPLATE_PATH="${REFORGER_PROFILE_DIR}/config.template.json"
ENV REFORGER_ARGV_PATH="${REFORGER_PROFILE_DIR}/argv.txt"
ENV REFORGER_VOTING_ENABLED=true
ENV STEAMCMD_LOGIN="Anonymous"
ENV STEAMCMD_PASSWORD=""
ENV STEAMCMD_DIR="${STEAMCMDDIR}"
ENV STEAMCMD_ARGV_PATH="${REFORGER_DIR}/steamcmd.sh.argv"

RUN set -x \
	&& apt-get update \
	&& apt-get install -y --no-install-recommends --no-install-suggests \
		libcurl4 \
		net-tools \
		wget \
		gnupg \
		gettext \
		python3 \
		python3-venv \
		python3-pip \
		build-essential \
		ca-certificates \
	&& echo "deb http://deb.debian.org/debian bullseye main" > /etc/apt/sources.list.d/bullseye.list \
	&& printf "Package: *\nPin: release n=bullseye\nPin-Priority: 100\n" > /etc/apt/preferences.d/bullseye \
	&& apt-get update \
	&& apt-get install -y --no-install-recommends --no-install-suggests -t bullseye \
		libssl1.1 \
	&& rm -f /etc/apt/sources.list.d/bullseye.list \
	&& rm -f /etc/apt/preferences.d/bullseye \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/* \
	&& mkdir "${REFORGER_DIR}" \
	&& chown -R ${USER}:${GROUP} "${REFORGER_DIR}" \
	&& chmod -R 2770 "${REFORGER_DIR}" \
	&& su "${USER}" -c " \
		mkdir -p \"${REFORGER_PROFILE_DIR}\" \
		&& mkdir -p \"${REFORGER_ADDONS_DIR}\" \
		&& mkdir -p \"${REFORGER_SERVER_DIR}\" \
		"

COPY reforger/requirements.txt "${REFORGER_DIR}/requirements.txt"
RUN set -x \
	&& cd "${REFORGER_DIR}" \
	&& python3 -m venv ./.venv \
        && . ./.venv/bin/activate \
        && pip install --upgrade pip \
        && pip install --no-cache-dir -r requirements.txt


COPY reforger/config.json "${REFORGER_CONFIG_PATH}"
COPY reforger/config.json "${REFORGER_PROFILE_DIR}/config.template.json"
COPY reforger/argv.txt "${REFORGER_ARGV_PATH}"
COPY reforger/run.sh "${REFORGER_DIR}/reforger.sh"
COPY reforger/voting/runtime "${REFORGER_DIR}/voting"
COPY reforger/voting/profile "${REFORGER_PROFILE_DIR}/V30/MapVoting"
COPY steamcmd/argv.txt "${STEAMCMD_ARGV_PATH}"
COPY steamcmd/run.sh "${REFORGER_DIR}/steamcmd.sh"
COPY run.sh "${REFORGER_DIR}/run.sh"
COPY reforger/watchdog.py "${REFORGER_DIR}/watchdog.py"
COPY reforger/args.json "${REFORGER_PROFILE_DIR}/args.json"

RUN set -x \
	&& chown ${USER}:${GROUP} -R "${REFORGER_DIR}" \
	&& chmod 660 \
		"${REFORGER_CONFIG_PATH}" \
		"${REFORGER_PROFILE_DIR}/config.template.json" \
		"${REFORGER_PROFILE_DIR}/V30/MapVoting/config.json" \
		"${REFORGER_ARGV_PATH}" \
		"${STEAMCMD_ARGV_PATH}" \
	&& chmod 750 \
		"${REFORGER_DIR}/reforger.sh" \
		"${REFORGER_DIR}/steamcmd.sh" \
		"${REFORGER_DIR}/run.sh" \
		"${REFORGER_DIR}/watchdog.py" \
		"${REFORGER_DIR}/voting/makeconfig.py" \
		"${REFORGER_DIR}/voting/runserver.py"

WORKDIR ${REFORGER_DIR}

ENV PATH="${REFORGER_DIR}/.venv/bin:$PATH"

USER ${USER}

CMD ["./run.sh"]
#CMD ["python3", "./watchdog.py"]
