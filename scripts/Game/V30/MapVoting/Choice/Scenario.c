[BaseContainerProps()]
class V30_MapVoting_SimpleChoice : V30_MapVoting_Choice {
    [Attribute()]
    protected ResourceName scenarioHeader;

    [Attribute()]
    protected ref array<string> addonsChanges;

    override V30_MapVoting_ChoicePreview GetPreview();

    override V30_MapVoting_Scenario GetScenario();
};
