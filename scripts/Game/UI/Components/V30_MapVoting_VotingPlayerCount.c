class V30_MapVoting_VotingPlayerCountComponent : V30_MapVoting_WidgetComponent {
	/*sealed*/ override void Setup(notnull V30_MapVoting_VotingComponent votingComponent) {
		super.Setup(votingComponent);
		GetGame().GetCallqueue().CallLater(UpdatePlayerCount, delay: 1 * 1000, repeat: true);
	};

	/*sealed*/ protected /*private*/ void UpdatePlayerCount() {
		auto votingComponent = GetVotingComponent();
		int count = votingComponent.CountPlayersWithVoteAbility();
		auto text = count.ToString();
		SetText(text);
	};

	/*sealed*/ protected /*private*/ void SetText(string text) {
		auto widget = GetRootWidget();
		auto textWidget = TextWidget.Cast(widget);
		textWidget.SetText(text);
	};

	override void HandlerDeattached(Widget w) {
		if (IsSetup())
			GetGame().GetCallqueue().Remove(UpdatePlayerCount);
		super.HandlerDeattached(w);
	};
};
