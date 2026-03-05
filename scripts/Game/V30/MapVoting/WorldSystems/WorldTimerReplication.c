class V30_MapVoting_ReplicatableWorldTimer : V30_MapVoting_WorldTimer {};

class V30_MapVoting_ReplicatableWorldTimer_Authority : V30_MapVoting_ReplicatableWorldTimer {
    protected ref V30_MapVoting_WorldTimerRef timer;

    void V30_MapVoting_ReplicatableWorldTimer_Authority(notnull V30_MapVoting_WorldTimerRef timer) {
        this.timer = timer;
    };

    V30_MapVoting_WorldTimerRef GetTimer() {
        return timer;
    };

    override WorldTimestamp GetEndTimestamp() {
        return timer.GetEndTimestamp();
    };
};

class V30_MapVoting_ReplicatableWorldTimer_Proxy : V30_MapVoting_ReplicatableWorldTimer {
    bool ended;

    bool IsEnded() {
        return ended;
    };

    void End() {
        ended = true;
    };
};

typedef int V30_MapVoting_ReplicatableWorldTimerId;

class V30_MapVoting_ReplicatableWorldTimerRef : V30_MapVoting_WorldTimerRef {
    protected V30_MapVoting_ReplicatableWorldTimerId replicatableTimerId;

    void V30_MapVoting_ReplicatableWorldTimerRef(notnull V30_MapVoting_WorldTimerWorldSystem system, V30_MapVoting_WorldTimerId timerId, V30_MapVoting_ReplicatableWorldTimerId replicatableTimerId) {
        this.replicatableTimerId = replicatableTimerId;
    };

    V30_MapVoting_ReplicatableWorldTimerId GetReplicatableTimerId() {
        return replicatableTimerId;
    };

    bool IsAuthority();

    bool IsProxy();
};

class V30_MapVoting_ReplicatableWorldTimerWorldSystem : V30_MapVoting_WorldTimerWorldSystem {
    override static void InitInfo(WorldSystemInfo outInfo) {
        super.InitInfo(outInfo);

        outInfo.SetAbstract(false);
        outInfo.SetUnique(false); // Allows create more optimized variants for specific timers
        outInfo.SetLocation(WorldSystemLocation.Both);
    };

    V30_MapVoting_WorldTimerRef CreateTimer(WorldTimestamp end) {
        auto timer = new V30_MapVoting_ReplicatableWorldTimer_Authority(null);
        auto timerId = AddTimer(timer);
        return new V30_MapVoting_ReplicatableWorldTimerRef(this, timerId, timerId);
    };

    // V30_MapVoting_ReplicatableWorldTimerId CreateTimer(notnull V30_MapVoting_Timer timer) {
    //     auto endTimestamp = timer.GetEndTimestamp();
    //     auto replicatableTimer = new V30_MapVoting_ReplicatableWorldTimer_Authority(timer);
    //     auto replicatableTimerId = timers.Insert(replicatableTimer);
    //     Rpc(RpcDo_CreateTimer, replicatableTimerId, endTimestamp);
    //     return replicatableTimerId;
    // };

    // void UpdateTimerEndTimestamp(V30_MapVoting_ReplicatableWorldTimerId timerId) {
    //     auto replicatableTimer = GetTimer(timerId);
    //     auto endTimestamp = replicatableTimer.GetEndTimestamp();
    //     Rpc(RpcDo_UpdateEndTimestamp, timerId, endTimestamp);
    // };

    // V30_MapVoting_ReplicatableWorldTimer GetTimer(V30_MapVoting_ReplicatableWorldTimerId timerId) {
    //     return timers.Get(timerId);
    // };

    // [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    // protected event void RpcDo_CreateTimer(V30_MapVoting_ReplicatableWorldTimerId timerId, WorldTimestamp endTimestamp) {

    // };

    // [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    // protected event void RpcDo_UpdateEndTimestamp(V30_MapVoting_ReplicatableWorldTimerId timerId, WorldTimestamp endTimestamp) {
    //     V30_MapVoting_ReplicatableWorldTimer_Proxy.Cast(GetTimer(timerId)).SetEndTimestamp(endTimestamp);
    // };

    // [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    // protected event void RpcDo_EndTimer(V30_MapVoting_ReplicatableWorldTimerId timerId) {
    //    V30_MapVoting_ReplicatableWorldTimer_Proxy.Cast(GetTimer(timerId)).End();
    // };

    // [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    // protected event void RpcDo_DestroyTimer(V30_MapVoting_ReplicatableWorldTimerId timerId);

	// bool RplSave(ScriptBitWriter writer) {
    //     writer.WriteInt(timers.Count());
    //     foreach (auto timer : timers) {
    //         if (timer) {
    //             writer.WriteBool(true);
    //             writer.Write(timer.GetEndTimestamp(), 64);
    //         }
    //         else {
    //             writer.WriteBool(false);
    //         };
    //     };
	// 	return true;
	// };

	// bool RplLoad(ScriptBitReader reader) {
    //     int n;
    //     if (!reader.ReadInt(n))
    //         return false;
    //     timers = new array<ref V30_MapVoting_ReplicatableWorldTimer>();
    //     timers.Reserve(n);
    //     for (int i = 0; i < n; ++i) {
    //         bool hasTimer;
    //         if (!reader.ReadBool(hasTimer))
    //             return false;
    //         if (hasTimer) {
    //             WorldTimestamp endTimestamp;
    //             if (!reader.Read(endTimestamp, 64))
    //                 return false;
    //             auto timer = new V30_MapVoting_ReplicatableWorldTimer_Proxy(endTimestamp);
    //             timers.Insert(timer);
    //         }
    //         else {
    //             timers.Insert(null);
    //         };
    //     };
    //     return true;
	// };
};
