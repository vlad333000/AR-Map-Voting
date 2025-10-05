class V30_MapVoting_SimplePreGameGameModeStateComponentClass : V30_MapVoting_PreGameGameModeStateComponentClass {};

class V30_MapVoting_SimplePreGameGameModeStateComponent : V30_MapVoting_PreGameGameModeStateComponent {
    [Attribute("1", uiwidget: "slider", params: "0 128")]
    protected /*private*/ int m_RequiredPlayerCount;

    [Attribute("10000.0", uiwidget: "slider", params: "0.0 3600000.0 1.0")]
    protected /*private*/ float m_StartDelay;

    protected /*private*/ V30_MapVoting_WorldTimestampSyncComponent m_WorldTimestampSyncComponent;

    protected /*private*/ RplComponent m_RplComponent;

    protected /*private*/ V30_MapVoting_PreGameGameModeState_State m_State;

    protected /*private*/ int m_PlayerCountLeft;

    protected /*private*/ WorldTimestamp m_StartWorldTimestamp;



    void V30_MapVoting_SimplePreGameGameModeStateComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {
        m_State = V30_MapVoting_PreGameGameModeState_State.WAITING_PLAYERS;
    };

    override protected event void OnPostInit(IEntity owner) {
        super.OnPostInit(owner);
        SetEventMask(owner, EntityEvent.INIT);
    };

    override protected event void EOnInit(IEntity owner) {
        super.EOnInit(owner);
        m_RplComponent = V30_MapVoting_ComponentHelper<RplComponent>.FindComponent(owner);
        m_WorldTimestampSyncComponent = V30_MapVoting_ComponentHelper<V30_MapVoting_WorldTimestampSyncComponent>.FindComponent(owner);

        if (IsProxy())
            return;

        m_VotingComponent.GetOnPlayerVoteAbilityChanged().Insert(OnPlayerVoteAbilityChanged);
        auto playersWithVoteAbility = new array<int>();
        m_VotingComponent.GetPlayersWithVoteAbility(playersWithVoteAbility);
        foreach (auto player : playersWithVoteAbility)
            OnPlayerVoteAbilityChanged(m_VotingComponent, player, true);
    };

    override protected event void EOnFrame(IEntity owner, float timeSlice) {
        super.EOnFrame(owner, timeSlice);
		auto message = GetMessage();
        OnMessageUpdated(message);
    };

    override protected event void EOnFixedFrame(IEntity owner, float timeSlice) {
        super.EOnFixedFrame(owner, timeSlice);
        auto now = GetGame().GetWorld().GetTimestamp();
        auto start = m_StartWorldTimestamp;
        if (now.GreaterEqual(start))
            SetState(V30_MapVoting_PreGameGameModeState_State.FINISHED);
    };

    override protected event void OnDelete(IEntity owner) {
        m_VotingComponent.GetOnPlayerVoteAbilityChanged().Remove(OnPlayerVoteAbilityChanged);
        super.OnDelete(owner);
    };



    /*selaed*/ int GetRequiredPlayerCount() {
        return m_RequiredPlayerCount;
    };

    /*sealed*/ int GetStartDelay() {
        return m_StartDelay;
    };



    /*sealed*/ V30_MapVoting_WorldTimestampSyncComponent GetWorldTimestampSyncComponent() {
        return m_WorldTimestampSyncComponent;
    };

    /*sealed*/ V30_MapVoting_PreGameGameModeState_State GetState() {
        return m_State;
    };



    protected /*private*/ void SetPlayerCountLeft(int playerCountLeft) {
        Rpc(RpcDo_SetPlayerCountLeft, playerCountLeft);
        RpcDo_SetPlayerCountLeft(playerCountLeft);
    };

    [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_SetPlayerCountLeft(int playerCountLeft) {
        m_PlayerCountLeft = playerCountLeft;
		auto message = GetMessage();
        OnMessageUpdated(message);
    };

    protected /*private*/ void SetState(V30_MapVoting_PreGameGameModeState_State state) {
        Rpc(RpcDo_SetState, state);
        RpcDo_SetState(state);
    };

    [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_SetState(V30_MapVoting_PreGameGameModeState_State state) {
        m_State = state;

        auto owner = GetOwner();
        switch (state) {
            case V30_MapVoting_PreGameGameModeState_State.WAITING_PLAYERS: {
				if (!System.IsConsoleApp())
                	ClearEventMask(owner, EntityEvent.FRAME);
                ClearEventMask(owner, EntityEvent.FIXEDFRAME);
                break;
            };
            case V30_MapVoting_PreGameGameModeState_State.WAITING_DELAY: {
				auto now = GetGame().GetWorld().GetTimestamp();
				m_StartWorldTimestamp = now.PlusMilliseconds(m_StartDelay);
                if (m_StartDelay > 0.0) {
					if (!System.IsConsoleApp())
                		SetEventMask(owner, EntityEvent.FRAME);
                    SetEventMask(owner, EntityEvent.FIXEDFRAME);
				}
                else if (!IsProxy()) {
                    SetState(V30_MapVoting_PreGameGameModeState_State.FINISHED);
                    return;
                };
                break;
            };
            case V30_MapVoting_PreGameGameModeState_State.FINISHED: {
				if (!System.IsConsoleApp())
                	ClearEventMask(owner, EntityEvent.FRAME);
                ClearEventMask(owner, EntityEvent.FIXEDFRAME);
                Finish();
                break;
            };
        };
		auto message = GetMessage();
        OnMessageUpdated(message);
    };

    protected /*private*/ event void OnPlayerVoteAbilityChanged(notnull V30_MapVoting_VotingComponent votingComponent, int playerId, bool hasVoteAbility) {
        switch (m_State) {
            case V30_MapVoting_PreGameGameModeState_State.WAITING_PLAYERS: {
                auto playerCount = votingComponent.CountPlayersWithVoteAbility();
                auto requiredPlayerCount = GetRequiredPlayerCount();
                if (playerCount >= requiredPlayerCount)
                    SetState(V30_MapVoting_PreGameGameModeState_State.WAITING_DELAY);
                else
                    SetPlayerCountLeft(requiredPlayerCount - playerCount);
                break;
            };
            case V30_MapVoting_PreGameGameModeState_State.WAITING_DELAY: {
                auto playerCount = votingComponent.CountPlayersWithVoteAbility();
                auto requiredPlayerCount = GetRequiredPlayerCount();
                if (playerCount < requiredPlayerCount)
                    SetState(V30_MapVoting_PreGameGameModeState_State.WAITING_PLAYERS);
                break;
            };
        };
    };



    override string GetMessage() {
        switch (m_State) {
            case V30_MapVoting_PreGameGameModeState_State.WAITING_PLAYERS: {
                auto format = "#AR-V30_MapVoting_SimplePreGame_WaitingPlayers";
                auto playerCountLeft = m_PlayerCountLeft.ToString();
                return SCR_StringHelper.Translate(format, playerCountLeft);
            };
            case V30_MapVoting_PreGameGameModeState_State.WAITING_DELAY: {
                auto format = "#AR-V30_MapVoting_SimplePreGame_WaitingDelay";
                auto now = m_WorldTimestampSyncComponent.GetSyncedWorldTimestamp();
                auto start = m_StartWorldTimestamp;
                int time = start.DiffSeconds(now) + 1;
                if (time < 1)
                    time = 1;
                auto minutes = time / 60;
                auto seconds = time % 60;
				auto timeStr = seconds.ToString();
                if (minutes != 0) {
                	if (timeStr.Length() < 2)
                    	timeStr = "0" + timeStr;
					timeStr = minutes.ToString() + ":" + timeStr;
				};
                return SCR_StringHelper.Translate(format, timeStr);
            };
        };
        return "";
    };



    protected /*sealed*/ bool IsProxy() {
        return m_RplComponent && m_RplComponent.IsProxy();
    };
};

enum V30_MapVoting_PreGameGameModeState_State {
    INITIAL,
    WAITING_PLAYERS,
    WAITING_DELAY,
    FINISHED
};
