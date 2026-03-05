modded class SCR_ServerRestartTimerUIComponent {
    protected V30_MapVoting_VotingWorldSystem m_VotingComponent;

    protected WorldTimestamp m_votingEndTimeStamp;

    protected bool m_VoteStarted;

    protected bool m_VoteEnded;

    override protected void Init() {
        super.Init();

        auto game = GetGame();
        auto gameMode = game.GetGameMode();
        auto votingComponent = V30_MapVoting_VotingWorldSystem.Cast(gameMode.FindComponent(V30_MapVoting_VotingWorldSystem));

        m_VotingComponent = votingComponent;
    };

    protected event void OnVoteStarted(notnull V30_MapVoting_VotingWorldSystem votingComponent) {
        m_VoteStarted = true;
    };

    protected event void OnVoteEnded(notnull V30_MapVoting_VotingWorldSystem votingComponent) {
        m_VoteStarted = true;
        m_VoteEnded = true;
    };

    override protected void UpdateTimer() {
        if (!m_VotingComponent)
            super.UpdateTimer();
        else if (!m_VoteStarted)
            m_wRestartTimer.SetTextFormat("<color rgba='%1'>Waiting voting to start...</color>", UIColors.FormatColor(GUIColors.ENABLED));
        else if (!m_VoteEnded) {
            m_wRestartTimer.SetTextFormat("Voting: <color rgba='%1'>%2</color>",  UIColors.FormatColor(GUIColors.ENABLED), SCR_FormatHelper.GetTimeFormatting(123, ETimeFormatParam.DAYS | ETimeFormatParam.HOURS));
        }
        else
            super.UpdateTimer();
	};
};
