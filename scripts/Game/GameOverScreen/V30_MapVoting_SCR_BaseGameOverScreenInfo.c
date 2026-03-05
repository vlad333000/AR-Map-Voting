[BaseContainerProps(), SCR_BaseContainerCustomTitleEnum(EGameOverTypes, "m_GameOverScreenId")]
modded class SCR_BaseGameOverScreenInfo {
    [Attribute("1", desc: "Enables voting for next scenario using V30 Map Voting addon.")]
    protected bool m_bNextScenarioVotingEnabled;

    bool IsNextScenarioVotingEnabled() {
        return m_bNextScenarioVotingEnabled;
    };
};
