class V30_MapVoting_EndVoteButtonWidgetComponent : V30_MapVoting_ButtonWidgetComponent {
    /*sealed*/ override void Setup(notnull V30_MapVoting_GameModeComponent votingComponent) {
		super.Setup(votingComponent);
		GetGame().GetCallqueue().CallLater(UpdateButtonAllowance, repeat: true, delay: 0.5 * 1000);
	};

    /*sealed*/ protected /*private*/ void UpdateButtonAllowance() {
        auto widget = GetRootWidget();
		auto isAdmin = SCR_Global.IsAdmin();
		if (widget.IsEnabled() != isAdmin) {
			widget.SetVisible(isAdmin);
			widget.SetEnabled(isAdmin);
		};
    };

    /*sealed*/ override protected event void OnButtonClicked(notnull SCR_ButtonBaseComponent buttonWidgetComponent) {
        if (SCR_Global.IsAdmin())
            V30_MapVoting_PlayerControllerComponent.GetLocalInstance().EndVote();
    };

	override event void HandlerDeattached(Widget w) {
        if (IsSetup())
            GetGame().GetCallqueue().Remove(UpdateButtonAllowance);
		super.HandlerDeattached(w);
	};
};
