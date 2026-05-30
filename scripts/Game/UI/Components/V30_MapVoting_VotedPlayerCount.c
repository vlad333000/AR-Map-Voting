class V30_MapVoting_VotedPlayerCountComponent : SCR_ScriptedWidgetComponent {
	protected TextWidget m_V30_MapVoting_Root;

	protected V30_MapVoting_GameModeComponent m_V30_MapVoting_GameModeComponent;

	override void HandlerAttached(Widget w) {
		super.HandlerAttached(w);
		m_V30_MapVoting_Root = TextWidget.Cast(w);
		m_V30_MapVoting_GameModeComponent = V30_MapVoting_GameModeComponent.GetInstance();
		GetGame().GetCallqueue().CallLater(UpdatePlayerCount, delay: 1 * 1000, repeat: true);
	};

	protected void UpdatePlayerCount() {
		if (!m_V30_MapVoting_Root) {
			GetGame().GetCallqueue().Remove(UpdatePlayerCount);
			return;
		};
		if (!m_V30_MapVoting_GameModeComponent) {
			m_V30_MapVoting_GameModeComponent = V30_MapVoting_GameModeComponent.GetInstance();
			if (!m_V30_MapVoting_GameModeComponent) return;
		};

		auto players = new array<int>();
		GetGame().GetPlayerManager().GetPlayers(players);
		int count = 0;
		foreach (int playerId : players) {
			if (m_V30_MapVoting_GameModeComponent.IsPlayerVoted(playerId)) {
				count++;
			};
		};

		m_V30_MapVoting_Root.SetText(count.ToString());
	};

	override void HandlerDeattached(Widget w) {
		m_V30_MapVoting_Root = null;
		m_V30_MapVoting_GameModeComponent = null;
		GetGame().GetCallqueue().Remove(UpdatePlayerCount);
		super.HandlerDeattached(w);
	}
}
