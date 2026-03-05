[BaseContainerProps()]
class V30_MapVoting_ExternalRunner : V30_MapVoting_Runner {
    string GetJsonString() {
        auto scenario = GetScenario();
        auto missionHeader = scenario.GetMissionHeader();
        auto worldSystemsConfig = scenario.GetWorldSystemsConfig();
        auto addonList = new array<string>();
        scenario.GetAddonList(addonList);

        auto json = "";
        json += "{";
            json += "\"missionHeader\":\"" + missionHeader + "\",";
            json += "\"worldSystemsConfig\":\"" + worldSystemsConfig + "\",";
            json += "\"addonList\":[";
                foreach (auto i, auto addon : addonList) {
                    if (i != 0) json += ",";
                    json += "\"" + addon + "\"";
                }
            json += "]";
        json += "}";
        return json;
    };

    override void SwitchScenario() {
        GameStateTransitions.RequestGameTerminateTransition();
    };
};
