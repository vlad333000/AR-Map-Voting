class V30_MapVoting_SimpleShutdownRunner : V30_MapVoting_Runner {
	override void Run(notnull V30_MapVoting_RunData runData) {
        auto game = GetGame();
        game.RequestClose();
	};
};
