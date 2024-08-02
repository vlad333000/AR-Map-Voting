modded class SCR_BaseGameMode {
	override protected void RestartSession() {
		if (!IsMaster()) return;
		
		auto addons = new array<string>();
		GameProject.GetLoadedAddons(addons);
		
		string addonsList;
		foreach (int i, string addon : addons) {
			if (i > 0) addonsList += ",";
			addonsList += addon;
		};
		
		GameStateTransitions.RequestScenarioChangeTransition("{B88CC33A14B71FDC}Missions/V30_MapVoting_Mission.conf", addonsList);
	};
};