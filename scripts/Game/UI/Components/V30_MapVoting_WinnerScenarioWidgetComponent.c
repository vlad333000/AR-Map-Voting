class V30_MapVoting_WinnerScenarioWidgetComponent : V30_MapVoting_WidgetComponent {
	override void Setup(notnull V30_MapVoting_VotingComponent votingComponent) {
		super.Setup(votingComponent);

		if (!votingComponent.IsVoteEnded())
			votingComponent.GetOnVoteEnded().Insert(OnVoteEnded);
		else
			SetupPreviewData();
	};

	protected event void OnVoteEnded(notnull V30_MapVoting_VotingComponent votingComponent, V30_MapVoting_ChoiceId winnerId) {
		SetupPreviewData();
	};

	protected void SetupPreviewData() {
		auto root = GetRootWidget();
		auto titleWidget = TextWidget.Cast(root.FindAnyWidget("Title"));
		auto subtitleWidget = TextWidget.Cast(root.FindAnyWidget("SubTitle"));
		auto image = ImageWidget.Cast(root.FindAnyWidget("MapImage"));

		auto votingComponent = GetVotingComponent();
		auto preview = votingComponent.GetWinnerPreview();
		titleWidget.SetText(preview.GetTitle());
		subtitleWidget.SetText(preview.GetSubTitle());
		image.LoadImageTexture(0, preview.GetImage());

		int x, y;
		image.GetImageSize(0, x, y);
		image.SetSize(x, y);
	};

	override event void HandlerDeattached(Widget w) {
		auto votingComponent = GetVotingComponent();
		if (votingComponent)
			votingComponent.GetOnVoteEnded().Remove(OnVoteEnded);

		super.HandlerDeattached(w);
	};
};