class V30_MapVoting_MenuUIComponent : SCR_ScriptedWidgetComponent {
    protected V30_MapVoting_VotingComponent m_VotingComponent;

    [Attribute("Waiting")]
    protected string m_WaitingWidgetName;

    protected Widget m_WaitingWidget;

    [Attribute("Content")]
    protected string m_ContentWidgetName;

    protected Widget m_ContentWidget;

    protected Widget m_ScreenWidget;

	override void HandlerAttached(Widget w) {
		super.HandlerAttached(w);

        m_WaitingWidget = w.FindAnyWidget(m_WaitingWidgetName);
		m_ContentWidget = w.FindAnyWidget(m_ContentWidgetName);

        m_WaitingWidget.SetVisible(true);
        m_WaitingWidget.SetEnabled(true);

        m_ContentWidget.SetVisible(false);
        m_ContentWidget.SetEnabled(false);
	};

    /*saeled*/ void Setup(notnull V30_MapVoting_VotingComponent votingComponent) {
        m_VotingComponent = votingComponent;
        m_VotingComponent.GetOnVoteStarted().Insert(OnVoteStarted);
        auto waitingWidgetComponent = V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_WaitingWidgetComponent>.FindHandler(m_WaitingWidget);
        waitingWidgetComponent.Setup(votingComponent);
    };

    protected /*private*/ event void OnVoteStarted(notnull V30_MapVoting_VotingComponent votingComponent) {
        m_WaitingWidget.SetVisible(false);
        m_WaitingWidget.SetEnabled(false);

        m_ContentWidget.SetVisible(true);
        m_ContentWidget.SetEnabled(true);

		SCR_UISoundEntity.SoundEvent(SCR_SoundEvent.SOUND_HUD_MAP_OPEN);
    };
};
