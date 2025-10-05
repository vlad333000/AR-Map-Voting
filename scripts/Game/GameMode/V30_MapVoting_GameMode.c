// TODO: Move logic to GameSystem

[EntityEditorProps(category: "GameScripted/Misc", description: "")]
class V30_MapVoting_GameModeClass : SCR_BaseGameModeClass {
};

class V30_MapVoting_GameMode : SCR_BaseGameMode {
	[Attribute(desc: "Time between vote is ended and starting of loading next scenario (in ms)", defvalue: "14000", params: "0 inf")]
	protected int m_V30_MapVoting_startVotedMissionDelay;

	protected V30_MapVoting_Menu m_Menu;

	protected V30_MapVoting_ScenarioVotingComponent m_V30_MapVoting_gameModeComponent;

	void V30_MapVoting_GameMode(IEntitySource src, IEntity parent) {
		if (SCR_Global.IsEditMode())
			return;

		SetEventMask(EntityEvent.INIT);
	};

	override protected event void EOnInit(IEntity owner) {
		super.EOnInit(owner);

		if (SCR_Global.IsEditMode())
			return;

		m_V30_MapVoting_gameModeComponent = V30_MapVoting_ScenarioVotingComponent.Cast(FindComponent(V30_MapVoting_ScenarioVotingComponent));
		m_V30_MapVoting_gameModeComponent.GetOnVoteEnded().Insert(OnVoteEnded);

		if (System.IsConsoleApp())
			return;

		m_Menu = V30_MapVoting_Menu.Cast(GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.V30_MapVoting_Menu));
		m_Menu.Setup(m_V30_MapVoting_gameModeComponent);
	};

	protected V30_MapVoting_ScenarioVotingComponent V30_MapVoting_GetGameModeComponent() {
		return m_V30_MapVoting_gameModeComponent;
	};

	override protected void OnGameModeStart() {
		super.OnGameModeStart();

		if (IsMaster() && m_V30_MapVoting_gameModeComponent.IsAutoStartEnabled()) {
			m_V30_MapVoting_gameModeComponent.StartVote();
		};
	};

	override protected void OnGameModeEnd(SCR_GameModeEndData endData) {
		super.OnGameModeEnd(endData);

		if (!IsMaster()) {
			return;
		};

		if (!m_V30_MapVoting_gameModeComponent.IsVoteEnded()) {
			m_V30_MapVoting_gameModeComponent.EndVote();
		};

		GetGame().GetCallqueue().Remove(V30_MapVoting_StartVotingMission);
	};

	override float GetAutoReloadDelay() {
		if (m_V30_MapVoting_overrideOnGameModeEnd) {
			return super.GetAutoReloadDelay();
		};

		return ((float)m_V30_MapVoting_startVotedMissionDelay) / 1000.0;
	};

	protected void V30_MapVoting_StartVotedMission() {
		if (!IsMaster())
			return;

		m_V30_MapVoting_gameModeComponent.RunWinner();
	};

	protected event void OnVoteEnded(V30_MapVoting_VotingComponent votingGameModeComponent, V30_MapVoting_ChoiceId winnerId) {
		if (IsRunning()) {
			EndGameMode(SCR_GameModeEndData.CreateSimple(EGameOverTypes.V30_MAPVOTING_NEXT_MAP));
		};

		GetGame().GetCallqueue().CallLater(V30_MapVoting_StartVotedMission, m_V30_MapVoting_startVotedMissionDelay, false);
	};

	override float GetRemainingTime() {
		if (!m_V30_MapVoting_gameModeComponent.IsTimeTicking())
			return -1;

		auto remainingTime = m_V30_MapVoting_gameModeComponent.GetRemainingTime();
		return ((float)remainingTime) / 1000.0;
	};
};
