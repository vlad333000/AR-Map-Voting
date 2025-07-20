class V30_MapVoting_Runner_RequestScenarioChangeTransition : V30_MapVoting_Runner {
	override void Run(notnull V30_MapVoting_RunData runData) {
		auto runDataScenario = V30_MapVoting_RunData_Scenario.Cast(runData);
		auto scenarioId = runDataScenario.GetScenarioId();
		auto addonsList = runDataScenario.GetAddonsList();
		GameStateTransitions.RequestScenarioChangeTransition(scenarioId, addonsList);
	};
};
