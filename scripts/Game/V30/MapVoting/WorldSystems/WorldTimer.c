class V30_MapVoting_WorldTimer {
    WorldTimestamp GetEndTimestamp();
};

typedef int V30_MapVoting_WorldTimerId;

class V30_MapVoting_WorldTimerRef {
    protected static const WorldTimestamp zero;

    protected V30_MapVoting_WorldTimerWorldSystem system;

    protected V30_MapVoting_WorldTimerId timerId;

    void V30_MapVoting_WorldTimerRef(notnull V30_MapVoting_WorldTimerWorldSystem system, V30_MapVoting_WorldTimerId timerId) {
        this.system = system;
        this.timerId = timerId;
    };

    void ~V30_MapVoting_WorldTimerRef() {
        if (system)
            system.RemoveTimer(timerId);
    };

    World GetWorld() {
        return system.GetWorld();
    };

    V30_MapVoting_WorldTimerWorldSystem GetSystem() {
        return system;
    };

    V30_MapVoting_WorldTimerId GetTimerId() {
        return timerId;
    };

    V30_MapVoting_WorldTimer GetTimer() {
        return system.GetTimer(timerId);
    };

    bool IsEnded() {
        return system.IsTimerEnded(timerId);
    };

    WorldTimestamp GetEndTimestamp() {
        return system.GetTimerEndTimestamp(timerId);
    };

    WorldTimestamp GetRemainingTime() {
        auto now = system.GetCurrentTimestamp();
        auto end = GetEndTimestamp();
        if (now.GreaterEqual(end))
            return zero;
        return end - now;
    };

    float GetRemainingSeconds() {
        auto now = system.GetCurrentTimestamp();
        auto end = GetEndTimestamp();
        if (now.GreaterEqual(end))
            return 0.0;
        return now.DiffSeconds(end);
    };

    float GetRemainingMilliseconds() {
        auto now = system.GetCurrentTimestamp();
        auto end = GetEndTimestamp();
        if (now.GreaterEqual(end))
            return 0.0;
        return now.DiffMilliseconds(end);
    };

    // friend V30_MapVoting_WorldTimerWorldSystem.RemoveTimer;

    // friend V30_MapVoting_WorldTimerWorldSystem.GetTimer;

    // friend V30_MapVoting_WorldTimerWorldSystem.IsTimerEnded;

    // friend V30_MapVoting_WorldTimerWorldSystem.GetTimerEndTimestamp;

    // friend V30_MapVoting_WorldTimerWorldSystem.GetCurrentTimestamp;
};

class V30_MapVoting_WorldTimerWorldSystem : WorldSystem {
    protected ref array<ref V30_MapVoting_WorldTimer> timers;

    override static void InitInfo(WorldSystemInfo outInfo) {
        super.InitInfo(outInfo);

        outInfo.SetAbstract(true);
        outInfo.SetUnique(false);
        outInfo.SetLocation(WorldSystemLocation.Both);
    };

    protected V30_MapVoting_WorldTimerId AddTimer(notnull V30_MapVoting_WorldTimer timer) {
        foreach (auto i, auto slot : timers) {
            if (!slot) {
                timers.Set(i, timer);
                return i;
            };
        };
        return timers.Insert(timer);
    };

    /*protected*/ void RemoveTimer(V30_MapVoting_WorldTimerId timerId) {
        timers.Set(timerId, null);
    };

    /*protected*/ V30_MapVoting_WorldTimer GetTimer(V30_MapVoting_WorldTimerId timerId) {
        return timers.Get(timerId);
    };

    /*protected*/ bool IsTimerEnded(V30_MapVoting_WorldTimerId timerId) {
        auto now = GetCurrentTimestamp();
        auto end = GetTimerEndTimestamp(timerId);
        return now.GreaterEqual(end);
    };

    /*protected*/ WorldTimestamp GetTimerEndTimestamp(V30_MapVoting_WorldTimerId timerId) {
        return timers.Get(timerId).GetEndTimestamp();
    };

    /*protected*/ WorldTimestamp GetCurrentTimestamp() {
        return GetWorld().GetTimestamp();
    };
};