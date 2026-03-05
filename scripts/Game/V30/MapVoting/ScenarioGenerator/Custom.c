[BaseContainerProps(), V30_MapVoting_CustomScenarioGenerator_CustomTitle()]
class V30_MapVoting_CustomScenarioGenerator : V30_MapVoting_ScenarioGenerator {
    [Attribute("", desc: "Path to mission's header.")]
    protected string missionHeader;

    [Attribute("", desc: "Path to world's systems config.")]
    protected string worldSystemsConfig;

    [Attribute("", desc: "Addons to load for this scenario.")]
    protected ref array<ref V30_MapVoting_Addon> addonList;

    [Attribute("", desc: "Custom title for this scenario (If empty tries to get it from mission's header).")]
    protected string customTitle;

    [Attribute("", desc: "Custom subtitle for this scenario (If empty tries to get it from mission's header).")]
    protected string customSubTitle;

    [Attribute("", desc: "Custom image for this scenario (If empty tries to get it from mission's header).")]
    protected string image;

    [Attribute("", desc: "List of tags for grouping multiple scenarios.")]
    protected ref array<string> tags;

    string GetMissionHeader() {
        return this.missionHeader;
    };

    string GetWorldSystemsConfig() {
        return this.worldSystemsConfig;
    };

    int CountAddonList() {
        return this.addonList.Count();
    };

    V30_MapVoting_Addon GetAddon(int index) {
        return this.addonList.Get(index);
    };

    void GetAddonList(notnull array<ref V30_MapVoting_Addon> outAddonList) {
        foreach (auto addon : this.addonList)
           outAddonList.Insert(addon);
    };

    string GetCustomTitle() {
        return this.customTitle;
    };

    string GetCustomSubTitle() {
        return this.customSubTitle;
    };

    string GetImage() {
        return this.image;
    };

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

class V30_MapVoting_CustomScenarioGenerator_CustomTitle : V30_MapVoting_ScenarioGenerator_CustomTitle {
	override bool _WB_GetCustomTitle(BaseContainer source, out string title) {
        if (AddCustomTitleFull(source, title)) {
            AddTags(source, title);
            return true;
        }
        else if (AddMissionHeaderName(source, title)) {
            AddTags(source, title);
            return true;
        };
        return false;
    };

    protected bool AddCustomTitleFull(BaseContainer source, out string title) {
        string customTitleFull;
        string customTitle;
        if (!AddCustomTitle(source, customTitle))
            if (!AddMissionHeaderName(source, customTitle))
                return false;
        string customSubTitle;
        if (AddCustomSubTitle(source, customSubTitle))
            title += string.Format("%1 - %2", customTitle, customSubTitle);
        else
            title += customTitle;
        return true;
    };

    protected bool AddCustomTitle(BaseContainer source, out string title, string param = "customTitle") {
        string customTitle;
        if (!source.Get("customTitle", customTitle))
            return false;
        if (customTitle.IsEmpty())
            return false;
        title += customTitle;
        return true;
    };

    protected bool AddCustomSubTitle(BaseContainer source, out string title, string param = "customSubTitle") {
        string customSubTitle;
        if (!source.Get("customSubTitle", customSubTitle))
            return false;
        if (customSubTitle.IsEmpty())
            return false;
        title += customSubTitle;
        return true;
    };
};
