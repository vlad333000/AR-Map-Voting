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
		
		string resourceStr = resourceName;
		auto addonsList = "";
		
		auto addons = new array<string>();
		PrintFormat("[V30][MapVoting] SCR_BaseGameMode::GetLoadedAddons(%1)", addons, level: LogLevel.DEBUG);
		GameProject.GetLoadedAddons(addons);
		PrintFormat("[V30][MapVoting]     %1 addon(s):", addons.Count(), level: LogLevel.DEBUG);
		foreach (auto i, auto addon : addons) {
			PrintFormat("[V30][MapVoting]         [%1] = \"%2\"", i, addon, level: LogLevel.DEBUG);
			if (i > 0) {
				addonsList += ",";
			};
			addonsList += addon;
		};
		
		PrintFormat("[V30][MapVoting] SCR_BaseGameMode::RequestScenarioChangeTransition(\"%1\", \"%2\")", resourceStr, addonsList, level: LogLevel.NORMAL);
		if (GameStateTransitions.RequestScenarioChangeTransition(resourceStr, addonsList)) {
			PrintFormat("[V30][MapVoting] Server transition request successfull!", level: LogLevel.NORMAL);
		}
		else {
			PrintFormat("[V30][MapVoting] Server transition request failed!", level: LogLevel.ERROR);
		};
	};
};