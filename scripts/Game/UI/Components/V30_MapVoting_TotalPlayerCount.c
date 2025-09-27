class V30_MapVoting_TotalPlayerCountComponent : SCR_ScriptedWidgetComponent {
	override void HandlerAttached(Widget w) {
		super.HandlerAttached(w);
		GetGame().GetCallqueue().CallLater(UpdatePlayerCount, delay: 1 * 1000, repeat: true);
	};

	/*sealed*/ protected /*private*/ void UpdatePlayerCount() {
		auto playerCount = GetGame().GetPlayerManager().GetPlayerCount();
		auto text = playerCount.ToString();
		SetText(text);
	};

	/*sealed*/ protected /*private*/ void SetText(string text) {
		auto widget = GetRootWidget();
		auto textWidget = TextWidget.Cast(widget);
		textWidget.SetText(text);
	};

	override void HandlerDeattached(Widget w) {
		GetGame().GetCallqueue().Remove(UpdatePlayerCount);
		super.HandlerDeattached(w);
	};
};
