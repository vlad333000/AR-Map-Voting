class V30_MapVoting_VoteComponent : ScriptedWidgetComponent {
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

	protected V30_MapVoting_ChoiceId m_ChoiceId;

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

	void SetupChoice(V30_MapVoting_ChoiceId choiceId, notnull V30_MapVoting_Choice choice) {
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

	V30_MapVoting_Choice GetChoice() {
		return m_Choice;
	};

	protected void OnClicked(SCR_ButtonBaseComponent component) {
		V30_MapVoting_PlayerControllerComponent.GetLocalInstance().SetVote(m_ChoiceId);
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
};
