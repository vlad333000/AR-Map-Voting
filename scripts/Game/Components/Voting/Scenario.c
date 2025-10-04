class V30_MapVoting_ScenarioVotingComponentClass : V30_MapVoting_VotingComponentClass {};

class V30_MapVoting_ScenarioVotingComponent : V30_MapVoting_VotingComponent {
    [Attribute("true")]
    protected /*private*/ bool m_TimeLimitEnabled;

    [Attribute("300000", uiwidget: "slider", params: "0 3600000")]
    protected /*private*/ int m_TimeLimit;

    protected /*private*/ WorldTimestamp m_TimeLimitStart;

    protected /*private*/ ref ScriptInvoker m_OnTimeLimitStarted;

    protected /*private*/ ref ScriptInvoker m_OnTimeLimitStopped;

    [Attribute("true")]
    protected /*private*/ bool m_FastTimeLimitEnabled;

    [Attribute("10000", uiwidget: "slider", params: "0 3600000")]
    protected /*private*/ int m_FastTimeLimit;

    [Attribute("50.0", uiwidget: "slider", params: "0.0 100.0")]
    protected /*private*/ float m_FastTimeLimitVotedPlayerPercentage;

    [Attribute("1", uiwidget: "slider", params: "0 128")]
    protected /*private*/ int m_FastTimeLimitVotedPlayerCount;

    protected /*private*/ WorldTimestamp m_FastTimeLimitStart;

    protected /*private*/ ref ScriptInvoker m_OnFastTimeLimitStarted;

    protected /*private*/ ref ScriptInvoker m_OnFastTimeLimitStopped;

    [Attribute("true")]
    protected /*private*/ bool m_TimeSyncEnabled;

    [Attribute("5000", uiwidget: "slider", params: "0 60000")]
    protected /*private*/ int m_TimeSyncInterval;

    [Attribute("true")]
    protected /*private*/ bool m_TimeSyncPingEnabled;

    protected /*private*/ WorldTimestamp m_TimeSyncTimestamp;

    protected /*private*/ ref ScriptInvoker m_OnTimeSynchronized;

    protected /*private*/ ref ScriptInvoker m_OnTimeTicking;



    void V30_MapVoting_ScenarioVotingComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {
        m_OnTimeLimitStarted = new ScriptInvoker();
        m_OnTimeLimitStopped = new ScriptInvoker();
        m_OnFastTimeLimitStarted = new ScriptInvoker();
        m_OnFastTimeLimitStopped = new ScriptInvoker();
        m_OnTimeSynchronized = new ScriptInvoker();
        m_OnTimeTicking = new ScriptInvoker();
    };




    override protected void ParseConfig(notnull V30_Json_object config) {
        super.ParseConfig(config);
        ParseTimeLimitConfig(config);
        ParseFastTimeLimitConfig(config);
        ParseFastTimeLimitPercentageConfig(config);
        ParseFastTimeLimitCountConfig(config);
    };

    protected /*private*/ void ParseTimeLimitConfig(notnull V30_Json_object config) {
        auto value = config.GetAt("time");
        if (!value)
            return;

        switch (value.GetType()) {
            case V30_Json_EValueType.INT: {
                m_TimeLimitEnabled = true;
                m_TimeLimit = value.AsInt().Get() * 1000;
                break;
            };
            case V30_Json_EValueType.FLOAT: {
                m_TimeLimitEnabled = true;
                m_TimeLimit = Math.Round(value.AsFloat().Get() * 1000.0);
                break;
            };
            case V30_Json_EValueType.BOOL: {
                m_TimeLimitEnabled = value.AsBool().Get();
                break;
            };
        };
    };

    protected /*private*/ void ParseFastTimeLimitConfig(notnull V30_Json_object config) {
        auto value = config.GetAt("thresholdTime");
        if (!value)
            return;

        switch (value.GetType()) {
            case V30_Json_EValueType.INT: {
                m_FastTimeLimitEnabled = true;
                m_FastTimeLimit = value.AsInt().Get() * 1000;
                break;
            };
            case V30_Json_EValueType.FLOAT: {
                m_FastTimeLimitEnabled = true;
                m_FastTimeLimit = Math.Round(value.AsFloat().Get() * 1000.0);
                break;
            };
            case V30_Json_EValueType.BOOL: {
                m_FastTimeLimitEnabled = value.AsBool().Get();
                break;
            };
        };
    };

    protected /*private*/ void ParseFastTimeLimitPercentageConfig(notnull V30_Json_object config) {
        auto value = config.GetAt("thresholdPercentage");
        if (!value)
            return;

        switch (value.GetType()) {
            case V30_Json_EValueType.INT: {
                m_FastTimeLimitVotedPlayerPercentage = value.AsInt().Get();
                break;
            };
            case V30_Json_EValueType.FLOAT: {
                m_FastTimeLimitVotedPlayerPercentage = value.AsFloat().Get();
                break;
            };
        };
    };

    protected /*private*/ void ParseFastTimeLimitCountConfig(notnull V30_Json_object config) {
        auto value = config.GetAt("thresholdCount");
        if (!value)
            return;

        switch (value.GetType()) {
            case V30_Json_EValueType.INT: {
                m_FastTimeLimitVotedPlayerCount = value.AsInt().Get();
                break;
            };
            case V30_Json_EValueType.FLOAT: {
                m_FastTimeLimitVotedPlayerCount = Math.Round(value.AsFloat().Get());
                break;
            };
        };
    };



    /*sealed*/ bool IsTimeLimitEnabled() {
        return m_TimeLimitEnabled;
    };

    /*sealed*/ bool IsTimeLimitTicking() {
        return !!m_TimeLimitStart;
    };

    /*sealed*/ WorldTimestamp GetTimeLimitStart() {
        return m_TimeLimitStart;
    };

    /*sealed*/ int GetTimeLimitRemaining() {
        auto start = m_TimeLimitStart;
        auto now = GetGame().GetWorld().GetTimestamp();
        int delta = now.DiffMilliseconds(start);

        auto remaining = m_TimeLimit - delta;
        if (remaining < 0)
            remaining = 0;
        return remaining;
    };

    protected /*private*/ void StartTimeLimit() {
        Rpc(RpcDo_StartTimeLimit);
        RpcDo_StartTimeLimit();
    };

    [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_StartTimeLimit() {
        m_TimeLimitStart = GetGame().GetWorld().GetTimestamp();
        if (IsTimeSyncPingEnabled())
            m_TimeLimitStart = m_TimeLimitStart.PlusMilliseconds(-GetLocalPing());
        OnTimeLimitStarted();
        SetEventMask(GetOwner(), EntityEvent.FRAME);
    };

    protected /*private*/ event void OnTimeLimitStarted() {
        m_OnTimeLimitStarted.Invoke(this);
    };

    /*sealed*/ ScriptInvoker GetOnTimeLimitStarted() {
        return m_OnTimeLimitStarted;
    };

    protected /*private*/ void StopTimeLimit() {
        Rpc(RpcDo_StopTimeLimit);
        RpcDo_StopTimeLimit();
    };

    [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_StopTimeLimit() {
        m_TimeLimitStart = null;
        OnTimeLimitStopped();
        if (!IsTimeTicking())
            ClearEventMask(GetOwner(), EntityEvent.FRAME);
    };

    protected /*private*/ event void OnTimeLimitStopped() {
        m_OnTimeLimitStopped.Invoke(this);
    };

    /*sealed*/ ScriptInvoker GetOnTimeLimitStopped() {
        return m_OnTimeLimitStopped;
    };



    /*sealed*/ bool IsFastTimeLimitEnabled() {
        return m_FastTimeLimitEnabled;
    };

    /*sealed*/ bool IsFastTimeLimitTicking() {
        return !!m_FastTimeLimitStart;
    };

    /*sealed*/ float GetFastTimeLimitVotedPlayerPercentage() {
        return m_FastTimeLimitVotedPlayerPercentage;
    };

    /*sealed*/ int GetFastTimeLimitVotedPlayerCount() {
        return m_FastTimeLimitVotedPlayerCount;
    };

    /*sealed*/ WorldTimestamp GetFastTimeLimitStart() {
        return m_FastTimeLimitStart;
    };

    /*sealed*/ bool IsFastTimeLimitTickingAllowed() {
        if (!IsFastTimeLimitEnabled())
            return false;

        auto votedCount = CountVotedPlayers();
        auto requiredVotedCount = GetFastTimeLimitVotedPlayerCount();
        if (votedCount < requiredVotedCount)
            return false;

		auto playerCount = CountPlayersWithVoteAbility();
        auto votedPercentage = ((float)votedCount / (float)playerCount) * 100.0;
        auto requiredVotedPercentage = GetFastTimeLimitVotedPlayerPercentage();
        if (votedPercentage < requiredVotedPercentage)
            return false;

        return true;
    };

    /*sealed*/ int GetFastTimeLimitRemaining() {
        auto start = m_FastTimeLimitStart;
        auto now = GetGame().GetWorld().GetTimestamp();
        int delta = now.DiffMilliseconds(start);

        auto remaining = m_FastTimeLimit - delta;
        if (remaining < 0)
            remaining = 0;
        return remaining;
    };

    protected /*private*/ void StartFastTimeLimit() {
        Rpc(RpcDo_StartFastTimeLimit);
        RpcDo_StartFastTimeLimit();
    };

    [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_StartFastTimeLimit() {
        m_FastTimeLimitStart = GetGame().GetWorld().GetTimestamp();
        if (IsTimeSyncPingEnabled())
            m_FastTimeLimitStart = m_FastTimeLimitStart.PlusMilliseconds(-GetLocalPing());
        OnFastTimeLimitStarted();
        SetEventMask(GetOwner(), EntityEvent.FRAME);
    };

    protected /*private*/ event void OnFastTimeLimitStarted() {
        m_OnFastTimeLimitStarted.Invoke(this);
    };

    /*sealed*/ ScriptInvoker GetOnFastTimeLimitStarted() {
        return m_OnFastTimeLimitStarted;
    };

    protected /*private*/ void StopFastTimeLimit() {
        Rpc(RpcDo_StopFastTimeLimit);
        RpcDo_StopFastTimeLimit();
    };

    [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_StopFastTimeLimit() {
        m_FastTimeLimitStart = null;
        OnTimeLimitStopped();
        if (!IsTimeTicking())
            ClearEventMask(GetOwner(), EntityEvent.FRAME);
    };

    protected /*private*/ event void OnFastTimeLimitStopped() {
        m_OnFastTimeLimitStopped.Invoke(this);
    };

    /*sealed*/ ScriptInvoker GetOnFastTimeLimitStopped() {
        return m_OnFastTimeLimitStopped;
    };



    /*sealed*/ bool IsTimeTicking() {
        return IsTimeLimitTicking() || IsFastTimeLimitTicking();
    };

    /*sealed*/ int GetRemainingTime() {
        auto remaining = GetTimeLimitRemaining();
        auto remainingFast = GetFastTimeLimitRemaining();
        if (remainingFast < remaining)
            remaining = remainingFast;
        return remaining;
    };

    /*sealed*/ WorldTimestamp GetEndTime() {
        auto now = GetGame().GetWorld().GetTimestamp();
        auto remaining = GetRemainingTime();
        auto end = now.PlusMilliseconds(remaining);
        return end;
    };

    protected /*private*/ void OnTimeTicking() {
        m_OnTimeTicking.Invoke(this);
    };

    /*sealed*/ ScriptInvoker GetOnTimeTicking() {
        return m_OnTimeTicking;
    };



    override event void OnPlayerConnected(int playerId) {
        super.OnPlayerConnected(playerId);
        UpdateTimeLimit();
    };

    override event void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout) {
        super.OnPlayerDisconnected(playerId, cause, timeout);
        UpdateTimeLimit();
    };

    override protected event void OnPlayerVoteAbilityChanged(int playerId, bool hasVoteAbility) {
        super.OnPlayerVoteAbilityChanged(playerId, hasVoteAbility);
        UpdateTimeLimit();
    };

    override protected event void OnPlayerVoteChanged(int playerId, V30_MapVoting_ChoiceId choiceId, V30_MapVoting_ChoiceId oldChoiceId) {
        super.OnPlayerVoteChanged(playerId, choiceId, oldChoiceId);
        if (IsAuthority())
            UpdateTimeLimit();
    };

    override protected event void OnVoteStarted() {
        super.OnVoteStarted();
        if (IsAuthority())
            UpdateTimeLimit();
    };

    override protected event void OnVoteEnded(V30_MapVoting_ChoiceId winnerId) {
        super.OnVoteEnded(winnerId);
        if (IsAuthority())
            UpdateTimeLimit();
    };

    protected void UpdateTimeLimit() {
        if (!IsVoteStarted()) {
            if (IsTimeLimitTicking())
                StopTimeLimit();
            if (IsFastTimeLimitTicking())
                StopFastTimeLimit();
            if (IsTimeSyncStarted())
                StopTimeSync();
            return;
        };

        // Time limit
        if (IsTimeLimitEnabled()) {
            if (!IsTimeLimitTicking())
                StartTimeLimit();
        }
        else if (IsTimeLimitTicking())
            StopTimeLimit();

        // Fast time limit
        if (IsFastTimeLimitEnabled() && IsFastTimeLimitTickingAllowed()) {
            if (!IsFastTimeLimitTicking())
                StartFastTimeLimit();
        }
        else if (IsFastTimeLimitTicking())
            StopFastTimeLimit();

        // Time sync
        if (IsTimeTicking() && IsTimeSyncEnabled()) {
            if (!IsTimeSyncStarted() && IsAuthority())
                StartTimeSync();
        }
        else if (IsTimeSyncStarted())
            StopTimeSync();
    };



    /*sealed*/ bool IsTimeSyncEnabled() {
        return m_TimeSyncEnabled;
    };

    /*saeled*/ int GetTimeSyncInterval() {
        return m_TimeSyncInterval;
    };

    /*sealed*/ bool IsTimeSyncPingEnabled() {
        return m_TimeSyncPingEnabled;
    };

    /*sealed*/ WorldTimestamp GetTimeSyncTimestamp() {
        return m_TimeSyncTimestamp;
    };

    /*sealed*/ WorldTimestamp GetNextTimeSyncTimestamp() {
        auto last = GetTimeSyncTimestamp();
        auto interval = GetTimeSyncInterval();
        auto next = last.PlusMilliseconds(interval);
        return next;
    };

    /*selaed*/ int GetNextTimeSyncRemaining() {
        auto now = GetGame().GetWorld().GetTimestamp();
        auto next = GetNextTimeSyncTimestamp();
        int remaining = next.DiffMilliseconds(now);
        if (remaining < 0.0)
            remaining = 0;
        return remaining;
    };

    protected /*private*/ bool IsTimeSyncRequired() {
        if (!GetTimeSyncTimestamp())
            return true;
		auto now = GetGame().GetWorld().GetTimestamp();
		auto next = GetNextTimeSyncTimestamp();
        return now.GreaterEqual(next);
    };

    protected /*private*/ void SyncTime() {
        m_TimeSyncTimestamp = GetGame().GetWorld().GetTimestamp();
        auto remainingTime = GetRemainingTime();
        Rpc(RpcDo_SyncTime, remainingTime);
    };

    [RplRpc(channel: RplChannel.Unreliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_SyncTime(int remainingTimeNew) {
        m_TimeSyncTimestamp = GetGame().GetWorld().GetTimestamp();
        auto remainingTimeNewOld = GetRemainingTime();
        auto timeDelta = remainingTimeNewOld - remainingTimeNew;

        if (IsTimeSyncPingEnabled())
            timeDelta -= GetLocalPing();

        if (timeDelta == 0)
            return;

        if (m_TimeLimitStart)
            m_TimeLimitStart = m_TimeLimitStart.PlusMilliseconds(timeDelta);
        if (m_FastTimeLimitStart)
            m_FastTimeLimitStart = m_FastTimeLimitStart.PlusMilliseconds(timeDelta);
        OnTimeSynchronized(timeDelta);
    };

    protected /*private*/ event void OnTimeSynchronized(int timeDelta) {
        m_OnTimeSynchronized.Invoke(this, timeDelta);
    };

    /*sealed*/ ScriptInvoker GetOnTimeSynchronized() {
        return m_OnTimeSynchronized;
    };

    protected /*private*/ void StartTimeSync() {
        SetEventMask(GetOwner(), EntityEvent.FIXEDFRAME);
    };

    protected /*private*/ bool IsTimeSyncStarted() {
        return (GetEventMask() & EntityEvent.FIXEDFRAME) != 0;
    };

    protected /*private*/ void StopTimeSync() {
        ClearEventMask(GetOwner(), EntityEvent.FIXEDFRAME);
    };



    override protected event void EOnFrame(IEntity owner, float timeSlice) {
        super.EOnFrame(owner, timeSlice);
        if (IsTimeTicking())
            OnTimeTicking();

        auto remainingTime = GetRemainingTime();
        if (remainingTime > 0)
            return;

        if (IsAuthority())
            EndVote();
    };

    override protected event void EOnFixedFrame(IEntity owner, float timeSlice) {
        super.EOnFixedFrame(owner, timeSlice);

        if (IsTimeSyncRequired())
            SyncTime();
    };



    override protected event bool RplSave(ScriptBitWriter writer) {
        if (!super.RplSave(writer))
            return false;

        // m_TimeLimitStart
        if (IsTimeLimitTicking()) {
            writer.WriteBool(true);
            writer.WriteInt(GetTimeLimitRemaining());
        }
        else
            writer.WriteBool(false);

        // m_FastTimeLimitStart
        if (IsFastTimeLimitTicking()) {
            writer.WriteBool(true);
            writer.WriteInt(GetFastTimeLimitRemaining());
        }
        else
            writer.WriteBool(false);

        return true;
    };

    override protected bool RplSaveConfig(ScriptBitWriter writer) {
        if (!super.RplSaveConfig(writer))
            return false;
        writer.WriteBool(m_TimeLimitEnabled);
        writer.WriteInt(m_TimeLimit);
        writer.WriteBool(m_FastTimeLimitEnabled);
        writer.WriteInt(m_FastTimeLimit);
        writer.WriteFloat(m_FastTimeLimitVotedPlayerPercentage);
        writer.WriteInt(m_FastTimeLimitVotedPlayerCount);
        writer.WriteBool(m_TimeSyncEnabled);
        writer.WriteInt(m_TimeSyncInterval);
        return true;
    };

    override protected event bool RplLoad(ScriptBitReader reader) {
        if (!super.RplLoad(reader))
            return false;

        // m_TimeLimitStart
        bool isTimeLimitTicking;
        if (!reader.ReadBool(isTimeLimitTicking))
            return false;
        if (isTimeLimitTicking) {
            int timeLimitRemaining;
            if (!reader.ReadInt(timeLimitRemaining))
                return false;
            m_TimeLimitStart = GetGame().GetWorld().GetTimestamp().PlusMilliseconds(-timeLimitRemaining);
        }
        else
            m_TimeLimitStart = null;

        // m_FastTimeLimitStart
        bool isFastTimeLimitTicking;
        if (!reader.ReadBool(isFastTimeLimitTicking))
            return false;
        if (isFastTimeLimitTicking) {
            int fastTimeLimitRemaining;
            if (!reader.ReadInt(fastTimeLimitRemaining))
                return false;
            m_FastTimeLimitStart = GetGame().GetWorld().GetTimestamp().PlusMilliseconds(-fastTimeLimitRemaining);
        }
        else
            m_FastTimeLimitStart = null;

        return true;
    };

    override protected bool RplLoadConfig(ScriptBitReader reader) {
        if (!super.RplLoadConfig(reader))
            return false;
        if (!reader.ReadBool(m_TimeLimitEnabled))
            return false;
        if (!reader.ReadInt(m_TimeLimit))
            return false;
        if (!reader.ReadBool(m_FastTimeLimitEnabled))
            return false;
        if (!reader.ReadInt(m_FastTimeLimit))
            return false;
        if (!reader.ReadFloat(m_FastTimeLimitVotedPlayerPercentage))
            return false;
        if (!reader.ReadInt(m_FastTimeLimitVotedPlayerCount))
            return false;
        return true;
    };

    override protected void SyncConfig() {
        super.SyncConfig();
        auto config = new V30_MapVoting_ScenarioVotingConfig();
        config.m_TimeLimitEnabled = m_TimeLimitEnabled;
        config.m_TimeLimit = m_TimeLimit;
        config.m_FastTimeLimitEnabled = m_FastTimeLimitEnabled;
        config.m_FastTimeLimit = m_FastTimeLimit;
        config.m_FastTimeLimitVotedPlayerPercentage = m_FastTimeLimitVotedPlayerPercentage;
        config.m_FastTimeLimitVotedPlayerCount = m_FastTimeLimitVotedPlayerCount;
        config.m_TimeSyncEnabled = m_TimeSyncEnabled;
        config.m_TimeSyncInterval = m_TimeSyncInterval;
        Rpc(RpcDo_SyncScenarioConfig, config);
    };

    [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_SyncScenarioConfig(notnull V30_MapVoting_ScenarioVotingConfig config) {
        m_TimeLimitEnabled = config.m_TimeLimitEnabled;
        m_TimeLimit = config.m_TimeLimit;
        m_FastTimeLimitEnabled = config.m_FastTimeLimitEnabled;
        m_FastTimeLimit = config.m_FastTimeLimit;
        m_FastTimeLimitVotedPlayerPercentage = config.m_FastTimeLimitVotedPlayerPercentage;
        m_FastTimeLimitVotedPlayerCount = config.m_FastTimeLimitVotedPlayerCount;
    };



    protected /*private*/ static int GetLocalPing() {
        auto localRplIdentity = RplIdentity.Local();
        auto connectionStats = Replication.GetConnectionStats(localRplIdentity);
        int roundTripTime = connectionStats.GetRoundTripTimeInMs();
        auto ping = roundTripTime / 2; // roundTrip is time from source to destination and back
        return ping;
    };
};

class V30_MapVoting_ScenarioVotingConfig {
    bool m_TimeLimitEnabled;
    int m_TimeLimit;
    bool m_FastTimeLimitEnabled;
    int m_FastTimeLimit;
    float m_FastTimeLimitVotedPlayerPercentage;
    int m_FastTimeLimitVotedPlayerCount;
    bool m_TimeSyncEnabled;
    int m_TimeSyncInterval;

    static bool Extract(V30_MapVoting_ScenarioVotingConfig instance, ScriptCtx ctx, SSnapSerializerBase snapshot) {
        snapshot.SerializeBool(instance.m_TimeLimitEnabled);
        snapshot.SerializeInt(instance.m_TimeLimit);
        snapshot.SerializeBool(instance.m_FastTimeLimitEnabled);
        snapshot.SerializeInt(instance.m_FastTimeLimit);
        snapshot.SerializeFloat(instance.m_FastTimeLimitVotedPlayerPercentage);
        snapshot.SerializeInt(instance.m_FastTimeLimitVotedPlayerCount);
        snapshot.SerializeBool(instance.m_TimeSyncEnabled);
        snapshot.SerializeInt(instance.m_TimeSyncInterval);
        return true;
    };

    static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, V30_MapVoting_ScenarioVotingConfig instance) {
        snapshot.SerializeBool(instance.m_TimeLimitEnabled);
        snapshot.SerializeInt(instance.m_TimeLimit);
        snapshot.SerializeBool(instance.m_FastTimeLimitEnabled);
        snapshot.SerializeInt(instance.m_FastTimeLimit);
        snapshot.SerializeFloat(instance.m_FastTimeLimitVotedPlayerPercentage);
        snapshot.SerializeInt(instance.m_FastTimeLimitVotedPlayerCount);
        snapshot.SerializeBool(instance.m_TimeSyncEnabled);
        snapshot.SerializeInt(instance.m_TimeSyncInterval);
        return true;
    };

    static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet) {
        snapshot.EncodeBool(packet); // m_TimeLimitEnabled
        snapshot.EncodeInt(packet); // m_TimeLimit
        snapshot.EncodeBool(packet); // m_FastTimeLimitEnabled
        snapshot.EncodeInt(packet); // m_FastTimeLimit
        snapshot.EncodeFloat(packet); // m_FastTimeLimitVotedPlayerPercentage
        snapshot.EncodeInt(packet); // m_FastTimeLimitVotedPlayerCount
        snapshot.EncodeBool(packet); // m_TimeSyncEnabled
        snapshot.EncodeInt(packet); // m_TimeSyncInterval
    };

    static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot) {
        snapshot.DecodeBool(packet); // m_TimeLimitEnabled
        snapshot.DecodeInt(packet); // m_TimeLimit
        snapshot.DecodeBool(packet); // m_FastTimeLimitEnabled
        snapshot.DecodeInt(packet); // m_FastTimeLimit
        snapshot.DecodeFloat(packet); // m_FastTimeLimitVotedPlayerPercentage
        snapshot.DecodeInt(packet); // m_FastTimeLimitVotedPlayerCount
        snapshot.DecodeBool(packet); // m_TimeSyncEnabled
        snapshot.DecodeInt(packet); // m_TimeSyncInterval
        return true;
    };

    static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx) {
        return lhs.CompareSnapshots(rhs, 0
            + 4 // bool m_TimeLimitEnabled
            + 4 // int m_TimeLimit
            + 4 // bool m_FastTimeLimitEnabled
            + 4 // int m_FastTimeLimit
            + 4 // float m_FastTimeLimitVotedPlayerPercentage
            + 4 // int m_FastTimeLimitVotedPlayerCount
            + 4 // bool m_TimeSyncEnabled
            + 4 // int m_TimeSyncInterval
        );
    };

    static bool PropCompare(V30_MapVoting_ScenarioVotingConfig instance, SSnapSerializerBase snapshot, ScriptCtx ctx) {
        if (!snapshot.CompareBool(instance.m_TimeLimitEnabled))
            return false;
        if (!snapshot.CompareInt(instance.m_TimeLimit))
            return false;
        if (!snapshot.CompareBool(instance.m_FastTimeLimitEnabled))
            return false;
        if (!snapshot.CompareInt(instance.m_FastTimeLimit))
            return false;
        if (!snapshot.CompareFloat(instance.m_FastTimeLimitVotedPlayerPercentage))
            return false;
        if (!snapshot.CompareInt(instance.m_FastTimeLimitVotedPlayerCount))
            return false;
        if (!snapshot.CompareBool(instance.m_TimeSyncEnabled))
            return false;
        if (!snapshot.CompareInt(instance.m_TimeSyncInterval))
            return false;
        return true;
    };
};
