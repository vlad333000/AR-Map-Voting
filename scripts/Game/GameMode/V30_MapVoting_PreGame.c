class V30_MapVoting_PreGameGameModeStateComponentClass : SCR_PreGameGameModeStateComponentClass {};

class V30_MapVoting_PreGameGameModeStateComponent : SCR_PreGameGameModeStateComponent {
    [Attribute("1", uiwidget: "slider", params: "0 128")]
    protected int m_PlayerCountToStart;

    override bool CanAdvanceState(SCR_EGameModeState nextState) {
        return super.CanAdvanceState(nextState) && GetGame().GetPlayerManager().GetPlayerCount() >= m_PlayerCountToStart;
    };
};
