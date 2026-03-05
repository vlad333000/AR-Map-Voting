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

    string GetPlainTextString() {
        auto scenario = GetScenario();

        auto plain = "";
        plain += scenario.GetMissionHeader() + "\n";

        if (scenario.IsCustomWorldSystemsConfig())
            plain += scenario.GetWorldSystemsConfig();
        plain += "\n";

        if (scenario.IsCustomAddonList()) {
            auto addonList = new array<string>();
            scenario.GetAddonList(addonList);

            foreach (auto i, auto addon : addonList) {
                if (i != 0) plain += ",";
                plain += addon;
            };
        };
        plain += "\n";

        return plain;
    };

    string GetXmlString() {
        auto scenario = GetScenario();

        auto xml = "";
        xml += "<scenario>";
        {
            xml += "<missionHeader>" + scenario.GetMissionHeader() + "</missionHeader>";

            if (scenario.IsCustomWorldSystemsConfig())
                xml += "<worldSystemsConfig>" + scenario.GetWorldSystemsConfig() + "</worldSystemsConfig>";

            if (scenario.IsCustomAddonList()) {
                auto addonList = new array<string>();
                scenario.GetAddonList(addonList);

                xml += "<addons>";
                foreach (auto addon : addonList)
                    xml += "<addon>" + addon + "</addon>";
                xml += "</addons>";
            };
        };
        xml += "</scenario>";

        return xml;
    };

    override void SwitchScenario() {
        GameStateTransitions.RequestGameTerminateTransition();
    };
};
