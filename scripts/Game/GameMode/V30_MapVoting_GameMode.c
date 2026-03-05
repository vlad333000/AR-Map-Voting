[EntityEditorProps(category: "GameCode/GameMode", description: "Game mode that allows players to vote for the next scenario.")]
class V30_MapVoting_GameModeClass : SCR_BaseGameModeClass {};

class V30_MapVoting_GameMode : SCR_BaseGameMode {
	[Attribute()]
	protected ref array<ref V30_MapVoting_ScenarioGenerator> scenarioGenerators;

	[Attribute()]
	protected V30_MapVoting_EChoicesMode choicesMode;

	[Attribute()]
	protected ref array<ref V30_MapVoting_Choice> choices;

	[Attribute()]
	protected int choicesCount;

	[Attribute()]
	protected V30_MapVoting_EGiveVoteAbility giveVoteAbility;

	[Attribute()]
	protected ref V30_MapVoting_Runner runner;

	[Attribute("$profile:V30/MapVoting/config.json", desc: "Path to config file.")]
	protected string configPath;

	protected ref V30_MapVoting_Vote vote;

	protected V30_MapVoting_VotingWorldSystem votingWorldSystem;

	protected V30_MapVoting_Menu votingMenu;

	void V30_MapVoting_GameMode(IEntitySource src, IEntity parent) {
		if (SCR_Global.IsEditMode())
			return;

		SetEventMask(EntityEvent.INIT);
	};

	override protected event void EOnInit(IEntity owner) {
		super.EOnInit(owner);

		if (SCR_Global.IsEditMode())
			return;

		auto world = owner.GetWorld();
		this.votingWorldSystem = V30_MapVoting_VotingWorldSystem.GetWorldInstance(world);
		V30_MapVoting_VotingWorldSystem.ConnectEvent(this.votingWorldSystem.OnVoteEnded, this.OnVoteEnded);
		V30_MapVoting_VotingWorldSystem.ConnectEvent(this.votingWorldSystem.OnVoteCancelled, this.OnVoteCancelled);

		if (IsMaster()) {
			// TODO: load config
			// TODO: choices
			// TODO: vote
			// TODO: votingWorldSystem.PrepareVote(vote);
		};

		if (!System.IsConsoleApp()) {
			auto game = GetGame();
			auto menuManager = game.GetMenuManager();
			auto menu = menuManager.OpenMenu(ChimeraMenuPreset.V30_MapVoting_Menu);
			this.votingMenu = V30_MapVoting_Menu.Cast(menu);
			this.votingMenu.Setup(this.votingWorldSystem);
		};
	};

	override protected void OnGameModeStart() {
		super.OnGameModeStart();

		if (!IsMaster())
			return;

		this.votingWorldSystem.StartVote();
	};

	[ReceiverAttribute()]
	protected event void OnVoteEnded(notnull V30_MapVoting_Vote vote, int winnerChoiceId) {
		if (!IsMaster())
			return;

		if (!IsRunning()) {
			PrintFormat("[V30][MapVoting] Vote ended, but game mode is not running.", level: LogLevel.ERROR);
			return;
		};

		// TODO: get winnerScenario
		// TODO: send preview of winnerScenario to clients

		auto endGameModeData = SCR_GameModeEndData.Create(EGameOverTypes.V30_MAPVOTING_NEXT_MAP);
		EndGameMode(endGameModeData);
	};

	[ReceiverAttribute()]
	protected event void OnVoteCancelled(notnull V30_MapVoting_Vote vote);

	override protected void OnGameModeEnd(SCR_GameModeEndData endData) {
		if (!IsMaster()) {
			// Just call vanila implementation on clients.
			super.OnGameModeEnd(endData);
			return;
		};

		//if (!this.votingWorldSystem.IsVoteFinished()) {
		//	PrintFormat("[V30][MapVoting] Game mode ended, but vote is not finished. Ending or cancelling vote is required.", level: LogLevel.ERROR);
		//	return;
		//};

		//if (this.votingWorldSystem.IsVoteCancelled()) {
		//	// If vote is cancelled, just call vanila implementation, it's normal behaviour.
		//	super.OnGameModeEnd(endData);
		//	return;
		//};

		auto endReason = endData.GetEndReason();
		if (endReason != EGameOverTypes.V30_MAPVOTING_NEXT_MAP) {
			PrintFormat("[V30][MapVoting] Game mode is not ended with reason EGameOverTypes.%1 (%2).", SCR_Enum.GetEnumName(EGameOverTypes, EGameOverTypes.V30_MAPVOTING_NEXT_MAP), EGameOverTypes.V30_MAPVOTING_NEXT_MAP, level: LogLevel.ERROR);
			return;
		};

		// TODO: get winnerScenario

		// TODO: call runner.PrepareScenarioSwitch(winnerScenario)

		super.OnGameModeEnd(endData);
	};

	override protected void RestartSession() {
		if (!IsMaster()) {
			return;
		};

		//if (!this.votingWorldSystem.IsFinished()) {
		//	PrintFormat("[V30][MapVoting] RestartSession() is called, but vote is not finished. Ending or cancelling vote is required.", level: LogLevel.ERROR);
		//	return;
		//};

		if (this.votingWorldSystem.IsVoteCancelled()) {
			// If vote is cancelled, just call vanila implementation, it's normal behaviour.
			super.RestartSession();
			return;
		};

		// TODO: get winnerScenario

		// TODO: call runner.SwitchScenario(winnerScenario)
	};

	V30_MapVoting_VotingWorldSystem GetVotingWorldSystem() {
		return this.votingWorldSystem;
	};

	V30_MapVoting_Menu GetVotingMenu() {
		return this.votingMenu;
	};

	// TODO:
	// override float GetRemainingTime() {
	// 	if (!m_V30_MapVoting_gameModeComponent.IsTimeTicking())
	// 		return -1;

	// 	auto remainingTime = m_V30_MapVoting_gameModeComponent.GetRemainingTime();
	// 	return ((float)remainingTime) / 1000.0;
	// };
};

enum V30_MapVoting_EChoicesMode {
	TEST
}

enum V30_MapVoting_EGiveVoteAbility {
	TEST
}
