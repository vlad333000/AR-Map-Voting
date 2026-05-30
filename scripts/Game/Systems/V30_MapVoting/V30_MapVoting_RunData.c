class V30_MapVoting_RunData {
	protected void V30_MapVoting_RunData();
};

class V30_MapVoting_RunData_Scenario : V30_MapVoting_RunData {
	protected string m_ScenarioId;

	protected string m_AddonsList;

	void V30_MapVoting_RunData_Scenario(string scenarioId, string addonsList) {
		m_ScenarioId = scenarioId;
		m_AddonsList = addonsList;
	};

	string GetScenarioId() {
		return m_ScenarioId;
	};

	string GetAddonsList() {
		return m_AddonsList;
	};
};
