import os
import argparse
import json
import subprocess
import shlex

import makeconfig


DEFAULT_VOTING_SCENARIO = "{B88CC33A14B71FDC}Missions/V30_MapVoting_Mission.conf"


def normalize_args(args: tuple[str, ...]) -> list[str]:
    normalized = []

    for arg in args:
        if arg == "--":
            continue
        if any(char.isspace() for char in arg):
            normalized.extend(shlex.split(arg))
        else:
            normalized.append(arg)

    return normalized


def run(
    exe: str,
    profile: str,
    config_template_path: str = "config.template.json",
    output: str = "config.json",
    force_json: bool = False,
    *args: str,
):
    voting = True

    while True:
        try:
            # Step 1: Generate server config using a helper script
            if not voting:
                with open(os.path.join(profile, "profile", "V30", "MapVoting", "voted.json"), 'r') as f:
                    scenario = json.load(f)
            else:
                scenario = DEFAULT_VOTING_SCENARIO

            voting = not voting
            makeconfig.makeconfig(scenario, config_template_path, output, force_json)

        except Exception as e:
            raise RuntimeError(f"[ERROR] Failed to generate server config. Error: {e}")

        try:
            # Step 2: Process the arguments
            new_args = []
            new_args.extend(["-profile", profile])
            new_args.extend(["-config", output])

            parsed_args = normalize_args(args)
            i = 0
            while i < len(parsed_args):
                if parsed_args[i].lower() == "-config":
                    # Skip this and its value (i.e., remove existing -config argument)
                    i += 2
                elif parsed_args[i].lower() == "-profile":
                    # Skip this and its value (i.e., remove existing -profile argument)
                    i += 2
                else:
                    new_args.append(parsed_args[i])
                    i += 1


            print(f"[INFO] Running command: {exe} {' '.join(new_args)}", flush=True)

            # Step 3: Run the executable with updated arguments
            subprocess.run(
                [exe] + new_args,
                check=True,
            )

        except Exception as e:
            raise RuntimeError(f"[ERROR] Command execution failed: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(
        description="Run server with scenario voting addon.",
        allow_abbrev=False,
    )
    parser.add_argument(
        "--profile",
        required=True,
        help="Path to profile folder"
    )
    parser.add_argument(
        "--config",
        default="config.template.json",
        help="Path to file with config template (simple json file with all required config parameters witout scenarioId - it will be automatically inserted).",
    )
    parser.add_argument(
        "--output",
        default="config.json",
        help="Path to generated server config file.",
    )
    parser.add_argument(
        "--force-json",
        action="store_true",
        help="Force use of Python's built-in json module, ignoring json5."
    )
    parser.add_argument(
        "EXE",
        help="Path to ArmaReforger executable"
    )
    args, argv = parser.parse_known_args()

    run(args.EXE, args.profile, args.config, args.output, args.force_json, *argv)
