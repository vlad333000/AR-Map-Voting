class V30_MapVoting_Runner_Podval : V30_MapVoting_Runner {
	protected string m_Url;

	protected string m_ServerId;

	void V30_MapVoting_Runner_Podval(string url, string serverId) {
		m_Url = url;
		m_ServerId = serverId;
	};

	override void Run(notnull V30_MapVoting_RunData runData) {
		auto runDataScenario = V30_MapVoting_RunData_Scenario.Cast(runData);
		auto scenarioId = runDataScenario.GetScenarioId();
		auto game = GetGame();
		auto restApi = game.GetRestApi();
		auto context = restApi.GetContext(m_Url);
		auto query = string.Format("?scenarioId=%1&serverId=%2", scenarioId, m_ServerId);
		context.GET_now(query);
		game.RequestClose();
	};
};
