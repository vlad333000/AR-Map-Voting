class V30_MapVoting_Runner_Podval : V30_MapVoting_Runner {
	protected string url;

	protected string serverId;

	void V30_MapVoting_Runner_Podval(string url, string serverId) {
		this.url = url;
		this.serverId = serverId;
	};

	override void Run(notnull V30_MapVoting_RunData runData) {
		auto runDataScenario = V30_MapVoting_RunData_Scenario.Cast(runData);
		auto scenarioId = runDataScenario.GetScenarioId();
		auto game = GetGame();
		auto restApi = game.GetRestApi();
		auto context = restApi.GetContext(url);
		auto query = string.Format("?scenarioId=%1&serverId=%2", scenarioId, serverId);
		context.GET_now(query);
		game.RequestClose();
	};
};
