class V30_MapVoting_TotalPlayerCountComponent : SCR_ScriptedWidgetComponent {
	protected TextWidget m_V30_MapVoting_Root;

	override void HandlerAttached(Widget w) {
		super.HandlerAttached(w);
		m_V30_MapVoting_Root = TextWidget.Cast(w);
		GetGame().GetCallqueue().CallLater(UpdatePlayerCount, delay: 1 * 1000, repeat: true);
	};

	protected void UpdatePlayerCount() {
		if (!m_V30_MapVoting_Root) return;
		m_V30_MapVoting_Root.SetText(GetGame().GetPlayerManager().GetPlayerCount().ToString());
	};

	override void HandlerDeattached(Widget w) {
		super.HandlerDeattached(w);
		m_V30_MapVoting_Root = null;
		GetGame().GetCallqueue().Remove(UpdatePlayerCount);
	}
}
