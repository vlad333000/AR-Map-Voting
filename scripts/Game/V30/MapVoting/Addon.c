[BaseContainerProps(), V30_MapVoting_Addon_CustomTitle()]
class V30_MapVoting_Addon {
    [Attribute("", desc: "Addon's name (only for comments, not used).")]
    protected string name;

    [Attribute("", desc: "Addon's GUID.")]
    protected string guid;

    [Attribute("1", desc: "Add = add addon to list, Remove = remove addon from list, Override = clears list for first occurence and then adds.", uiwidget: UIWidgets.ComboBox, enumType: V30_MapVoting_EAddonAction)]
    protected V30_MapVoting_EAddonAction action;

    string GetName() {
        return this.name;
    };

    string GetGUID() {
        return this.guid;
    };

    V30_MapVoting_EAddonAction GetAction() {
        return this.action;
    };
};

class V30_MapVoting_Addon_CustomTitle : BaseContainerCustomTitle {
	override bool _WB_GetCustomTitle(BaseContainer source, out string title) {
        string name;
        string guid;
        auto isName = source.Get("name", name);
        auto isGuid = source.Get("guid", guid);
        if (!isName && !isGuid)
            return false;

        if (!name.IsEmpty() && !guid.IsEmpty()) {
            title = string.Format("%1 (%2)", name, guid);
            return true;
        }
        else if (!name.IsEmpty()) {
            title = name;
            return true;
        }
        else if (!guid.IsEmpty()) {
            title = guid;
            return true;
        }
        else {
            return false;
        };
    };
};

enum V30_MapVoting_EAddonAction {
    Override = 0,
    Add = 1,
    Remove = -1
};
