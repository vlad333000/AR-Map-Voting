class V30_MapVoting_Runner_RequestScenarioChangeTransition : V30_MapVoting_Runner {
	override void Run(notnull V30_MapVoting_RunData runData) {
		auto runDataScenario = V30_MapVoting_RunData_Scenario.Cast(runData);
		auto scenarioId = runDataScenario.GetScenarioId();
		auto addonsList = runDataScenario.GetAddonsList();
		auto systemConfig = MissionHeader.ReadMissionHeader(scenarioId).GetWorldSystemsConfig();

		auto saveGameManager = GetGame().GetSaveGameManager();
		if (saveGameManager)
			saveGameManager.StartPlaythrough(scenarioId, transition: false);

		if (GameStateTransitions.RequestScenarioChangeTransition(scenarioId, systemConfig, addonsList))
			GetGame().GetMenuManager().CloseAllMenus();
		else
			PrintFormat("Failed to start scenario %1 with systemConfig %2 and addonsList %3.", scenarioId, systemConfig, addonsList, level: LogLevel.ERROR);;
	};
};
