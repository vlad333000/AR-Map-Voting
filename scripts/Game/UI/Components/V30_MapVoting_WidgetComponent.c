class V30_MapVoting_WidgetComponent : SCR_ScriptedWidgetComponent {
    protected /*private*/ V30_MapVoting_VotingWorldSystem m_VotingWorldSystem;

    /*modded*/ void Setup(notnull V30_MapVoting_VotingWorldSystem votingWorldSystem) {
        m_VotingWorldSystem = votingWorldSystem;
    };

    /*sealed*/ bool IsSetup() {
        return GetVotingComponent();
    };

    /*sealed*/ V30_MapVoting_VotingWorldSystem GetVotingComponent() {
        return m_VotingWorldSystem;
    };

	override void HandlerDeattached(Widget w) {
		m_VotingWorldSystem = null;
		super.HandlerDeattached(w);
	};
};
