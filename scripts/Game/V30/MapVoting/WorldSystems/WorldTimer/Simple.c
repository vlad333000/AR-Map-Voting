class V30_MapVoting_SimpleWorldTimer : V30_MapVoting_WorldTimer {
    protected WorldTimestamp end;

    void V30_MapVoting_SimpleWorldTimer(WorldTimestamp end) {
        this.end = end;
    };

    override WorldTimestamp GetEndTimestamp() {
        return end;
    };

    void SetEndTimestamp(WorldTimestamp end) {
        this.end = end;
    };
};

class V30_MapVoting_SimpleWorldTimerRef : V30_MapVoting_WorldTimerRef {
    void SetEndTimestamp(WorldTimestamp end) {
        GetSimpleSystem().SetTimerEndTimestamp(timerId, end);
    };

    V30_MapVoting_SimpleWorldTimerWorldSystem GetSimpleSystem() {
        return V30_MapVoting_SimpleWorldTimerWorldSystem.Cast(GetSystem());
    };

    V30_MapVoting_SimpleWorldTimer GetSimpleTimer() {
        return V30_MapVoting_SimpleWorldTimer.Cast(GetTimer());
    };

    // friend V30_MapVoting_SimpleWorldTimerWorldSystem.SetTimerEndTimestamp;
};

class V30_MapVoting_SimpleWorldTimerWorldSystem : V30_MapVoting_WorldTimerWorldSystem {
    override static void InitInfo(WorldSystemInfo outInfo) {
        super.InitInfo(outInfo);

        outInfo.SetAbstract(false);
        outInfo.SetUnique(false);
        outInfo.SetLocation(WorldSystemLocation.Both);
    };

    V30_MapVoting_SimpleWorldTimerRef CreateTimer(WorldTimestamp end) {
        auto timer = new V30_MapVoting_SimpleWorldTimer(end);
        auto timerId = AddTimer(timer);
        return new V30_MapVoting_SimpleWorldTimerRef(this, timerId);
    };

    V30_MapVoting_SimpleWorldTimerRef CreateTimerForSeconds(float seconds) {
        auto now = GetCurrentTimestamp();
        auto end = now.PlusSeconds(seconds);
        return CreateTimer(end);
    };

    V30_MapVoting_SimpleWorldTimerRef CreateTimerForMilliseconds(float milliseconds) {
        auto now = GetCurrentTimestamp();
        auto end = now.PlusMilliseconds(milliseconds);
        return CreateTimer(end);
    };

	/*protected*/ V30_MapVoting_SimpleWorldTimer GetSimpleTimer(V30_MapVoting_WorldTimerId timerId) {
		return V30_MapVoting_SimpleWorldTimer.Cast(GetTimer(timerId));
	};

    /*protected*/ void SetTimerEndTimestamp(V30_MapVoting_WorldTimerId timerId, WorldTimestamp end) {
        GetSimpleTimer(timerId).SetEndTimestamp(end);
    };
};
