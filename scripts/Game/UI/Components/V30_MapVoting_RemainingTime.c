class V30_MapVoting_RemainingTimeComponentWidgetComponent : V30_MapVoting_WidgetComponent {
	override void Setup(notnull V30_MapVoting_VotingWorldSystem votingWorldSystem) {
		super.Setup(votingWorldSystem);

		GetRootWidget().SetVisible(false);
	};

	override event void HandlerDeattached(Widget w) {
		super.HandlerDeattached(w);
	};



	protected /*private*/ void StartTimer() {
		GetRootWidget().SetVisible(true);
	};

	protected /*private*/ void StopTimer() {
		GetRootWidget().SetVisible(false);
	};

	protected /*private*/ void OnTimeTicking(notnull V30_MapVoting_VotingWorldSystem votingWorldSystem) {
		//auto remainingTime = votingWorldSystem.GetRemainingTime();

		//auto time = remainingTime / 1000;
		//auto minutes = (time / 60).ToString();
		//auto seconds = (time % 60).ToString();
		//if (minutes.Length() == 1)
		//	minutes = "0" + minutes;
		//if (seconds.Length() == 1)
		//	seconds = "0" + seconds;

		//auto widget = GetRootWidget();
		//GetRootTextWidget().SetText(string.Format("%1:%2", minutes, seconds));
	};



	/*sealed*/ TextWidget GetRootTextWidget() {
		return TextWidget.Cast(GetRootWidget());
	};
};
