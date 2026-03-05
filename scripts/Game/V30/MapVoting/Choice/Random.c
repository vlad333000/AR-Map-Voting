[BaseContainerProps(description: "Selects random choice from the list of choices.")]
class V30_MapVoting_RandomChoice : V30_MapVoting_Choice {
    [Attribute()]
    protected V30_MapVoting_ERandomChoiceList list;

    override V30_MapVoting_ChoicePreview GetPreview();

    override V30_MapVoting_Scenario GetScenario();
};

enum V30_MapVoting_ERandomChoiceList {
    VOTE_INCLUDED,
    VOTE_EXCLUDED,
    ALL
};

enum V30_MapVoting_ERepeatMode {
    DEFAULT,
    REPEAT_ALLOWED,
    REPEAT_NOT_ALLOWED
};
