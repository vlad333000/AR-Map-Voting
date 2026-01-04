modded class SCR_GameOverScreenUIComponent {
    override void InitGameOverScreen(SCR_GameOverScreenUIContentData endScreenUIContent) {
        super.InitGameOverScreen(endScreenUIContent);

		auto sequenceHeader = SCR_MissionHeaderSequence.Cast(GetGame().GetMissionHeader());
		if (sequenceHeader)
			return;

		if (!endScreenUIContent.m_GameOverInfo.IsNextScenarioVotingEnabled())
			return;

		auto nextScenarioButton = m_wRoot.FindAnyWidget(m_sNextScenarioButtonName);
		if (!nextScenarioButton) {
			Debug.Error("Missing next scenario button");
			return;
		};

		nextScenarioButton.SetVisible(true);
		nextScenarioButton.SetEnabled(true);

		auto nextScenarioButtonComponent = SCR_InputButtonComponent.Cast(nextScenarioButton.FindHandler(SCR_InputButtonComponent));
		if (nextScenarioButtonComponent) {
			nextScenarioButtonComponent.m_OnActivated.Remove(NextScenario);
			nextScenarioButtonComponent.m_OnActivated.Insert(OpenNextScenarioVotingScreen);
			nextScenarioButtonComponent.SetAction("MenuSelect");
			nextScenarioButtonComponent.SetLabel("#AR-DeployScreen_ContinueButton");
		};

		auto debriefingButton = m_wRoot.FindAnyWidget(m_sDebriefingButtonName);
		if (debriefingButton) {
			debriefingButton.SetVisible(false);
			debriefingButton.SetEnabled(false);

			auto debriefingButtonComponent = SCR_InputButtonComponent.Cast(debriefingButton.FindHandler(SCR_InputButtonComponent));
			if (debriefingButtonComponent)
				debriefingButtonComponent.m_OnActivated.Remove(OpenDebriefingScreenMenu);
		};

		GetGame().GetCallqueue().CallLater(OpenNextScenarioVotingScreen, 5 * 1000, false);
    };

    event void OpenNextScenarioVotingScreen() {
        GetGame().GetCallqueue().Remove(OpenNextScenarioVotingScreen);
		GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.V30_MapVoting_GameOverScreenVotingMenu, 0, true);
    };
};
