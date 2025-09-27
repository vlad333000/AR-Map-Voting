class V30_MapVoting_WidgetComponent : SCR_ScriptedWidgetComponent {
    protected /*private*/ V30_MapVoting_GameModeComponent m_VotingComponent;

    /*modded*/ void Setup(notnull V30_MapVoting_GameModeComponent votingComponent) {
        m_VotingComponent = votingComponent;
    };

    /*sealed*/ bool IsSetup() {
        return GetVotingComponent();
    };

    /*sealed*/ V30_MapVoting_GameModeComponent GetVotingComponent() {
        return m_VotingComponent;
    };

	override void HandlerDeattached(Widget w) {
		m_VotingComponent = null;
		super.HandlerDeattached(w);
	};
};
