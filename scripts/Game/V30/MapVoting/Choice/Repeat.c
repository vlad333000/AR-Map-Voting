[BaseContainerProps()]
class V30_MapVoting_RepeatChoice : V30_MapVoting_Choice {
    static V30_MapVoting_RepeatChoice Create();

    override V30_MapVoting_ChoicePreview GetPreview();

    override V30_MapVoting_Scenario GetScenario();
};
