import subprocess
import json
import sys
import os
import makeconfig
import argparse
import json

def run(exe: str, profile: str, config_template_path: str = "config.template.json", force_json: bool = False, *args: str):
    first = True

    while True:
        try:
            # Step 1: Generate server config using a helper script
            if not first:
                with open(os.path.join(profile, "profile", "V30", "MapVoting", "voted.json"), 'r') as f:
                    scenario = json.load(f)
            else:
                scenario = "{B88CC33A14B71FDC}Missions/V30_MapVoting_Mission.conf"
                first = True

            makeconfig.makeconfig(scenario, config_template_path, "config.json", force_json)

        except Exception as e:
            raise RuntimeError(f"[ERROR] Failed to generate server config. Error: {e}")

        try:
            # Step 2: Process the arguments
            new_args = []
            new_args.extend(["-profile", profile])
            new_args.extend(["-config", "config.json"])

            i = 0
            while i < len(args):
                if args[i].lower() == "-config":
                    # Skip this and its value (i.e., remove existing -config argument)
                    i += 2
                elif args[i].lower() == "-profile":
                    # Skip this and its value (i.e., remove existing -profile argument)
                    i += 2
                else:
                    new_args.append(args[i])
                    i += 1


            print(f"[INFO] Running command: {exe} {' '.join(new_args)}")

            # Step 3: Run the executable with updated arguments
            result = subprocess.run(
                [exe] + new_args,
                check=False,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True
            )

            if result.stdout:
                print(f"[EXEC] {result.stdout}")

        except Exception as e:
            raise RuntimeError(f"[ERROR] Command execution failed: {e}")

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Run server with scenario voting addon.")
    parser.add_argument(
        "EXE",
        help="Path to ArmaReforger executable"
    )
    parser.add_argument(
        "-p", "--profile",
        required=True,
        help="Path to profile folder"
    )
    parser.add_argument(
        "-c", "--config",
        default="config.template.json",
        help="Path to file with config template (simple json file with all required config parameters witout scenarioId - it will be automatically inserted).",
    )
    parser.add_argument(
        "--force-json",
        action="store_true",
        help="Force use of Python's built-in json module, ignoring json5."
    )
    args, argv = parser.parse_known_args()

    run(args.EXE, args.profile, args.config, args.force_json, *argv)

