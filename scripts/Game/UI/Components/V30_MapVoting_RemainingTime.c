class V30_MapVoting_RemainingTimeComponentWidgetComponent : V30_MapVoting_WidgetComponent {
	protected /*private*/ V30_MapVoting_GameMode m_VotingGameMode;

	/*sealed*/ override void Setup(notnull V30_MapVoting_GameModeComponent votingComponent) {
		super.Setup(votingComponent);
		m_VotingGameMode = V30_MapVoting_GameMode.Cast(votingComponent.GetOwner());
		GetGame().GetCallqueue().CallLater(UpdateTimer, delay: 0.5 * 1000, repeat: true);
	};

	/*sealed*/ V30_MapVoting_GameMode GetVotingGameMode() {
		return m_VotingGameMode;
	};

	/*sealed*/ protected /*private*/ void UpdateTimer() {
		auto w = GetRootWidget();

		if (!w || !m_VotingGameMode) {
			GetGame().GetCallqueue().Remove(UpdateTimer);
			return;
		};

		auto time = m_VotingGameMode.GetVotingRemainingTime();
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

	override event void HandlerDeattached(Widget w) {
		if (IsSetup()) {
			m_VotingGameMode = null;
			GetGame().GetCallqueue().Remove(UpdateTimer);
		};
		super.HandlerDeattached(w);
	};
};
