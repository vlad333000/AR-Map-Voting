class V30_MapVoting_Runner_ScriptedRestart : V30_MapVoting_Runner {
	override void Run(notnull V30_MapVoting_RunData runData) {
        // Get voted scenario id
		auto runDataScenario = V30_MapVoting_RunData_Scenario.Cast(runData);
		auto scenarioId = runDataScenario.GetScenarioId();

        // Write it to the file as valid json-formatted string
        auto fileContent = string.Format("\"%1\"", scenarioId);
        auto file = FileIO.OpenFile("$profile:V30/MapVoting/voted.json", FileMode.WRITE);
        file.WriteLine(fileContent);
        file.Close();

        // Close game, so script on next iteration can load scenario into config
        auto game = GetGame();
        game.RequestClose();
	};
};
