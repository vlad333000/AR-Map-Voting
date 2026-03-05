[BaseContainerProps(visible: false, insertable: false)]
class V30_MapVoting_ScenarioGenerator {
    void V30_MapVoting_ScenarioGenerator(notnull V30_Json_object config);

    void Generate(notnull array<ref V30_MapVoting_Scenario> outScenarios);
};

class V30_MapVoting_ScenarioGenerator_CustomTitle : BaseContainerCustomTitle {
    protected MissionHeader GetMissionHeader(ResourceName missionHeaderResourceName) {
        if (missionHeaderResourceName.IsEmpty())
            return null;
        return MissionHeader.ReadMissionHeader(missionHeaderResourceName);
    };

    protected SCR_MissionHeader GetScrMissionHeader(ResourceName missionHeaderResourceName) {
        auto missionHeader = GetMissionHeader(missionHeaderResourceName);
        if (!missionHeader)
            return null;
        return SCR_MissionHeader.Cast(missionHeader);
    };

    protected string GetMissionHeaderNameRaw(ResourceName missionHeaderResourceName) {
        auto missionHeader = GetScrMissionHeader(missionHeaderResourceName);
        if (!missionHeader)
            return "";
        return missionHeader.m_sName;
    };

    protected string GetMissionHeaderName(ResourceName missionHeaderResourceName) {
        auto name = GetMissionHeaderNameRaw(missionHeaderResourceName);
        if (name.IsEmpty())
            return missionHeaderResourceName;
        if (name.StartsWith("#"))
            name = WidgetManager.Translate(name);
        return name;
    };

    protected bool AddMissionHeaderName(BaseContainer source, out string title, string param = "missionHeader") {
        ResourceName missionHeaderResourceName;
        if (!source.Get(param, missionHeaderResourceName))
            return false;
        if (missionHeaderResourceName.IsEmpty())
            return false;
        title += GetMissionHeaderName(missionHeaderResourceName);
        return true;
    };

    protected void AddTags(notnull array<string> tags, out string title) {
        auto f = false;
        foreach (auto i, auto tag : tags) {
            if (tag.IsEmpty())
                continue;
            if (f)
                title += " ";
            f = true;
            title += " #" + tag;
        };
    };

    protected bool AddTags(BaseContainer source, out string title, string param = "tags") {
        auto tags = new array<string>();
        if (!source.Get(param, tags))
            return false;
        title += " ";
        AddTags(tags, title);
        return true;
    };
};
