modded class SCR_BaseGameMode {
	protected bool m_V30_MapVoting_overrideOnGameModeEnd = false;

	protected V30_MapVoting_VotingWorldSystem m_V30_MapVoting_votingComponent;

	override protected void OnGameModeEnd(SCR_GameModeEndData endData) {
		// TODO: Change to normal code when BIS allows to disable all actions in super.OnGameModeEnd
		m_V30_MapVoting_overrideOnGameModeEnd = true;
		super.OnGameModeEnd(endData);
		m_V30_MapVoting_overrideOnGameModeEnd = false;

		m_V30_MapVoting_votingComponent = V30_MapVoting_VotingWorldSystem.Cast(FindComponent(V30_MapVoting_VotingWorldSystem));
		if (!m_V30_MapVoting_votingComponent)
			return;

		if (!IsMaster())
			return;
	};

	override float GetAutoReloadDelay() {
		// Force to call RestartSession in super.OnGameModeEnd
		if (m_V30_MapVoting_overrideOnGameModeEnd)
			return 0;

		return super.GetAutoReloadDelay();
	};

	override protected void RestartSession() {
		// Cancel RestartSession in super.OnGameModeEnd
		if (m_V30_MapVoting_overrideOnGameModeEnd)
			return;

		super.RestartSession();
	};

	protected event void V30_MapVoting_OnVoteEnded(int winnerId) {
		float reloadTime = Math.Max(5, GetAutoReloadDelay());

		GetGame().GetCallqueue().CallLater(V30_MapVoting_StartVotedMission, reloadTime * 1000, false);
	};

	protected event void V30_MapVoting_StartVotedMission() {
		if (!IsMaster())
			return;
	};
};
