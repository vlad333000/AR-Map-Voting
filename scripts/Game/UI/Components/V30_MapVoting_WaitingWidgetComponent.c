class V30_MapVoting_WaitingWidgetComponent : V30_MapVoting_WidgetComponent {
    [Attribute("Message")]
    protected string m_MessageWidgetName;

    protected TextWidget m_MessageWidget;

    protected V30_MapVoting_PreGameGameModeStateComponent m_PreGameGameModeStateComponent;



	override void HandlerAttached(Widget w) {
        super.HandlerAttached(w);

        m_MessageWidget = V30_MapVoting_WidgetHelperT<TextWidget>.FindAnyWidget(w, m_MessageWidgetName);
    };

    void SetMessage(string text) {
        m_MessageWidget.SetText(text);
    };



    override void Setup(notnull V30_MapVoting_VotingComponent votingComponent) {
        super.Setup(votingComponent);

        auto owner = votingComponent.GetOwner();
        auto gameMode = SCR_BaseGameMode.Cast(owner);
        if (!gameMode)
            return;

        auto preGameGameStateComponent = gameMode.GetStateComponent(SCR_EGameModeState.PREGAME);
        m_PreGameGameModeStateComponent = V30_MapVoting_PreGameGameModeStateComponent.Cast(preGameGameStateComponent);
        if (!m_PreGameGameModeStateComponent)
			return;

        m_PreGameGameModeStateComponent.GetOnMessageUpdated().Insert(OnMessageUpdated);
		auto message = m_PreGameGameModeStateComponent.GetMessage();
		OnMessageUpdated(m_PreGameGameModeStateComponent, message);
    };

    void OnMessageUpdated(notnull V30_MapVoting_PreGameGameModeStateComponent preGameGameModeStateComponent, string message) {
        SetMessage(message);
    };
};
