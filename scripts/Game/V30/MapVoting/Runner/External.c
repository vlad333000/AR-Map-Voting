[BaseContainerProps()]
class V30_MapVoting_ExternalRunner : V30_MapVoting_Runner {
    string GetJsonString() {
        auto scenario = GetScenario();

        auto json = "";
        json += "{";
        {
            auto missionHeader = scenario.GetMissionHeader();
            json += "\"missionHeader\":\"" + missionHeader + "\",";

            if (scenario.IsCustomWorldSystemsConfig()) {
                auto worldSystemsConfig = scenario.GetWorldSystemsConfig();
                json += "\"worldSystemsConfig\":\"" + worldSystemsConfig + "\",";
            };

            if (scenario.IsCustomAddonList()) {
                auto addonList = new array<string>();
                scenario.GetAddonList(addonList);

                json += "\"addonList\":[";
                {
                    foreach (auto i, auto addon : addonList) {
                        if (i != 0) json += ",";
                        json += "\"" + addon + "\"";
                    };
                };
                json += "]";
            };
        };
        json += "}";
        return json;
    };

    override void SwitchScenario() {
        GameStateTransitions.RequestGameTerminateTransition();
    };
};
