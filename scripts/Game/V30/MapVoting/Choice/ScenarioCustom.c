[BaseContainerProps()]
class V30_MapVoting_SimpleCustomChoice : V30_MapVoting_Choice {
    [Attribute()]
    protected string Title;

    [Attribute()]
    protected string SubTitle;

    [Attribute()]
    protected string Image;

    [Attribute()]
    protected string scenarioHeader;

    [Attribute()]
    protected ref array<string> addonsChanges;

    override V30_MapVoting_ChoicePreview GetPreview();

    override V30_MapVoting_Scenario GetScenario();
};
