class V30_MapVoting_Scenario {
	protected ResourceName m_ResourceName;
	
	protected string m_AddonsList;
	
	protected static bool m_IsInitialAddonsList = false;
	
	protected static string m_InitialAddonsList;
	
	void V30_MapVoting_Scenario(ResourceName resourceName, string addonsList = "$initial") {
		m_ResourceName = resourceName;
		m_AddonsList = addonsList;
	};
	
	void Play() {
		auto addonsList = m_AddonsList;
		addonsList.Replace("$initial", GetInitialAddonsList());
		addonsList.Replace("$current", GetCurrentAddonsList());
		ArmaReforgerScripted.Cast(GetGame()).PlayGameConfig(m_ResourceName, addonsList);
	};
	
	protected static string GetInitialAddonsList() {
		if (m_IsInitialAddonsList) return m_InitialAddonsList;
		m_InitialAddonsList = GetCurrentAddonsList();
		return m_InitialAddonsList;
	};
	
	protected static string GetCurrentAddonsList() {
		auto addons = new array<string>();
		GameProject.GetLoadedAddons(addons);
		
		string addonsList;
		foreach (int i, string addon : addons) {
			if (i > 0) addonsList += ",";
			addonsList += addon;
		};
		
		return addonsList;
	};
	
	ResourceName GetResourceName() {
		return m_ResourceName;
	};
	
	string GetAddonsList() {
		return m_AddonsList;
	};
	
	V30_MapVoting_PreviewData GetPreview() {
		SCR_MissionHeader header = SCR_ConfigHelperT<SCR_MissionHeader>.GetConfigObject(m_ResourceName);
		return V30_MapVoting_PreviewData.Create(header.m_sPreviewImage, header.m_sName, header.m_sGameMode);
	};
};

