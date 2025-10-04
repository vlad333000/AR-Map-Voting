class V30_MapVoting_RemainingTimeComponentWidgetComponent : V30_MapVoting_WidgetComponent {
	override void Setup(notnull V30_MapVoting_VotingComponent votingComponent) {
		super.Setup(votingComponent);

		auto scenarioVotingComponent = V30_MapVoting_ScenarioVotingComponent.Cast(votingComponent);
		scenarioVotingComponent.GetOnTimeLimitStarted().Insert(OnTimeLimitStarted);
		scenarioVotingComponent.GetOnTimeLimitStopped().Insert(OnTimeLimitStopped);
		scenarioVotingComponent.GetOnFastTimeLimitStarted().Insert(OnFastTimeLimitStarted);
		scenarioVotingComponent.GetOnFastTimeLimitStopped().Insert(OnFastTimeLimitStopped);
		scenarioVotingComponent.GetOnTimeTicking().Insert(OnTimeTicking);

		GetRootWidget().SetVisible(false);

		if (scenarioVotingComponent.IsTimeLimitTicking())
			OnTimeLimitStarted(scenarioVotingComponent);
		if (scenarioVotingComponent.IsFastTimeLimitTicking())
			OnFastTimeLimitStarted(scenarioVotingComponent);
	};

	override event void HandlerDeattached(Widget w) {
		if (IsSetup()) {
			auto scenarioVotingComponent = GetScenarioVotingComponent();
			scenarioVotingComponent.GetOnTimeLimitStarted().Remove(OnTimeLimitStarted);
			scenarioVotingComponent.GetOnTimeLimitStopped().Remove(OnTimeLimitStopped);
			scenarioVotingComponent.GetOnFastTimeLimitStarted().Remove(OnFastTimeLimitStarted);
			scenarioVotingComponent.GetOnFastTimeLimitStopped().Remove(OnFastTimeLimitStopped);
			scenarioVotingComponent.GetOnTimeTicking().Remove(OnTimeTicking);
		};
		super.HandlerDeattached(w);
	};



	protected /*private*/ void OnTimeLimitStarted(notnull V30_MapVoting_ScenarioVotingComponent votingComponent) {
		StartTimer();
	};

	protected /*private*/ void OnTimeLimitStopped(notnull V30_MapVoting_ScenarioVotingComponent votingComponent) {
		if (!votingComponent.IsTimeTicking())
			StopTimer();
	};

	protected /*private*/ void OnFastTimeLimitStarted(notnull V30_MapVoting_ScenarioVotingComponent votingComponent) {
		StartTimer();
	};

	protected /*private*/ void OnFastTimeLimitStopped(notnull V30_MapVoting_ScenarioVotingComponent votingComponent) {
		if (!votingComponent.IsTimeTicking())
			StopTimer();
	};



	protected /*private*/ void StartTimer() {
		GetRootWidget().SetVisible(true);
	};

	protected /*private*/ void StopTimer() {
		GetRootWidget().SetVisible(false);
	};

	protected /*private*/ void OnTimeTicking(notnull V30_MapVoting_ScenarioVotingComponent votingComponent) {
		auto remainingTime = votingComponent.GetRemainingTime();

		auto time = remainingTime / 1000;
		auto minutes = (time / 60).ToString();
		auto seconds = (time % 60).ToString();
		if (minutes.Length() == 1)
			minutes = "0" + minutes;
		if (seconds.Length() == 1)
			seconds = "0" + seconds;

		auto widget = GetRootWidget();
		GetRootTextWidget().SetText(string.Format("%1:%2", minutes, seconds));
	};



	/*sealed*/ TextWidget GetRootTextWidget() {
		return TextWidget.Cast(GetRootWidget());
	};

	/*sealed*/ V30_MapVoting_ScenarioVotingComponent GetScenarioVotingComponent() {
		return V30_MapVoting_ScenarioVotingComponent.Cast(GetVotingComponent());
	};
};
