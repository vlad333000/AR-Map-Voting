[BaseContainerProps(), V30_MapVoting_Addon_CustomTitle()]
class V30_MapVoting_Addon {
    [Attribute("", desc: "Addon's GUID.")]
    protected string guid;

    [Attribute("1", desc: "Add = add addon to list, Remove = remove addon from list, Override = clear entire list and add this addon.", uiwidget: UIWidgets.ComboBox, enumType: V30_MapVoting_EAddonAction)]
    protected V30_MapVoting_EAddonAction action;

    [Attribute("", desc: "Addon's name (only for comments, not used).")]
    protected string comment;

    void V30_MapVoting_Addon(string guid, V30_MapVoting_EAddonAction action = V30_MapVoting_EAddonAction.Add) {
        this.guid = guid;
        this.action = action;
        this.comment = "";
    };

    string GetGUID() {
        return this.guid;
    };

    V30_MapVoting_EAddonAction GetAction() {
        return this.action;
    };

    string GetComment() {
        return this.comment;
    };

    bool IsVanillaAddon() {
        return GameProject.IsVanillaAddon(this.guid);
    };

    static void GetVanillaAddons(notnull array<string> outAddons) {
        auto addons = new array<string>();
        GameProject.GetAvailableAddons(addons);
        foreach (auto addon : addons)
            if (GameProject.IsVanillaAddon(addon))
                outAddons.Insert(addon);
    };

    static void GetVanillaAddonList(notnull array<ref V30_MapVoting_Addon> outAddonList) {
        auto addons = new array<string>();
        GetVanillaAddons(addons);
        foreach (auto i, auto addon : addons) {
            auto action = V30_MapVoting_EAddonAction.Add;
            if (i == 0) action = V30_MapVoting_EAddonAction.Override;
            auto outAddon = new V30_MapVoting_Addon(addon, action);
            outAddonList.Insert(outAddon);
        };
    };
};

class V30_MapVoting_Addon_CustomTitle : BaseContainerCustomTitle {
	override bool _WB_GetCustomTitle(BaseContainer source, out string title) {
        string guid;
        if (!source.Get("guid", guid) || guid.IsEmpty()) {
            title = "<MISSING ADDON'S GUID>";
            return true;
        };

        string comment;
        auto addonTitle = GameProject.GetAddonTitle(guid);
        if (addonTitle.IsEmpty())
            addonTitle = GameProject.GetAddonID(guid);
        if (!addonTitle.IsEmpty())
            title = string.Format("%1 (%2)", addonTitle, guid);
        else if (source.Get("comment", comment) && !comment.IsEmpty())
            title = string.Format("%1 (%2)", guid, comment);
        else
            title = string.Format("%1", guid);
        if (!GameProject.IsVanillaAddon(guid))
            title = string.Format("Mod: %1", title);
        return true;
    };
};

enum V30_MapVoting_EAddonAction {
    Override = 0,
    Add = 1,
    Remove = -1
};
