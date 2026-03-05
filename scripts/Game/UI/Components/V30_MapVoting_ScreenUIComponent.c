class V30_MapVoting_ScreenUIComponent : SCR_ScriptedWidgetComponent {
    protected /*private*/ V30_MapVoting_VotingWorldSystem m_VotingComponent;

    [Attribute("", desc: "Content widget name")]
    protected /*private*/ string m_ContentWidgetName;

    protected /*private*/ Widget m_ContentWidget;

    [Attribute("1", desc: "Content widget's grid columns count", params: "1 inf")]
    protected /*private*/ int m_ContentColumns;

    [Attribute("{1258C211D43EEB69}UI/layouts/V30/MapVoting/V30_MapVoting_Choice.layout", params: "layout")]
    protected /*private*/ ResourceName m_ChoiceLayout;

    protected /*private*/ ref map<int, V30_MapVoting_ChoiceUIComponent> m_ChoiceUiComponents;

    protected /*private*/ int m_ChoiceIndex;

	/*sealed*/ override event void HandlerAttached(Widget w) {
        super.HandlerAttached(w);

        m_ContentWidget = w.FindAnyWidget(m_ContentWidgetName);
        m_ChoiceIndex = 0;
        m_ChoiceUiComponents = new map<int, V30_MapVoting_ChoiceUIComponent>();
    };

    /*sealed*/ string GetContentWidgetName() {
        return m_ContentWidgetName;
    };

    /*sealed*/ Widget GetContentWidget() {
        return m_ContentWidget;
    };

    /*selaed*/ ResourceName GetChoiceLayout() {
        return m_ChoiceLayout;
    };

    /*sealed*/ V30_MapVoting_VotingWorldSystem GetVotingComponent() {
        return m_VotingComponent;
    };

    /*modded*/ void Setup(notnull V30_MapVoting_VotingWorldSystem votingWorldSystem) {
        m_VotingComponent = votingWorldSystem;
    };

    void GetChoices(notnull array<V30_MapVoting_ChoiceUIComponent> handlers) {
        auto widget = GetRootWidget();
        V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_ChoiceUIComponent>.FindChildrenHandlers(widget, handlers, true);
    };

    V30_MapVoting_ChoiceUIComponent GetFocusedChoice() {
        auto focusedWidget = GetGame().GetWorkspace().GetFocusedWidget();
        if (!focusedWidget)
            return null;

        auto focusedChoice = V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_ChoiceUIComponent>.FindHandler(focusedWidget);
        if (!focusedChoice)
            return null;

        auto screenWidget = GetRootWidget();
        auto parentWidget = focusedWidget.GetParent();
        while (parentWidget) {
            if (screenWidget == parentWidget)
                return focusedChoice;
            parentWidget = parentWidget.GetParent();
        };
        return null;
    };

    /*sealed*/ V30_MapVoting_ChoiceUIComponent GetChoice(int choiceId) {
        return m_ChoiceUiComponents.Get(choiceId);
    };

    /*sealed*/ protected /*private*/ V30_MapVoting_ChoiceUIComponent AddChoice(int choiceId) {
		auto choiceWidget = GetGame().GetWorkspace().CreateWidgets(m_ChoiceLayout, m_ContentWidget);
        auto row = m_ChoiceIndex / m_ContentColumns;
        auto col = m_ChoiceIndex % m_ContentColumns;
        UniformGridSlot.SetRow(choiceWidget, row);
        UniformGridSlot.SetColumn(choiceWidget, col);
        m_ChoiceIndex++;

        auto choiceUiComponent = V30_MapVoting_ChoiceUIComponent.GetInstance(choiceWidget);
        choiceUiComponent.SetupChoice(m_VotingComponent, choiceId);

        return choiceUiComponent;
    };

    /*sealed*/ protected /*private*/ event void OnAllChoicesLoaded(notnull V30_MapVoting_VotingWorldSystem votingComponent, notnull map<int, ref V30_MapVoting_Choice> choices) {
        foreach (auto choiceId, auto choice : choices)
            m_ChoiceUiComponents.Set(choiceId, AddChoice(choiceId));
    };
};

class V30_MapVoting_SimpleScreenUIComponent : V30_MapVoting_ScreenUIComponent {
    [Attribute("")]
    protected /*private*/ string m_TimerWidgetName;

    protected /*private*/ Widget m_TimerWidget;

    [Attribute("")]
    protected /*private*/ string m_VotedPlayerCountWidgetName;

    protected /*private*/ Widget m_VotedPlayerCountWidget;

    [Attribute("")]
    protected /*private*/ string m_VotingPlayerCountWidgetName;

    protected /*private*/ Widget m_VotingPlayerCountWidget;

    [Attribute("")]
    protected /*private*/ string m_VoteWidgetName;

    protected /*private*/ Widget m_VoteWidget;

    [Attribute("")]
    protected /*private*/ string m_RemoveVoteWidgetName;

    protected /*private*/ Widget m_RemoveVoteWidget;

    [Attribute("")]
    protected /*private*/ string m_EndVoteWidgetName;

    protected /*private*/ Widget m_EndVoteWidget;

	/*sealed*/ override event void HandlerAttached(Widget w) {
        super.HandlerAttached(w);

        m_TimerWidget = w.FindAnyWidget(m_TimerWidgetName);
        m_VotedPlayerCountWidget = w.FindAnyWidget(m_VotedPlayerCountWidgetName);
        m_VotingPlayerCountWidget = w.FindAnyWidget(m_VotingPlayerCountWidgetName);
        m_VoteWidget = w.FindAnyWidget(m_VoteWidgetName);
        m_RemoveVoteWidget = w.FindAnyWidget(m_RemoveVoteWidgetName);
        m_EndVoteWidget = w.FindAnyWidget(m_EndVoteWidgetName);
    };

    /*sealed*/ override void Setup(notnull V30_MapVoting_VotingWorldSystem votingWorldSystem) {
		super.Setup(votingWorldSystem);

        if (m_TimerWidget)
			V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_RemainingTimeComponentWidgetComponent>.FindHandler(m_TimerWidget).Setup(votingWorldSystem);
        if (m_VotedPlayerCountWidget)
			V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_VotedPlayerCountComponent>.FindHandler(m_VotedPlayerCountWidget).Setup(votingWorldSystem);
        if (m_VotingPlayerCountWidget)
			V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_VotingPlayerCountComponent>.FindHandler(m_VotingPlayerCountWidget).Setup(votingWorldSystem);
        if (m_VoteWidget)
			V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_VoteButtonWidgetComponent>.FindHandler(m_VoteWidget).Setup(votingWorldSystem);
        if (m_RemoveVoteWidget)
			V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_RemoveVoteButtonWidgetComponent>.FindHandler(m_RemoveVoteWidget).Setup(votingWorldSystem);
        if (m_EndVoteWidget)
			V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_EndVoteButtonWidgetComponent>.FindHandler(m_EndVoteWidget).Setup(votingWorldSystem);
    };
};

class V30_MapVoting_GameoverScreenUIComponent : V30_MapVoting_ScreenUIComponent {

};
