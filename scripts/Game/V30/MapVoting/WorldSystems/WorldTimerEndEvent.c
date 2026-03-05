class V30_MapVoting_WorldTimerEndEventWorldSystem : WorldSystem {
    protected ref array<V30_MapVoting_WorldTimerRef> timers;

    override static void InitInfo(WorldSystemInfo outInfo) {
        super.InitInfo(outInfo);

        outInfo.SetAbstract(false);
        outInfo.SetUnique(false); // Allows to create more optimized variants for specific timers
        outInfo.AddPoint(WorldSystemPoint.Frame);
        outInfo.SetLocation(WorldSystemLocation.Both);
    };

    void AddTimer(notnull V30_MapVoting_WorldTimerRef timer) {
        foreach (auto slot : timers)
            if (slot == timer)
                return;
        AddUniqueTimer(timer);
    };

    void AddUniqueTimer(notnull V30_MapVoting_WorldTimerRef timer) {
        timers.Insert(timer);
    };

    void RemoveTimer(notnull V30_MapVoting_WorldTimerRef timer) {
        timers.RemoveItem(timer);
    };

    [EventAttribute()]
    void TimerEnded(notnull V30_MapVoting_WorldTimerRef timer);

    protected event void OnTimerEnded(notnull V30_MapVoting_WorldTimerRef timer) {
        ThrowEvent(TimerEnded, timer);
    };

    override protected event void OnUpdatePoint(WorldUpdatePointArgs args) {
        super.OnUpdatePoint(args);

        switch (args.GetPoint()) {
            case WorldSystemPoint.Frame: {
                auto toRemove = new array<int>();
                foreach (auto i, auto timer : timers) {
                    if (!timer) {
                        toRemove.Insert(i);
                        continue;
                    };
                    if (timer.IsEnded()) {
                        toRemove.Insert(i);
                        OnTimerEnded(timer);
                    };
                };
                SCR_ArrayHelperT<int>.Reverse(toRemove);
                foreach (auto index : toRemove)
                    timers.Remove(index);
                break;
            };
        };
    };
};
