class V30_MapVoting_WorldTimestampSyncWorldSystem : WorldSystem {
    [Attribute("0")]
    protected bool useReliableChannel;

    [Attribute("0.0", params: "0 inf")]
    protected float syncInterval;

    protected WorldTimestamp lastSyncWorldTimestamp;

    protected WorldTimestamp lastSyncTime;

    override static void InitInfo(WorldSystemInfo outInfo) {
        super.InitInfo(outInfo);

        outInfo.SetAbstract(false);
        outInfo.SetUnique(true);
        outInfo.AddPoint(WorldSystemPoint.RuntimeStarted);
        outInfo.AddPoint(WorldSystemPoint.FixedFrame);
        outInfo.SetLocation(WorldSystemLocation.Both);
    };

    override protected event void OnOneTimePoint(WorldOneTimePointArgs args) {
        super.OnOneTimePoint(args);

        switch (args.GetPoint()) {
            case WorldSystemPoint.RuntimeStarted: {
                if (!Replication.IsServer())
                   break;

                Sync();
                break;
            };
        };
    };

    override protected event void OnUpdatePoint(WorldUpdatePointArgs args) {
        super.OnUpdatePoint(args);

        switch (args.GetPoint()) {
            case WorldSystemPoint.FixedFrame: {
                if (!Replication.IsServer())
                    break;

                TrySync();
                break;
            };
        };
    };

    protected bool TrySync() {
        if (!IsSyncRequired())
            return false;

        Sync();
        return true;
    };

    protected void Sync() {
        auto now = GetLocalWorldTimestamp();
        if (useReliableChannel)
            Rpc(RpcDo_Sync_Reliable, now);
        else
            Rpc(RpcDo_Sync_Unreliable, now);
        RpcDo_Sync(now);
    };

    [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    protected event void RpcDo_Sync_Reliable(WorldTimestamp serverWorldTimestamp) {
        RpcDo_Sync(serverWorldTimestamp);
    };

    [RplRpc(channel: RplChannel.Unreliable, rcver: RplRcver.Broadcast)]
    protected event void RpcDo_Sync_Unreliable(WorldTimestamp serverWorldTimestamp) {
        RpcDo_Sync(serverWorldTimestamp);
    };

    protected event void RpcDo_Sync(WorldTimestamp serverWorldTimestamp) {
        lastSyncWorldTimestamp = serverWorldTimestamp;
        lastSyncTime = GetLocalWorldTimestamp();
        PrintFormat("[V30][MapVoting][WorldTimestampSync] RpcDo_Sync: server = %2, local = %3, delta = %4 ms", this, lastSyncWorldTimestamp, lastSyncTime, lastSyncWorldTimestamp.DiffMilliseconds(lastSyncTime), level: LogLevel.DEBUG);
    };

    WorldTimestamp GetLastSyncWorldTimestamp() {
        return lastSyncWorldTimestamp;
    };

    WorldTimestamp GetLastSyncTime() {
        return lastSyncTime;
    };

    WorldTimestamp GetNextSyncTime() {
        auto serverLastSync = GetLastSyncWorldTimestamp();
        auto serverNextSync = serverLastSync.PlusSeconds(syncInterval);
        return GetSyncedLocalWorldTimestamp(serverNextSync);
    };

    bool IsSyncRequired() {
        auto now = GetLocalWorldTimestamp();
        auto next = GetNextSyncTime();
        return now.GreaterEqual(next);
    };



    float GetDelta() {
        auto server = GetLastSyncWorldTimestamp();
        auto local = GetLastSyncTime();
        return server.DiffMilliseconds(local);
    };

    WorldTimestamp GetSyncedWorldTimestamp() {
        auto now = GetLocalWorldTimestamp();
        return GetSyncedWorldTimestamp(now);
    };

    WorldTimestamp GetSyncedWorldTimestamp(WorldTimestamp localWorldTimestamp) {
        auto delta = GetDelta();
        return localWorldTimestamp.PlusMilliseconds(delta);
    };

    WorldTimestamp GetSyncedLocalWorldTimestamp(WorldTimestamp serverWorldTimestamp) {
        auto delta = GetDelta();
        return serverWorldTimestamp.PlusMilliseconds(-delta);
    };

    protected static World GetGameWorld() {
        return GetGame().GetWorld();
    };

    protected WorldTimestamp GetLocalWorldTimestamp() {
        auto world = GetWorld();
        return GetLocalWorldTimestamp(world);
    };

    protected static WorldTimestamp GetLocalGameWorldTimestamp() {
        auto world = GetGameWorld();
        return GetLocalWorldTimestamp(world);
    };

    protected static WorldTimestamp GetLocalWorldTimestamp(BaseWorld world) {
        return world.GetTimestamp();
    };

    static V30_MapVoting_WorldTimestampSyncWorldSystem GetGameWorldInstance() {
        auto world = GetGameWorld();
        return GetWorldInstance(world);
    };

    static V30_MapVoting_WorldTimestampSyncWorldSystem GetWorldInstance(World world) {
        return V30_MapVoting_WorldTimestampSyncWorldSystem.Cast(world.FindSystem(V30_MapVoting_WorldTimestampSyncWorldSystem));
    };

    static WorldTimestamp GetGameWorldTimestamp() {
        auto world = GetGameWorld();
        return GetWorldTimestamp(world);
    };

    static WorldTimestamp GetWorldTimestamp(World world) {
        auto system = GetWorldInstance(world);
        if (!system)
            return GetLocalWorldTimestamp(world);
        return system.GetSyncedWorldTimestamp();
    };
};
