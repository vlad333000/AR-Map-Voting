[BaseContainerProps()]
class V30_MapVoting_SimpleRunner : V30_MapVoting_Runner {
    override void SwitchScenario() {
        auto scenario = GetScenario();

        if (scenario.IsInherited(V30_MapVoting_RepeatChoice)) {
            GameStateTransitions.RequestScenarioRestart();
        };

        auto missionHeader = scenario.GetMissionHeader();
        auto worldSystemsConfig = scenario.GetWorldSystemsConfig();
        auto addonList = new array<string>();
		scenario.GetAddonList(addonList);

        auto addonListStr = "";
        foreach (auto addon : addonList) {
            if (GameProject.IsVanillaAddon(addon))
                continue;
            if (!addonListStr.IsEmpty())
                addonListStr += ",";
            addonListStr += addon;
        };

		GameStateTransitions.RequestScenarioChangeTransition(missionHeader, worldSystemsConfig, addonListStr);
    };
};
