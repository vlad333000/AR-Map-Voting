class V30_MapVoting_ChoiceUIComponent : SCR_ScriptedWidgetComponent {
	protected /*private*/ V30_MapVoting_GameModeComponent m_VotingComponent;
	
	protected /*private*/ V30_MapVoting_ChoiceId m_ChoiceId;
	
	override event void HandlerAttached(Widget w) {
		super.HandlerAttached(w);
		m_VotingComponent = null;
		m_ChoiceId = V30_MapVoting_NoChoice;
	};
	
	override event void HandlerDeattached(Widget w) {
		m_VotingComponent = null;
		m_ChoiceId = V30_MapVoting_NoChoice;
		super.HandlerDeattached(w);
	};
	
	/*modded*/ void SetupChoice(notnull V30_MapVoting_GameModeComponent votingComponent, V30_MapVoting_ChoiceId choiceId) {
		m_VotingComponent = votingComponent;
		m_ChoiceId = choiceId;
	};

	/*modded*/ void ClearChoice() {
		m_VotingComponent = null;
		m_ChoiceId = V30_MapVoting_NoChoice;
	};
	
	/*sealed*/ void Vote() {
		auto choiceId = GetChoiceId();
		V30_MapVoting_PlayerControllerComponent.GetLocalInstance().SetVote(choiceId);
	};
	
	/*sealed*/ V30_MapVoting_GameModeComponent GetVotingComponent() {
		return m_VotingComponent;
	};
	
	/*sealed*/ V30_MapVoting_ChoiceId GetChoiceId() {
		return m_ChoiceId;
	};
	
	/*sealed*/ V30_MapVoting_Choice GetChoice() {
		auto choiceId = GetChoiceId();
		return m_VotingComponent.GetChoice(choiceId);
	};

	/*sealed*/ static V30_MapVoting_ChoiceUIComponent GetInstance(notnull Widget w) {
		return V30_MapVoting_ChoiceUIComponent.Cast(w.FindHandler(V30_MapVoting_ChoiceUIComponent));
	};
};

class V30_MapVoting_SimpleChoiceUIComponent : V30_MapVoting_ChoiceUIComponent {
	[Attribute("", desc: "Name of widget for number of votes for this option.", uiwidget: UIWidgets.EditBox)];
	protected string m_sVoteCountWidgetName;

	[Attribute("", desc: "Name of widget for check mark.", uiwidget: UIWidgets.EditBox)];
	protected string m_sCheckWidgetName;

	[Attribute("", desc: "Name of widget for image.", uiwidget: UIWidgets.EditBox)];
	protected string m_sImageWidgetName;

	[Attribute("", desc: "Name of widget for title text.", uiwidget: UIWidgets.EditBox)];
	protected string m_sTitleWidgetName;

	[Attribute("", desc: "Name of widget for sub-title text.", uiwidget: UIWidgets.EditBox)];
	protected string m_sSubTitleWidgetName;

	protected ref V30_MapVoting_Choice m_Choice;

	protected ButtonWidget m_RootWidget;

	protected SCR_ButtonBaseComponent m_ButtonComponent;

	protected TextWidget m_VoteCountWidget;

	protected Widget m_CheckWidget;

	protected ImageWidget m_ImageWidget;

	protected TextWidget m_TitleWidget;

	protected TextWidget m_SubTitleWidget;

	protected V30_MapVoting_PlayerControllerComponent m_PlayerControllerComponent;

	override event void HandlerAttached(Widget w) {
		super.HandlerAttached(w);
		m_ChoiceId = V30_MapVoting_NoChoice;
		m_RootWidget = ButtonWidget.Cast(w);
		m_ButtonComponent = SCR_ButtonBaseComponent.Cast(m_RootWidget.FindHandler(SCR_ButtonBaseComponent));
		m_ButtonComponent.m_OnClicked.Insert(OnClicked);
		m_VoteCountWidget = TextWidget.Cast(m_RootWidget.FindAnyWidget(m_sVoteCountWidgetName));
		m_CheckWidget = m_RootWidget.FindAnyWidget(m_sCheckWidgetName);
		if (m_CheckWidget) m_CheckWidget.SetVisible(false);
		m_ImageWidget = ImageWidget.Cast(m_RootWidget.FindAnyWidget(m_sImageWidgetName));
		m_TitleWidget = TextWidget.Cast(m_RootWidget.FindAnyWidget(m_sTitleWidgetName));
		m_SubTitleWidget = TextWidget.Cast(m_RootWidget.FindAnyWidget(m_sSubTitleWidgetName));
		if (!GetGame().InPlayMode()) return;
		GetGame().GetCallqueue().CallLater(DelayedAddOnPlayerVoteChanged, repeat: true);
	};

	protected void DelayedAddOnPlayerVoteChanged() {
		auto game = GetGame();
		if (!game) return;
		auto playerController = game.GetPlayerController();
		if (!playerController) return;
		m_PlayerControllerComponent = V30_MapVoting_PlayerControllerComponent.Cast(playerController.FindComponent(V30_MapVoting_PlayerControllerComponent));
		if (!m_PlayerControllerComponent) return;
		m_PlayerControllerComponent.GetOnVoteChanged().Insert(OnPlayerVoteChanged);
		GetGame().GetCallqueue().Remove(DelayedAddOnPlayerVoteChanged);
	};

	/*sealed*/ override void SetupChoice(notnull V30_MapVoting_GameModeComponent votingComponent, V30_MapVoting_ChoiceId choiceId) {
		auto choice = votingComponent.GetChoice(choiceId);
		m_ChoiceId = choiceId;
		m_Choice = choice;
		auto choicePreview = choice.GetPreviewData();
		if (m_VoteCountWidget) UpdateVoteCount();
		if (m_ImageWidget) {
			m_ImageWidget.LoadImageTexture(0, choicePreview.GetImage());
			int x, y;
			m_ImageWidget.GetImageSize(0, x, y);
			m_ImageWidget.SetSize(x, y);
		};
		if (m_TitleWidget) m_TitleWidget.SetText(choicePreview.GetTitle());
		if (m_SubTitleWidget) m_SubTitleWidget.SetText(choicePreview.GetSubTitle());
		choice.GetOnPlayerVoteAdded().Insert(OnChoiceVoteAdded);
		choice.GetOnPlayerVoteRemoved().Insert(OnChoiceVoteRemoved);
	};

	/*sealed*/ override void ClearChoice() {
		if (m_VoteCountWidget)
			m_VoteCountWidget.SetText("—");
		if (m_ImageWidget) {
			m_ImageWidget.LoadImageTexture(0, "{FB84DD112A84AB32}UI/Textures/LoadingScreen/PlaceholderScenery_UI.edds");
			int x, y;
			m_ImageWidget.GetImageSize(0, x, y);
			m_ImageWidget.SetSize(x, y);
		};
		if (m_TitleWidget)
			m_TitleWidget.SetText("#AR-V30_MapVoting_MenuChoiceTitle");
		if (m_SubTitleWidget)
			m_SubTitleWidget.SetText("#AR-V30_MapVoting_MenuChoiceSubTitle");

		m_Choice.GetOnPlayerVoteAdded().Remove(OnChoiceVoteAdded);
		m_Choice.GetOnPlayerVoteRemoved().Remove(OnChoiceVoteRemoved);
	};

	protected void OnClicked(SCR_ButtonBaseComponent component) {
		Vote();
	};

	protected void OnChoiceVoteAdded(V30_MapVoting_Choice choice, int playerId) {
		UpdateVoteCount();
	};

	protected void OnChoiceVoteRemoved(V30_MapVoting_Choice choice, int playerId) {
		UpdateVoteCount();
	};

	protected void UpdateVoteCount() {
		// TODO: Add player's vote extrapolation
		if (m_VoteCountWidget) m_VoteCountWidget.SetText(m_Choice.CountVotes().ToString());
	};

	protected void OnPlayerVoteChanged(V30_MapVoting_PlayerControllerComponent component, V30_MapVoting_ChoiceId choiceId, V30_MapVoting_ChoiceId oldChoiceId) {
		if (m_CheckWidget) {
			if (choiceId == m_ChoiceId) {
				m_CheckWidget.SetVisible(true);
			}
			else if (oldChoiceId == m_ChoiceId) {
				m_CheckWidget.SetVisible(false);
			};
		};
	};

	/*sealed*/ static V30_MapVoting_SimpleChoiceUIComponent GetSimpleInstance(notnull Widget w) {
		return V30_MapVoting_SimpleChoiceUIComponent.Cast(V30_MapVoting_ChoiceUIComponent.GetInstance(w));
	};

	override event bool OnFocus(Widget w, int x, int y) {
		PrintFormat("[V30_MapVoting_SimpleChoiceUIComponent] OnFocus: %1, %2, %3", w, x, y);
		return false;
	};
	override event bool OnFocusLost(Widget w, int x, int y) {
		PrintFormat("[V30_MapVoting_SimpleChoiceUIComponent] OnFocusLost: %1, %2, %3", w, x, y);
		return false;
	};
	override event bool OnMouseEnter(Widget w, int x, int y) {
		PrintFormat("[V30_MapVoting_SimpleChoiceUIComponent] OnMouseEnter: %1, %2, %3", w, x, y);
		return false;
	};
	override event bool OnMouseLeave(Widget w, Widget enterW, int x, int y) {
		PrintFormat("[V30_MapVoting_SimpleChoiceUIComponent] OnMouseLeave: %1, %2, %3, %4", w, enterW, x, y);
		return false;
	};
	override event bool OnController(Widget w, ControlID control, int value) {
		PrintFormat("[V30_MapVoting_SimpleChoiceUIComponent] OnController: %1, %2, %3", w, SCR_Enum.GetEnumName(ControlID, control), value);
		return false;
	};
};
