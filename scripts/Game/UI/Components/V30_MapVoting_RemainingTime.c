class V30_MapVoting_RemainingTimeComponentWidgetComponent : SCR_ScriptedWidgetComponent {
	protected V30_MapVoting_GameMode m_gameMode;
	
	override void HandlerAttached(Widget w) {
		super.HandlerAttached(w);
		
		m_gameMode = V30_MapVoting_GameMode.Cast(GetGame().GetGameMode());
		
		GetGame().GetCallqueue().CallLater(UpdateTimer, delay: 0.5 * 1000, repeat: true, param1: w);
	};
	
	protected void UpdateTimer(Widget w) {
		if (!w || !m_gameMode) {
			GetGame().GetCallqueue().Remove(UpdateTimer);
			return;
		};
		
		auto time = m_gameMode.GetVotingRemainingTime();
		if (time == int.MAX) {
			if (w.IsVisible()) w.SetVisible(false);
			return;
		}
		
		if (!w.IsVisible()) w.SetVisible(true);
		
		time /= 1000;
		auto minutes = (time / 60).ToString();
		auto seconds = (time % 60).ToString();
		
		if (minutes.Length() == 1) minutes = "0" + minutes;
		if (seconds.Length() == 1) seconds = "0" + seconds;
		
		TextWidget.Cast(w).SetText(string.Format("%1:%2", minutes, seconds));
	};

	//------------------------------------------------------------------------------------------------
	override void HandlerDeattached(Widget w) {
		super.HandlerDeattached(w);
		
		auto game = GetGame();
		if (!game) return;
		
		auto callqueue = game.GetCallqueue();
		if (!callqueue) return;
		
		callqueue.Remove(UpdateTimer);
	};
};
