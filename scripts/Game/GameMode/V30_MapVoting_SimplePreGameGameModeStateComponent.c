class V30_MapVoting_SimplePreGameGameModeStateComponentClass : V30_MapVoting_PreGameGameModeStateComponentClass {};

class V30_MapVoting_SimplePreGameGameModeStateComponent : V30_MapVoting_PreGameGameModeStateComponent {
    [Attribute("1", uiwidget: "slider", params: "0 128")]
    protected /*private*/ int m_RequiredPlayerCount;

    [Attribute("10000.0", uiwidget: "slider", params: "0.0 3600000.0 1.0")]
    protected /*private*/ float m_StartDelay;

    protected /*private*/ V30_MapVoting_WorldTimestampSyncComponent m_WorldTimestampSyncComponent;

    protected /*private*/ RplComponent m_RplComponent;

    protected /*private*/ V30_MapVoting_PreGameGameModeState_State m_State;

	protected /*private*/ ref ScriptInvoker m_OnStateChanged;

    protected /*private*/ int m_PlayerCountLeft;

    protected /*private*/ WorldTimestamp m_StartWorldTimestamp;



    void V30_MapVoting_SimplePreGameGameModeStateComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {
        m_State = V30_MapVoting_PreGameGameModeState_State.INITIAL;
		m_OnStateChanged = new ScriptInvoker();
    };

    override protected event void OnPostInit(IEntity owner) {
        super.OnPostInit(owner);
        SetEventMask(owner, EntityEvent.INIT);
    };

    override protected event void EOnInit(IEntity owner) {
        super.EOnInit(owner);
        m_RplComponent = V30_MapVoting_ComponentHelper<RplComponent>.FindComponent(owner);
        m_WorldTimestampSyncComponent = V30_MapVoting_ComponentHelper<V30_MapVoting_WorldTimestampSyncComponent>.FindComponent(owner);

		if (SCR_Global.IsEditMode())
			return;

        if (IsProxy())
            return;

        SetState(V30_MapVoting_PreGameGameModeState_State.WAITING_PLAYERS);
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



    protected /*private*/ void UpdatePlayerCountLeft(int playerCountLeft) {
        if (playerCountLeft < 0)
            playerCountLeft = 0;

        Rpc(RpcDo_UpdatePlayerCountLeft, playerCountLeft);
        RpcDo_UpdatePlayerCountLeft(playerCountLeft);

        if (playerCountLeft == 0)
            SetState(V30_MapVoting_PreGameGameModeState_State.DELAY_START);
        else if (GetState() == V30_MapVoting_PreGameGameModeState_State.DELAY_START)
            SetState(V30_MapVoting_PreGameGameModeState_State.WAITING_PLAYERS);
    };

    protected /*private*/ int UpdatePlayerCountLeft() {
        auto votingComponent = GetVotingComponent();
        auto requiredPlayerCount = GetRequiredPlayerCount();
        auto playerCount = votingComponent.CountPlayersWithVoteAbility();
        auto playerCountLeft = requiredPlayerCount - playerCount;
        UpdatePlayerCountLeft(playerCountLeft);
        return playerCountLeft;
    };

    [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_UpdatePlayerCountLeft(int playerCountLeft) {
        m_PlayerCountLeft = playerCountLeft;
        OnMessageUpdated();
    };

    protected /*private*/ void UpdateStartDelay(WorldTimestamp startWorldTimestamp) {
        Rpc(RpcDo_UpdateStartDelay, startWorldTimestamp);
        RpcDo_UpdateStartDelay(startWorldTimestamp);

        auto now = GetGame().GetWorld().GetTimestamp();
        if (now.GreaterEqual(startWorldTimestamp))
            SetState(V30_MapVoting_PreGameGameModeState_State.FINISHED);
    };

    protected /*private*/ WorldTimestamp UpdateStartDelay() {
        auto now = GetGame().GetWorld().GetTimestamp();
        auto delay = GetStartDelay();
        auto start = now.PlusMilliseconds(delay);
        UpdateStartDelay(start);
        return start;
    };

	[RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_UpdateStartDelay(WorldTimestamp startRemoteWorldTimestamp) {
        m_StartWorldTimestamp = startRemoteWorldTimestamp;
        OnMessageUpdated();
    }



    protected /*private*/ void SetState(V30_MapVoting_PreGameGameModeState_State state) {
        Rpc(RpcDo_SetState, state);
        RpcDo_SetState(state);
    };

    [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_SetState(V30_MapVoting_PreGameGameModeState_State state) {
        auto owner = GetOwner();

        switch (m_State) {
            case V30_MapVoting_PreGameGameModeState_State.INITIAL: {
                break;
            };
            case V30_MapVoting_PreGameGameModeState_State.WAITING_PLAYERS: {
                if (!IsProxy())
                    m_VotingComponent.GetOnPlayerVoteAbilityChanged().Remove(OnPlayerVoteAbilityChanged);
                break;
            };
            case V30_MapVoting_PreGameGameModeState_State.DELAY_START: {
                if (!System.IsConsoleApp())
                    ClearEventMask(owner, EntityEvent.FRAME);
                if (!IsProxy())
                    ClearEventMask(owner, EntityEvent.FIXEDFRAME);
                break;
            };
            case V30_MapVoting_PreGameGameModeState_State.FINISHED: {
                break;
            };
        };

		auto stateOld = m_State;
        m_State = state;

        switch (m_State) {
            case V30_MapVoting_PreGameGameModeState_State.INITIAL: {
                break;
            };
            case V30_MapVoting_PreGameGameModeState_State.WAITING_PLAYERS: {
                if (!IsProxy()) {
                    auto votingComponent = GetVotingComponent();
                    votingComponent.GetOnPlayerVoteAbilityChanged().Insert(OnPlayerVoteAbilityChanged);
                };
                break;
            };
            case V30_MapVoting_PreGameGameModeState_State.DELAY_START: {
                if (!IsProxy())
                    SetEventMask(owner, EntityEvent.FIXEDFRAME);
                if (!System.IsConsoleApp())
                    SetEventMask(owner, EntityEvent.FRAME);
                break;
            };
            case V30_MapVoting_PreGameGameModeState_State.FINISHED: {
                break;
            };
        };

		OnStateChanged(state, stateOld);

        switch (m_State) {
            case V30_MapVoting_PreGameGameModeState_State.INITIAL: {
                break;
            };
            case V30_MapVoting_PreGameGameModeState_State.WAITING_PLAYERS: {
                if (!IsProxy())
                    UpdatePlayerCountLeft();
                break;
            };
            case V30_MapVoting_PreGameGameModeState_State.DELAY_START: {
                if (!IsProxy())
                    UpdateStartDelay();
                break;
            };
            case V30_MapVoting_PreGameGameModeState_State.FINISHED: {
				if (!IsProxy())
					Finish();
                break;
            };
        };
    };

	protected /*private*/ event void OnStateChanged(V30_MapVoting_PreGameGameModeState_State stateNew, V30_MapVoting_PreGameGameModeState_State stateOld) {
		PrintFormat("[V30][MapVoting] %1.OnStateChanged(%2, %3)", this, SCR_Enum.GetEnumName(V30_MapVoting_PreGameGameModeState_State, stateNew), SCR_Enum.GetEnumName(V30_MapVoting_PreGameGameModeState_State, stateOld));
		m_OnStateChanged.Invoke(this, stateNew, stateOld);
	};

    /*sealed*/ ScriptInvoker GetOnStateChanged() {
        return m_OnStateChanged;
    };

    protected /*private*/ event void OnPlayerVoteAbilityChanged(notnull V30_MapVoting_VotingComponent votingComponent, int playerId, bool hasVoteAbility) {
        UpdatePlayerCountLeft();
    };



    override string GetMessage() {
        switch (m_State) {
            case V30_MapVoting_PreGameGameModeState_State.WAITING_PLAYERS: {
                auto format = "#AR-V30_MapVoting_SimplePreGame_WaitingPlayers";
                auto playerCountLeft = m_PlayerCountLeft.ToString();
                return SCR_StringHelper.Translate(format, playerCountLeft);
            };
            case V30_MapVoting_PreGameGameModeState_State.DELAY_START: {
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
    DELAY_START,
    FINISHED
};
