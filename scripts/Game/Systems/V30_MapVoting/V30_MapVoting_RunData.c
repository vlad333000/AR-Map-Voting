class V30_MapVoting_RunData {
	protected void V30_MapVoting_RunData();
};

class V30_MapVoting_RunData_Scenario : V30_MapVoting_RunData {
	protected string scenarioId;

	protected string addonsList;

	void V30_MapVoting_RunData_Scenario(string scenarioId, string addonsList) {
		this.scenarioId = scenarioId;
		this.addonsList = addonsList;
	};

	string GetScenarioId() {
		return scenarioId;
	};

	string GetAddonsList() {
		return addonsList;
	};
};
