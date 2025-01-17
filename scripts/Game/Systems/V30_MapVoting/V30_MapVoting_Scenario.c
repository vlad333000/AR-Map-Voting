class V30_MapVoting_Scenario {
	protected ResourceName m_ResourceName;
	
	protected string m_AddonsList;
	
	void V30_MapVoting_Scenario(ResourceName resourceName, string addonsList = string.Empty) {
		m_ResourceName = resourceName;
		m_AddonsList = addonsList;
	};
	
	void Play() {
		ArmaReforgerScripted.Cast(GetGame()).PlayGameConfig(m_ResourceName, m_AddonsList);
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

