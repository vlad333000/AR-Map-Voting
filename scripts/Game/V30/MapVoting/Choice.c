[BaseContainerProps()]
class V30_MapVoting_Choice {
    V30_MapVoting_ChoicePreview GetPreview();

    V30_MapVoting_Scenario GetScenario();
};

[BaseContainerProps()]
class V30_MapVoting_ChoicePreview {
    protected string Title;

    protected string SubTitle;

    protected string Image;

    void V30_MapVoting_ChoicePreview(string title, string subTitle, string image) {
        this.Title = title;
        this.SubTitle = subTitle;
        this.Image = image;
    };
};
