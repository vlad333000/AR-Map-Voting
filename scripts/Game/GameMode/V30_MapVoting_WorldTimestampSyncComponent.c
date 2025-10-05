class V30_MapVoting_WorldTimestampSyncComponentClass : ScriptComponentClass {};

class V30_MapVoting_WorldTimestampSyncComponent : ScriptComponent {
    [Attribute("5000.0", uiwidget: "slider", params: "-1.0 300000.0 1.0")]
    protected /*private*/ float m_SyncInterval;

    [Attribute("true")]
    protected /*private*/ bool m_UsePing;

    protected /*private*/ WorldTimestamp m_LastSyncWorldTimestamp;

    protected /*private*/ WorldTimestamp m_RemoteWorldTimestamp;

    protected /*private*/ RplComponent m_RplComponent;

	protected /*private*/ ref ScriptInvoker m_OnSync;



    void V30_MapVoting_WorldTimestampSyncComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {
        m_OnSync = new ScriptInvoker();
    };

    override protected event void OnPostInit(IEntity owner) {
        super.OnPostInit(owner);
        SetEventMask(owner, EntityEvent.INIT);
    };

    override protected event void EOnInit(IEntity owner) {
        super.EOnInit(owner);

        m_RplComponent = V30_MapVoting_ComponentHelper<RplComponent>.FindComponent(owner);
        if (!IsProxy() && m_SyncInterval >= 0.0)
            SetEventMask(owner, EntityEvent.FIXEDFRAME);
    };

    override protected event void EOnActivate(IEntity owner) {
        super.EOnActivate(owner);
        if (!IsProxy() && m_SyncInterval >= 0.0)
            SyncTime();
    };

    override protected event void EOnFixedFrame(IEntity owner, float timeSlice) {
        super.EOnFixedFrame(owner, timeSlice);

        if (IsSyncRequired())
            SyncTime();
    };



    /*sealed*/ float GetSyncInterval() {
        return m_SyncInterval;
    };

    /*sealed*/ bool IsPingUsed() {
        return m_UsePing;
    };

    /*sealed*/ WorldTimestamp GetLastSyncWorldTimestamp() {
        return m_LastSyncWorldTimestamp;
    };

    /*sealed*/ WorldTimestamp GetRemoteWorldTimestamp() {
        return m_RemoteWorldTimestamp;
    };

    /*sealed*/ float GetRemoteWorldTimestampDelta() {
        auto last = GetLastSyncWorldTimestamp();
        auto lastRemote = GetRemoteWorldTimestamp();
        return lastRemote.DiffMilliseconds(last);
    };



    /*sealed*/ WorldTimestamp GetSyncedRemoteWorldTimestamp() {
        auto now = GetGame().GetWorld().GetTimestamp();
        auto last = GetLastSyncWorldTimestamp();
        auto elapsed = now.DiffMilliseconds(last);
        auto lastRemote = GetRemoteWorldTimestamp();
        return lastRemote.PlusMilliseconds(elapsed);
    };

    /*sealed*/ WorldTimestamp GetSyncedWorldTimestamp() {
        if (!IsProxy())
            return GetGame().GetWorld().GetTimestamp();
        return GetSyncedRemoteWorldTimestamp();
    };

    /*sealed*/ bool IsSyncRequired() {
        auto interval = GetSyncInterval();
        if (interval <= 0.0)
            return true;

        auto last = GetLastSyncWorldTimestamp();
        auto next = last.PlusMilliseconds(interval);
        auto now = GetGame().GetWorld().GetTimestamp();
        return now.GreaterEqual(next);
    };

    /*sealed*/ WorldTimestamp GetNextSyncWorldTimestamp() {
        auto interval = GetSyncInterval();
        auto last = GetLastSyncWorldTimestamp();
        return last.PlusMilliseconds(interval);
    };

    /*sealed*/ void SyncTime() {
        auto now = GetGame().GetWorld().GetTimestamp();
        Rpc(RpcDo_SyncTime, now);
        m_LastSyncWorldTimestamp = now;
    };

    [RplRpc(channel: RplChannel.Unreliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_SyncTime(WorldTimestamp nowRemote) {
        if (IsPingUsed()) {
            auto ping = GetPing();
            nowRemote = nowRemote.PlusMilliseconds(ping);
        };

        auto now = GetGame().GetWorld().GetTimestamp();
        auto last = GetLastSyncWorldTimestamp();
        auto elapsed = now.DiffMilliseconds(last);
        auto lastRemote = m_RemoteWorldTimestamp;
        auto nowRemoteExpected = lastRemote.PlusMilliseconds(elapsed);

        auto delta = nowRemote.DiffMilliseconds(nowRemoteExpected);
        m_LastSyncWorldTimestamp = now;
        m_RemoteWorldTimestamp = nowRemote;
        OnSync(delta);
    };

    protected /*private*/ void OnSync(float delta) {
        m_OnSync.Invoke(this, delta);
    };

    /*sealed*/ ScriptInvoker GetOnSync() {
        return m_OnSync;
    };



    override protected event bool RplSave(ScriptBitWriter writer) {
        if (!super.RplSave(writer))
            return false;
        auto now = GetGame().GetWorld().GetTimestamp();
        writer.Write(now, 64);
        return true;
    };

    override protected event bool RplLoad(ScriptBitReader reader) {
        if (!super.RplLoad(writer))
            return false;
        if (!reader.Read(m_RemoteWorldTimestamp, 64))
            return false;
        if (IsPingUsed())
            m_RemoteWorldTimestamp = m_RemoteWorldTimestamp.PlusMilliseconds(GetPing());
        auto now = GetGame().GetWorld().GetTimestamp();
        m_LastSyncWorldTimestamp = now;
        auto delta = m_RemoteWorldTimestamp.DiffMilliseconds(now);
        OnSync(delta);
        return true;
    };



    protected /*sealed*/ bool IsProxy() {
        return m_RplComponent && m_RplComponent.IsProxy();
    };

    protected /*sealed*/ float GetPing() {
        auto rplIdentity = RplIdentity.Local();
        auto connectionStats = Replication.GetConnectionStats(rplIdentity);
        auto roundTripTime = connectionStats.GetRoundTripTimeInMs();
        return roundTripTime / 2; // roundTrip is time from source to destination and back
    };
};
