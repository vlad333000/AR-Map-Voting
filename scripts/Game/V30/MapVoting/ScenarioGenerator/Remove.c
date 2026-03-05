[BaseContainerProps(), V30_MapVoting_RemoveScenarioGenerator_CustomTitle()]
class V30_MapVoting_RemoveScenarioGenerator : V30_MapVoting_ScenarioGenerator {
    [Attribute("", desc: "Path to mission's header to remove from scenario list.")]
    protected string missionHeader;
};

class V30_MapVoting_RemoveScenarioGenerator_CustomTitle : V30_MapVoting_ScenarioGenerator_CustomTitle {
	override bool _WB_GetCustomTitle(BaseContainer source, out string title) {
        if (!AddMissionHeaderName(source, title))
            return false;
        AddRemovePrefix(source, title);
        return true;
    };

    protected bool AddRemovePrefix(BaseContainer source, out string title) {
        title = string.Format("Remove: %1", title);
        return true;
    };
};