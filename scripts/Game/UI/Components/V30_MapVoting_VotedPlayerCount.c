class V30_MapVoting_VotedPlayerCountComponent : V30_MapVoting_WidgetComponent {
	/*sealed*/ override void Setup(notnull V30_MapVoting_GameModeComponent votingComponent) {
		super.Setup(votingComponent);
		GetGame().GetCallqueue().CallLater(UpdatePlayerCount, delay: 1 * 1000, repeat: true);
	};

	/*sealed*/ protected /*private*/ void UpdatePlayerCount() {
		auto playerIds = new array<int>();
		GetGame().GetPlayerManager().GetPlayers(playerIds);

		int count = 0;
		foreach (int playerId : playerIds)
			if (m_VotingComponent.IsPlayerVoted(playerId))
				count++;

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
