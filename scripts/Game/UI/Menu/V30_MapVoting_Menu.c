class V30_MapVoting_Menu : ChimeraMenuBase {
	protected V30_MapVoting_VotingComponent m_VotingComponent;

	protected V30_MapVoting_MenuUIComponent m_MenuUIComponent;

	protected SCR_InputButtonComponent m_ChatButtonComponent;

	protected SCR_InputButtonComponent m_PauseButtonComponent;

	protected SCR_InputButtonComponent m_VONButtonDirectComponent;

	protected SCR_ChatPanel m_ChatPanel;

	protected SCR_HUDMenuComponent m_HUDMenuComponent;

	protected Widget m_Choices;

	protected Widget m_Hud;

	protected void Golosovanie(SCR_ButtonComponent comp, bool state) {
		PrintFormat("%1.Golosovanie(%2, %3)", this, comp, state);
		auto world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (!world) return;
		auto musicManager = world.GetMusicManager();
		if (!musicManager) return;

		if (state) {
			//musicManager.Stop("SOUND_ONSPAWN");
			musicManager.Play("SOUND_GOLOSOVANIE");
		}
		else {
			musicManager.Stop("SOUND_GOLOSOVANIE");
			//musicManager.Play("SOUND_ONSPAWN");
		};
	};

	override void OnMenuOpen() {
		super.OnMenuOpen();

		auto root = GetRootWidget();

		auto golosovanie = root.FindAnyWidget("Golosovanie");
		if (golosovanie) {
			auto comp = SCR_ButtonComponent.Cast(golosovanie.FindHandler(SCR_ButtonComponent));
			if (comp) {
				comp.m_OnToggled.Insert(Golosovanie);
			};
		};

		auto world = ChimeraWorld.CastFrom(GetGame().GetWorld());
		if (world) {
			auto musicManager = world.GetMusicManager();
			if (musicManager) {
				//musicManager.Play("SOUND_ONSPAWN");
			};
		};

		m_ChatButtonComponent = SCR_InputButtonComponent.GetInputButtonComponent("ChatButton", root);
		if (m_ChatButtonComponent) m_ChatButtonComponent.m_OnActivated.Insert(OnChatToggle);

		m_PauseButtonComponent = SCR_InputButtonComponent.GetInputButtonComponent("PauseButton", root);
		if (m_PauseButtonComponent) m_PauseButtonComponent.m_OnActivated.Insert(OnPauseMenu);

		m_HUDMenuComponent = SCR_HUDMenuComponent.Cast(SCR_HUDMenuComponent.GetComponent(SCR_HUDMenuComponent, "HUD", root));

		m_MenuUIComponent = V30_MapVoting_MenuUIComponent.Cast(root.FindHandler(V30_MapVoting_MenuUIComponent));

		m_Choices = root.FindAnyWidget("Choices");
		m_Choices.SetEnabled(false);
		m_Choices.SetVisible(false);
	};

	/*saeled*/ void Setup(notnull V30_MapVoting_VotingComponent votingComponent) {
		m_VotingComponent = votingComponent;

        auto menuUiComponent = V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_MenuUIComponent>.FindHandler(GetRootWidget());
		menuUiComponent.Setup(votingComponent);

		if (votingComponent.IsAllChoicesLoaded())
			GetGame().GetCallqueue().CallLater(OnAllChoicesLoaded, delay: 0, param1: votingComponent, param2: votingComponent.GetAllChoices());
		else
			votingComponent.GetOnAllChoicesLoaded().Insert(OnAllChoicesLoaded);
	};

	protected void OnAllChoicesLoaded(notnull V30_MapVoting_VotingComponent component, notnull map<V30_MapVoting_ChoiceId, ref V30_MapVoting_Choice> choices) {
		auto screenWidget = GetRootWidget().FindAnyWidget("ChoicesWrapper");
		auto screenUiComponent = V30_MapVoting_WidgetHandlerHelperT<V30_MapVoting_ScreenUIComponent>.FindHandler(screenWidget);
		screenUiComponent.Setup(component);

		m_Choices.SetEnabled(true);
		m_Choices.SetVisible(true);
	};

	protected void OnChatToggle() {
		if (!m_ChatPanel) {
			auto chat = GetRootWidget().FindAnyWidget("ChatPanel");
			if (chat) m_ChatPanel = SCR_ChatPanel.Cast(chat.FindHandler(SCR_ChatPanel));
		};

		if (!m_ChatPanel || m_ChatPanel.IsOpen()) return;

		SCR_ChatPanelManager chatPanelManager = SCR_ChatPanelManager.GetInstance();
		if (!chatPanelManager) return;

		SCR_ChatPanelManager.GetInstance().ToggleChatPanel(m_ChatPanel);
	};

	protected void OnPauseMenu() {
		GetGame().OpenPauseMenu(false, true);
	};

	override event void OnMenuFocusGained() {
		super.OnMenuFocusGained();
		m_Hud = GetGame().GetWorkspace().CreateWidgets("{63397C60B1761AD9}UI/layouts/V30/MapVoting/V30_MapVoting_HUD.layout", GetRootWidget());
	};

	override event void OnMenuFocusLost() {
		super.OnMenuFocusLost();
		m_Hud.RemoveFromHierarchy();
		m_Hud = null;
	};
};
