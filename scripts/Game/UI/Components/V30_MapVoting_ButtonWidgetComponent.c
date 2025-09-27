class V30_MapVoting_ButtonWidgetComponent : V30_MapVoting_WidgetComponent {
	override void Setup(notnull V30_MapVoting_VotingComponent votingComponent) {
		super.Setup(votingComponent);
        auto widget = GetRootWidget();
        auto buttonWidgetComponent = V30_MapVoting_WidgetHandlerHelperT<SCR_ButtonBaseComponent>.FindHandler(widget);
        auto onButtonWidgetComponentClicked = buttonWidgetComponent.m_OnClicked;
        onButtonWidgetComponentClicked.Insert(OnButtonClicked);
	};

    /*modded*/ protected event void OnButtonClicked(notnull SCR_ButtonBaseComponent buttonWidgetComponent);

	override event void HandlerDeattached(Widget w) {
        if (IsSetup())
            V30_MapVoting_WidgetHandlerHelperT<SCR_ButtonBaseComponent>.FindHandler(w).m_OnClicked.Remove(OnButtonClicked);
		super.HandlerDeattached(w);
	};
};
