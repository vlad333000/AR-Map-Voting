class V30_MapVoting_VotingPlayerCountComponent : SCR_ScriptedWidgetComponent {
	protected TextWidget m_Root;
	
	protected V30_MapVoting_GameModeComponent m_GameModeComponent;
	
	override void HandlerAttached(Widget w) {
		super.HandlerAttached(w);
		m_Root = TextWidget.Cast(w);
		m_GameModeComponent = V30_MapVoting_GameModeComponent.GetInstance();
		GetGame().GetCallqueue().CallLater(UpdatePlayerCount, delay: 1 * 1000, repeat: true);
	};
	
	protected void UpdatePlayerCount() {
		if (!m_Root) return;
		if (!m_GameModeComponent) return;
		m_Root.SetText(m_GameModeComponent.CountPlayersWithVoteAbility().ToString());
	};

	override void HandlerDeattached(Widget w) {
		super.HandlerDeattached(w);
		m_Root = null;
		GetGame().GetCallqueue().Remove(UpdatePlayerCount);
	}
}
