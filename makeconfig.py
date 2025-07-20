import json
import argparse

# Optional: Try using json5
try:
    import json5
    JSON5_AVAILABLE = True
except ImportError:
    JSON5_AVAILABLE = False

def makeconfig(scenario: str, template: str = "config.template.json", output: str = "config.json", force_json: bool = False):

    # Step 1: Read config template (may contain invalid syntax)
    with open(template) as f:
        # Choose parser based on --force-json flag
        if force_json:
            print(f"[INFO] Forced built-in json module")
        use_json5 = not force_json and JSON5_AVAILABLE

        if use_json5:
            try:
                config = json5.load(f)
            except Exception as e:
                raise RuntimeError(f"[ERROR] Failed to parse template with json5 from config.template.json file. Error: {e}")
        else:
            try:
                config = json.load(f)
            except Exception as e:
                raise RuntimeError(f"[ERROR] Failed to parse template with json from config.template.json file. Error: {e}")

    # Step 2 Inject the value into "scenarioId" key in config object
    if not config:
        config = {}
    if 'game' not in config:
        config['game'] = {}
    config['game']['scenarioId'] = scenario

    # Step 3: Write merged and updated config to output file
    with open(output, 'w') as f:
        json.dump(config, f, indent=4)


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Merge scenario ID into configuration template.")
    parser.add_argument(
        "-s", "--scenario",
        required=True,
        help="Scenario id or path to file with scenario if --scenario-file specified."
    )
    parser.add_argument(
        "--scenario-file",
        action="store_true",
        help="If specified then scenario id will be obtained from file."
    )
    parser.add_argument(
        "-t", "--template",
        default="config.template.json",
        help="Path to file with config template.",
    )
    parser.add_argument(
        "--force-json",
        action="store_true",
        help="Force use of Python's built-in json module, ignoring json5."
    )
    parser.add_argument(
        "-o", "--output",
        default="config.json",
        help="Path to output config file."
    )
    args = parser.parse_args()

    if args.scenario_file:
        with open(args.scenario, 'r') as f:
            scenario = json.load(f)
    else:
        scenario = args.scenario

    makeconfig(scenario, args.template, args.output, force_json=args.force_json)
