[BaseContainerProps()]
class V30_MapVoting_Runner {
    protected ref V30_MapVoting_Scenario scenario;

    V30_MapVoting_Scenario GetScenario() {
        return this.scenario;
    };

    void SetScenario(notnull V30_MapVoting_Scenario scenario);

    void PrepareScenarioSwitch();

    void SwitchScenario();
};
