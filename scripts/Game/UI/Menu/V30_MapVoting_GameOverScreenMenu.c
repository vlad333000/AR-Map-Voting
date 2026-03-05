class V30_MapVoting_GameOverScreenVotingMenu : ChimeraMenuBase {
    protected Widget rootWidget;

    protected Widget backButton;

    protected SCR_InputButtonComponent backButtonComponent;

    protected Widget chatButton;

    protected SCR_InputButtonComponent chatButtonComponent;

    protected Widget debriefingButton;

    protected SCR_InputButtonComponent debriefingButtonComponent;

    protected SCR_ChatPanel chatPanel;

    protected Widget restartTimer;

    protected V30_MapVoting_VotingWorldSystem m_VotingComponent;

    protected Widget content;

    protected Widget contentChoices;

	protected Widget contentNextScenario;

	override void OnMenuOpen() {
		super.OnMenuOpen();

        auto rootWidget = GetRootWidget();

        backButton = rootWidget.FindAnyWidget("BackButton");
		backButtonComponent = SCR_InputButtonComponent.GetInputButtonComponent("BackButton", rootWidget);
		if (backButtonComponent)
			backButtonComponent.m_OnActivated.Insert(OnBackButtonActivated);

		chatButton = rootWidget.FindAnyWidget("ChatButton");
		chatButtonComponent = SCR_InputButtonComponent.GetInputButtonComponent("ChatButton", rootWidget);
		if (chatButtonComponent)
			chatButtonComponent.m_OnActivated.Insert(OnChatButtonActivated);

		debriefingButton = rootWidget.FindAnyWidget("DebriefingButton");
		debriefingButtonComponent = SCR_InputButtonComponent.GetInputButtonComponent("DebriefingButton", rootWidget);
		if (debriefingButtonComponent)
			debriefingButtonComponent.m_OnActivated.Insert(OnDebriefingButtonActivated);

		Widget chatPanelWidget = GetRootWidget().FindAnyWidget("ChatPanel");
		if (chatPanelWidget)
			chatPanel = SCR_ChatPanel.Cast(chatPanelWidget.FindHandler(SCR_ChatPanel));

		restartTimer = GetRootWidget().FindAnyWidget("RestartTimerHolder");
		if (restartTimer)
			restartTimer.SetVisible(true);

        content = rootWidget.FindAnyWidget("Content");
        if (content) {
            content.SetVisible(false);

            contentChoices = content.FindAnyWidget("ChoicesWrapper");
            contentNextScenario = content.FindAnyWidget("NextScenario");
        };

        auto game = GetGame();
        auto gameMode = game.GetGameMode();
        auto votingComponent = V30_MapVoting_VotingWorldSystem.Cast(gameMode.FindComponent(V30_MapVoting_VotingWorldSystem));
        Setup(votingComponent);
    };

    void Setup(notnull V30_MapVoting_VotingWorldSystem votingComponent) {
		m_VotingComponent = votingComponent;

        // auto root = GetRootWidget();
        // auto menuUiComponent = V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_MenuUIComponent>.FindHandler(root);
		// menuUiComponent.Setup(votingComponent);

        content.SetVisible(true);
        contentChoices.SetVisible(false);
        contentNextScenario.SetVisible(false);
    };

    protected event void OnAllChoicesLoaded(notnull V30_MapVoting_VotingWorldSystem votingComponent, notnull map<int, ref V30_MapVoting_Choice> choices) {

		auto screenUiComponent = V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_ScreenUIComponent>.FindHandler(contentChoices);
		screenUiComponent.Setup(votingComponent);
    };

    protected event void OnVoteEnded(notnull V30_MapVoting_VotingWorldSystem votingComponent, int winnerId) {
		contentChoices.SetVisible(false);
        contentNextScenario.SetVisible(true);
		auto contentNextScenarioComponent = V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_WinnerScenarioWidgetComponent>.FindHandler(contentNextScenario);
		contentNextScenarioComponent.Setup(m_VotingComponent);
    };

    override void OnMenuOpened() {
        super.OnMenuOpened();
	    MuteSounds();
    };

	override void OnMenuUpdate(float tDelta) {
		super.OnMenuUpdate(tDelta);

		if (chatPanel)
			chatPanel.OnUpdateChat(tDelta);
	};

    override void OnMenuClose() {
		UnmuteSounds();
		super.OnMenuClose();
	};

    override void OnMenuShow() {
        super.OnMenuShow();
	    MuteSounds();
    };

	override void OnMenuHide() {
		UnmuteSounds();
        super.OnMenuHide();
	};

    protected event void OnBackButtonActivated() {
        Close();
    };

    protected event void OnChatButtonActivated() {
		if (!chatPanel || chatPanel.IsOpen() || (chatButton && (!chatButton.IsVisible() || chatButton.GetOpacity() != 1)))
			return;

		SCR_ChatPanelManager.GetInstance().ToggleChatPanel(chatPanel);
    };

    protected event void OnDebriefingButtonActivated() {
		GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.DebriefingScreenMenu);
    };

    protected void MuteSounds() {
		AudioSystem.SetMasterVolume(AudioSystem.SFX, false);
		AudioSystem.SetMasterVolume(AudioSystem.VoiceChat, false);
		AudioSystem.SetMasterVolume(AudioSystem.Dialog, false);
    };

    protected void UnmuteSounds() {
		AudioSystem.SetMasterVolume(AudioSystem.SFX, true);
		AudioSystem.SetMasterVolume(AudioSystem.VoiceChat, true);
		AudioSystem.SetMasterVolume(AudioSystem.Dialog, true);
    };
};