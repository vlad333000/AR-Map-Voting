class V30_MapVoting_Scenario {
    protected ref array<string> tags;

    ResourceName GetMissionHeader();

    ResourceName GetWorldSystemsConfig() {
        return GetDefaultWorldSystemsConfig();
    };

    int CountAddonList() {
        auto addonList = new array<string>();
        GetAddonList(addonList);
		return addonList.Count();
    };

    string GetAddon(int index) {
        auto addonList = new array<string>();
        GetAddonList(addonList);
        return addonList.Get(index);
    };

    void GetAddonList(notnull array<string> outAddonList) {
        GetDefaultAddonList(outAddonList);
    };

    V30_MapVoting_ScenarioPreview GetPreview() {
		return GetDefaultPreview();
    };

    protected ResourceName GetDefaultWorldSystemsConfig() {
        auto missionHeaderResourceName = GetMissionHeader();
        return MissionHeader.ReadMissionHeader(missionHeaderResourceName).GetWorldSystemsConfig();
    };

    protected void GetDefaultAddonList(notnull array<string> outAddonList) {
        GetCurrentAddonList(outAddonList);
    };

    protected V30_MapVoting_ScenarioPreview GetDefaultPreview() {
        auto missionHeaderResourceName = GetMissionHeader();
        return V30_MapVoting_ScenarioPreview.CreateFromMissionHeader(missionHeaderResourceName);
    };

    static protected void GetCurrentAddonList(notnull array<string> outAddonList) {
		GameProject.GetLoadedAddons(outAddonList);
    };
};
