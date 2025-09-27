class V30_MapVoting_ScreenUIComponent : SCR_ScriptedWidgetComponent {
    /*modded*/ void SetupScreen(notnull V30_MapVoting_GameModeComponent votingComponent);

    /*modded*/ void ClearScreen();

    /*modded*/ V30_MapVoting_ChoiceUIComponent FindChoice(V30_MapVoting_ChoiceId choiceId);

	/*sealed*/ static V30_MapVoting_ScreenUIComponent GetInstance(notnull Widget widget) {
		return V30_MapVoting_ScreenUIComponent.Cast(widget.FindHandler(V30_MapVoting_ScreenUIComponent));
	};
};

class V30_MapVoting_SimpleScreenUIComponent : V30_MapVoting_ScreenUIComponent {
    [Attribute("", desc: "Content widget name")]
    protected /*private*/ string m_ContentWidgetName;

    protected /*private*/ Widget m_ContentWidget;

    [Attribute("1", desc: "Content widget's grid columns count", params: "1 inf")]
    protected /*private*/ int m_ContentColumns;

    [Attribute("{1258C211D43EEB69}UI/layouts/V30/MapVoting/V30_MapVoting_Choice.layout", params: "layout")]
    protected /*private*/ ResourceName m_ChoiceLayout;

    protected /*private*/ V30_MapVoting_GameModeComponent m_VotingComponent;

    protected /*private*/ int m_ChoiceIndex;

	/*sealed*/ override event void HandlerAttached(Widget w) {
        super.HandlerAttached(w);

        m_ContentWidget = w.FindAnyWidget(m_ContentWidgetName);
        m_ChoiceIndex = 0;
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

    /*sealed*/ override void SetupScreen(notnull V30_MapVoting_GameModeComponent votingComponent) {
        m_VotingComponent = votingComponent;
        if (!votingComponent.IsAllChoicesLoaded()) {
            votingComponent.GetOnAllChoicesLoaded().Insert(OnAllChoicesLoaded);
            return;
        };

        auto choices = votingComponent.GetAllChoices();
        OnAllChoicesLoaded(votingComponent, choices);
    };

    /*sealed*/ protected /*private*/ event void OnAllChoicesLoaded(notnull V30_MapVoting_GameModeComponent votingComponent, notnull map<V30_MapVoting_ChoiceId, ref V30_MapVoting_Choice> choices) {
        foreach (auto choiceId, auto choice : choices)
            AddChoice(choiceId);
    };

    /*sealed*/ protected /*private*/ V30_MapVoting_ChoiceUIComponent AddChoice(V30_MapVoting_ChoiceId choiceId) {
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
};
