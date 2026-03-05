class V30_MapVoting_PausableWorldTimer : V30_MapVoting_SimpleWorldTimer {
    protected bool isPaused;

    protected WorldTimestamp paused;

    void V30_MapVoting_PausableWorldTimer(WorldTimestamp end) {
        isPaused = false;
    };

    void Pause(WorldTimestamp paused) {
        isPaused = true;
        this.paused = paused;
    };

    void Resume(WorldTimestamp end) {
        isPaused = false;
		this.end = end;
    };

    bool IsPaused() {
        return isPaused;
    };

    WorldTimestamp GetPausedTimestamp() {
        return paused;
    };
};

class V30_MapVoting_PausableWorldTimerRef : V30_MapVoting_SimpleWorldTimerRef {
    void Pause() {
        GetPausableSystem().PauseTimer(timerId);
    };

     void Resume() {
        GetPausableSystem().ResumeTimer(timerId);
    };

    bool IsPaused() {
        return GetPausableSystem().IsTimerPaused(timerId);
    };

    override bool IsEnded() {
        return !IsPaused() && super.IsEnded();
    };

    override WorldTimestamp GetEndTimestamp() {
        if (IsPaused()) {
            auto end = super.GetEndTimestamp();
            auto pauseTime = GetPauseTime();
            return end + pauseTime;
        };
        return super.GetEndTimestamp();
    };

    WorldTimestamp GetPausedTimestamp() {
        return GetPausableSystem().GetTimerPausedTimestamp(timerId);
    };

    WorldTimestamp GetPauseTime() {
        auto now = GetSystem().GetCurrentTimestamp();
        auto paused = GetPausedTimestamp();
        return now - paused;
    };

    float GetPauseSeconds() {
        auto now = GetSystem().GetCurrentTimestamp();
        auto paused = GetPausedTimestamp();
        return now.DiffSeconds(paused);
    };

    float GetPauseMilliseconds() {
        auto now = GetSystem().GetCurrentTimestamp();
        auto paused = GetPausedTimestamp();
        return now.DiffMilliseconds(paused);
    };

    V30_MapVoting_PausableWorldTimerWorldSystem GetPausableSystem() {
        return V30_MapVoting_PausableWorldTimerWorldSystem.Cast(GetSystem());
    };

    V30_MapVoting_PausableWorldTimer GetPausableTimer() {
        return V30_MapVoting_PausableWorldTimer.Cast(GetTimer());
    };

    // friend V30_MapVoting_SimpleWorldTimerRef.SetEndTimestamp;

    // friend V30_MapVoting_PausableWorldTimerWorldSystem.PauseTimer;

    // friend V30_MapVoting_PausableWorldTimerWorldSystem.ResumeTimer;

    // friend V30_MapVoting_PausableWorldTimerWorldSystem.IsTimerPaused;

    // friend V30_MapVoting_PausableWorldTimerWorldSystem.GetTimerPausedTimestamp;
};

class V30_MapVoting_PausableWorldTimerWorldSystem : V30_MapVoting_SimpleWorldTimerWorldSystem {
    override static void InitInfo(WorldSystemInfo outInfo) {
        super.InitInfo(outInfo);

        outInfo.SetAbstract(false);
        outInfo.SetUnique(false);
        outInfo.SetLocation(WorldSystemLocation.Both);
    };

    V30_MapVoting_SimpleWorldTimerRef CreateTimer(WorldTimestamp end, bool start = true) {
        auto timer = new V30_MapVoting_PausableWorldTimer(end);
        auto timerId = AddTimer(timer);
		if (!start)
			PauseTimer(timerId);
        return new V30_MapVoting_PausableWorldTimerRef(this, timerId);
    };

	/*protected*/ V30_MapVoting_PausableWorldTimer GetPausableTimer(V30_MapVoting_WorldTimerId timerId) {
		return V30_MapVoting_PausableWorldTimer.Cast(GetTimer(timerId));
	};

    /*protected*/ void PauseTimer(V30_MapVoting_WorldTimerId timerId) {
		auto now = GetCurrentTimestamp();
        GetPausableTimer(timerId).Pause(now);
    };

    /*protected*/ void ResumeTimer(V30_MapVoting_WorldTimerId timerId) {
		auto timer = GetPausableTimer(timerId);
		auto now = GetCurrentTimestamp();
		auto paused = timer.GetPausedTimestamp();
		auto endOld = timer.GetEndTimestamp();
		auto endNew = endOld + (now - paused);
        timer.Resume(endNew);
    };

    /*protected*/ bool IsTimerPaused(V30_MapVoting_WorldTimerId timerId) {
        return GetPausableTimer(timerId).IsPaused();
    };

    /*protected*/ WorldTimestamp GetTimerPausedTimestamp(V30_MapVoting_WorldTimerId timerId) {
        return GetPausableTimer(timerId).GetPausedTimestamp();
    };
};
