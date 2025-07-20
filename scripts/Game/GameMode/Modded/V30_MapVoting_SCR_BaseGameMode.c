modded class SCR_BaseGameMode {
	[Attribute(desc: "Voting mission to load after game mode end", defvalue: "{B88CC33A14B71FDC}Missions/V30_MapVoting_Mission.conf", uiwidget: UIWidgets.ResourceNamePicker, params: "conf")];
	protected ResourceName m_V30_MapVoting_votingMissionResourceName;

	protected bool m_V30_MapVoting_overrideOnGameModeEnd = false;

	protected ResourceName V30_MapVoting_GetVotingMissionResourceName() {
		return m_V30_MapVoting_votingMissionResourceName;
	};

	override protected void OnGameModeEnd(SCR_GameModeEndData endData) {
		// TODO: Change to normal code when BIS allows to disable all actions in super.OnGameModeEnd
		m_V30_MapVoting_overrideOnGameModeEnd = true;
		super.OnGameModeEnd(endData);
		m_V30_MapVoting_overrideOnGameModeEnd = false;

		float reloadTime = Math.Max(0.0, GetAutoReloadDelay());
		GetGame().GetCallqueue().CallLater(V30_MapVoting_StartVotingMission, reloadTime * 1000.0, false);
	};

	override float GetAutoReloadDelay() {
		if (m_V30_MapVoting_overrideOnGameModeEnd) {
			return 0; // Force to call RestartSession in super.OnGameModeEnd
		};

		return super.GetAutoReloadDelay();
	};

	override protected void RestartSession() {
		if (m_V30_MapVoting_overrideOnGameModeEnd) {
			return; // Cancel RestartSession
		};

		super.RestartSession();
	};

	protected void V30_MapVoting_StartVotingMission() {
		if (!IsMaster()) {
			return;
		};

		if (TryShutdownServer()) {
			return;
		};

		auto resourceName = V30_MapVoting_GetVotingMissionResourceName();
		auto resource = SCR_ConfigHelperT<SCR_MissionHeader>.GetConfigObject(resourceName);
		if (!resource) {
			PrintFormat("[V30][MapVoting] Failed to load resource \"%1\" as mission!", resourceName, level: LogLevel.WARNING);
		};

		auto jsonConfigDeserializer = V30_Json_FileDeserializer("$profile:V30/MapVoting/config.json");
		auto jsonConfig = jsonConfigDeserializer.Deserialize();
		auto config = jsonConfig.AsObject();
		
		V30_MapVoting_Runner runner;
		auto runMethod = config.GetAt("runMethod");
		if (runMethod && !runMethod.IsNull()) {
			switch (runMethod.AsString().Get()) {
				case "RequestScenarioChangeTransition": {
					runner = new V30_MapVoting_Runner_RequestScenarioChangeTransition();
					break;
				};
				case "Podval": {
					auto url = config.GetAt("podvalUrl").AsString().Get();
					auto serverId = config.GetAt("podvalServerId").AsString().Get();
					runner = new V30_MapVoting_Runner_Podval(url, serverId);
					break;
				};
				default : {
					runner = new V30_MapVoting_Runner_RequestScenarioChangeTransition();
					break;
				};
			};
		}
		else {
			runner = new V30_MapVoting_Runner_RequestScenarioChangeTransition();
		};
		
		auto runData = new V30_MapVoting_RunData_Scenario(resourceName, "");
		runner.Run(runData);
	};
};
