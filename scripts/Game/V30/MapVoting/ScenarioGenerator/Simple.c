[BaseContainerProps(), V30_MapVoting_SimpleScenarioGenerator_CustomTitle()]
class V30_MapVoting_SimpleScenarioGenerator : V30_MapVoting_ScenarioGenerator {
    [Attribute("", desc: "Path to mission's header.", uiwidget: UIWidgets.ResourceNamePicker, params: "conf class=MissionHeader")]
    protected ResourceName missionHeader;

    [Attribute("", desc: "List of tags for grouping multiple scenarios.")]
    protected ref array<string> tags;

    int CountTags() {
        return this.tags.Count();
    };

    string GetTag(int index) {
        return this.tags.Get(index);
    };

    void GetTags(notnull array<string> outTags) {
        foreach (auto tag : this.tags)
            outTags.Insert(tag);
    };
};

class V30_MapVoting_SimpleScenarioGenerator_CustomTitle : V30_MapVoting_ScenarioGenerator_CustomTitle {
	override bool _WB_GetCustomTitle(BaseContainer source, out string title) {
        if (!AddMissionHeaderName(source, title))
            return false;
        AddTags(source, title);
        return true;
    };
};
