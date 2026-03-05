class V30_MapVoting_SimpleScenario : V30_MapVoting_Scenario {
    protected ResourceName missionHeader;

    void V30_MapVoting_SimpleScenario(ResourceName missionHeader) {
        this.missionHeader = missionHeader;
    };

    override ResourceName GetMissionHeader() {
        return missionHeader;
    };

    void SetMissionHeader(ResourceName missionHeader) {
        this.missionHeader = missionHeader;
    };
};
