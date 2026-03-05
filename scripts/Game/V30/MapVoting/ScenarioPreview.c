class V30_MapVoting_ScenarioPreview {
    protected string title;

    protected string subTitle;

    protected string image;

    void V30_MapVoting_ScenarioPreview(string title, string subTitle, string image) {
        this.title = title;
        this.subTitle = subTitle;
        this.image = image;
    };

    string GetTitle() {
        return this.title;
    };

    string GetSubTitle() {
        return this.subTitle;
    };

    string GetImage() {
        return this.image;
    };

    static V30_MapVoting_ScenarioPreview CreateFromMissionHeader(ResourceName missionHeaderResourceName) {
        auto missionHeader = MissionHeader.ReadMissionHeader(missionHeaderResourceName);
        if (!missionHeader)
            return CreateFromMissionHeaderDefault(missionHeaderResourceName);

        return CreateFromMissionHeader(missionHeader);
    };

    static V30_MapVoting_ScenarioPreview CreateFromMissionHeader(notnull MissionHeader missionHeader) {
        auto scrMissionHeader = SCR_MissionHeader.Cast(missionHeader);
        if (!scrMissionHeader)
            return CreateFromMissionHeaderDefault(missionHeader);

        auto name = scrMissionHeader.m_sName;
        auto author = scrMissionHeader.m_sAuthor;
        auto previewImage = scrMissionHeader.m_sPreviewImage;
        auto gameMode = scrMissionHeader.m_sGameMode;
        auto playerCount = scrMissionHeader.m_iPlayerCount;

        auto owner = "";
        auto missionWorkshopItem = FindMissionWorkshopItem(scrMissionHeader);
        if (missionWorkshopItem) {
            auto missionWorkshopItemOwner = missionWorkshopItem.GetOwner();
            if (!missionWorkshopItemOwner)
                owner = "#AR-Editor_Attribute_OverlayLogo_Reforger";
            else
                owner = missionWorkshopItemOwner.Name();
        }
        else
            owner = scrMissionHeader.m_sAuthor;

        auto title = string.Format("[%1] %2", playerCount, name);
        auto subTitle = string.Format("%1 - %2", owner, gameMode);
        auto image = previewImage;

        return new V30_MapVoting_ScenarioPreview(title, subTitle, image);
    };
	
	static V30_MapVoting_ScenarioPreview CreateFromMissionHeaderDefault(MissionHeader missionHeader) {
		auto missionHeaderResourceName = missionHeader.GetHeaderResourceName();
		return CreateFromMissionHeaderDefault(missionHeaderResourceName);
	};

    static V30_MapVoting_ScenarioPreview CreateFromMissionHeaderDefault(ResourceName missionHeaderResourceName) {
        auto missionHeaderPath = missionHeaderResourceName.GetPath();
        auto missionHeaderFile = FilePath.StripPath(missionHeaderPath);
        return new V30_MapVoting_ScenarioPreview(missionHeaderFile, missionHeaderResourceName, "{31F6670FAB57B6FE}UI/Textures/V30_MapVoting_Choice_UnknownImage.edds");
    };

    protected static MissionWorkshopItem FindMissionWorkshopItem(ResourceName missionHeaderResourceName) {
		auto workshopItems = new array<MissionWorkshopItem>();
        GetAllMissionWorkshopItems(workshopItems);

        foreach (auto workshopItem : workshopItems) {
            auto workshopItemMissionHeaderResourceName = workshopItem.Id();

            if (missionHeaderResourceName == workshopItemMissionHeaderResourceName)
                return workshopItem;
        };

        return null;
    };

    protected static MissionWorkshopItem FindMissionWorkshopItem(notnull MissionHeader missionHeader) {
        auto missionHeaderResourceName = missionHeader.GetHeaderResourceName();
        return FindMissionWorkshopItem(missionHeaderResourceName);
    };

    protected static void GetAllMissionWorkshopItems(notnull array<MissionWorkshopItem> outMissionWorkshopItems) {
        auto game = GetGame();
        if (!game)
            return;

        auto backendApi = game.GetBackendApi();
        if (!backendApi)
            return;

        auto workshop = backendApi.GetWorkshop();
        if (!workshop)
            return;

        auto pageMissions = new array<MissionWorkshopItem>();
		auto pageMissionsCount = workshop.GetPageScenarios(pageMissions, 0, SCR_WorkshopUiCommon.PAGE_SCENARIOS);
        while (pageMissionsCount > 0) {
            outMissionWorkshopItems.InsertAll(pageMissions);
            pageMissions.Clear();
		    pageMissionsCount = workshop.GetPageScenarios(pageMissions, 0, SCR_WorkshopUiCommon.PAGE_SCENARIOS);
        };
    };
};
