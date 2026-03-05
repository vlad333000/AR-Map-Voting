class V30_MapVoting_WinnerScenarioWidgetComponent : V30_MapVoting_WidgetComponent {
	override void Setup(notnull V30_MapVoting_VotingWorldSystem votingWorldSystem) {
		super.Setup(votingWorldSystem);
	};

	protected event void OnVoteEnded(notnull V30_MapVoting_VotingWorldSystem votingComponent, int winnerId) {
		SetupPreviewData();
	};

	protected void SetupPreviewData() {
		auto root = GetRootWidget();
		auto titleWidget = TextWidget.Cast(root.FindAnyWidget("Title"));
		auto subtitleWidget = TextWidget.Cast(root.FindAnyWidget("SubTitle"));
		auto image = ImageWidget.Cast(root.FindAnyWidget("MapImage"));

		//auto votingComponent = GetVotingComponent();
		//auto preview = votingComponent.GetWinnerPreview();
		//titleWidget.SetText(preview.GetTitle());
		//subtitleWidget.SetText(preview.GetSubTitle());
		//image.LoadImageTexture(0, preview.GetImage());

		//int x, y;
		//image.GetImageSize(0, x, y);
		//image.SetSize(x, y);
	};

	override event void HandlerDeattached(Widget w) {
		super.HandlerDeattached(w);
	};
};