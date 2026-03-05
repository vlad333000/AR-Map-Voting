[BaseContainerProps(description: "Selects random choice from the list of choices.")]
class V30_MapVoting_RandomListChoice : V30_MapVoting_Choice {
    [Attribute()]
    protected string Title;

    [Attribute()]
    protected string SubTitle;

    [Attribute()]
    protected string Image;

    [Attribute()]
    protected ref array<ref V30_MapVoting_Choice> choices;

    override V30_MapVoting_ChoicePreview GetPreview();

    override V30_MapVoting_Scenario GetScenario();
};
