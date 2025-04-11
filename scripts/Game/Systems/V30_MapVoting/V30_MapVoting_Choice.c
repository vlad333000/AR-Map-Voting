class V30_MapVoting_Choice {
	protected ref ScriptInvoker m_OnPlayerVoteAdded;
	
	protected ref ScriptInvoker m_OnPlayerVoteRemoved;
	
	protected ref set<int> m_VotedPlayers;
	
	void V30_MapVoting_Choice() {
		m_OnPlayerVoteAdded = new ScriptInvoker();
		m_OnPlayerVoteRemoved = new ScriptInvoker();
		m_VotedPlayers = new set<int>();
	};
	
	void AddPlayerVote(int playerId) {
		if (IsPlayerVoted(playerId)) return;
		m_VotedPlayers.Insert(playerId);
		OnPlayerVoteAdded(playerId);
	};
	
	void RemovePlayerVote(int playerId) {
		if (!IsPlayerVoted(playerId)) return;
		m_VotedPlayers.RemoveItem(playerId);
		OnPlayerVoteRemoved(playerId);
	};
	
	bool IsPlayerVoted(int playerId) {
		return m_VotedPlayers.Contains(playerId);
	};
	
	int CountVotes() {
		return m_VotedPlayers.Count();
	};
	
	protected event void OnPlayerVoteAdded(int playerId) {
		PrintFormat("%1.OnPlayerVoteAdded(%2)", this, playerId);
		m_OnPlayerVoteAdded.Invoke(this, playerId);
	};
	
	ScriptInvoker GetOnPlayerVoteAdded() {
		return m_OnPlayerVoteAdded;
	};
	
	protected event void OnPlayerVoteRemoved(int playerId) {
		PrintFormat("%1.OnPlayerVoteRemoved(%2)", this, playerId);
		m_OnPlayerVoteRemoved.Invoke(this, playerId);
	};
	
	ScriptInvoker GetOnPlayerVoteRemoved() {
		return m_OnPlayerVoteRemoved;
	};
	
	event void OnVoteEnd();
	
	event void OnWinner();
	
	void Play();
	
	V30_MapVoting_PreviewData GetPreviewData();
	
	static V30_MapVoting_Choice FromConfig(notnull V30_Json_object config) {
		// TODO
	};
};

class V30_MapVoting_ChoiceProxy : V30_MapVoting_Choice {
	protected ref V30_MapVoting_PreviewData m_Preview;
	
	void V30_MapVoting_ChoiceProxy(V30_MapVoting_PreviewData preview) {
		m_Preview = preview;
	};
	
	override V30_MapVoting_PreviewData GetPreviewData() {
		return m_Preview;
	};
};

class V30_MapVoting_ChoiceResource : V30_MapVoting_Choice {
	protected ResourceName m_ResourceName;
	
	protected string m_AddonsList;
	
	protected ref Resource m_Resource;
	
	protected int m_Repeats;
	
	protected static const int s_MaxRepeats = 10;
	
	void V30_MapVoting_ChoiceResource(ResourceName resourceName, string addonsList = "") {
		m_ResourceName = resourceName;
		m_AddonsList = addonsList;
		m_Repeats = 0;
		
		m_Resource = Resource.Load(m_ResourceName);
		if (!m_Resource.IsValid()) PrintFormat("%1: invalid resource '%2'.", this, m_ResourceName, level: LogLevel.ERROR);
	};
	
	ResourceName GetResourceName() {
		return m_ResourceName;
	};
	
	string GetAddonsList() {
		return m_AddonsList;
	};
	
	Resource GetResource() {
		return m_Resource;
	};
	
	override V30_MapVoting_PreviewData GetPreviewData() {
		return V30_MapVoting_PreviewData.Create("{65B6F7199650B53A}UI/Textures/V30_MapVoting_Choice_RandomImage.edds", m_ResourceName, m_AddonsList);
	};
	
	protected static bool m_IsInitialAddonsList = false;
	
	protected static string m_InitialAddonsList;
	
	override void Play() {
		PrintFormat("%1.Play()", this);
		
		auto initialAddonsList = GetInitialAddonsList();
		PrintFormat("	addonsList.$initial = '%1'", GetInitialAddonsList());
		
		auto currentAddonsList = GetCurrentAddonsList();
		PrintFormat("	addonsList.$current = '%1'", GetCurrentAddonsList());
		
		auto addonsList = m_AddonsList;
		PrintFormat("	addonsList = '%1'", addonsList);
		addonsList.Replace("$initial", initialAddonsList);
		addonsList.Replace("$current", currentAddonsList);
		
		PrintFormat("	RequestScenarioChangeTransition('%1', '%2')", m_ResourceName, addonsList);
		bool r = GameStateTransitions.RequestScenarioChangeTransition(m_ResourceName, addonsList); // GetGame().PlayGameConfig(m_ResourceName, addonsList);
		if (!r) {
			m_Repeats += 1;
			if (m_Repeats >= s_MaxRepeats) {
				PrintFormat("	Failed to RequestScenarioChangeTransition in %1 time(s), shutdown the server.", s_MaxRepeats, level: LogLevel.FATAL);
				GetGame().RequestClose();
				//GameStateTransitions.RequestGameTerminateTransition();
				return;
			};
			
			PrintFormat("	Failed to RequestScenarioChangeTransition in %1 time(s), repeat in 1000 ms", m_Repeats, level: LogLevel.ERROR);
			GetGame().GetCallqueue().CallLater(Play, delay: 1.0 * 1000);
		};
	};
	
	static string GetInitialAddonsList() {
		if (m_IsInitialAddonsList) return m_InitialAddonsList;
		
		auto addons = new array<string>();
		GameProject.GetLoadedAddons(addons);
		
		m_InitialAddonsList = "";
		foreach (int i, string addon : addons) {
			if (i > 0) m_InitialAddonsList += ",";
			m_InitialAddonsList += addon;
		};
		m_IsInitialAddonsList = true;
		return m_InitialAddonsList;
	};
	
	static string GetCurrentAddonsList() {
		auto addons = new array<string>();
		GameProject.GetLoadedAddons(addons);
		
		string addonsList;
		foreach (int i, string addon : addons) {
			if (i > 0) addonsList += ",";
			addonsList += addon;
		};
		
		return addonsList;
	};
};

class V30_MapVoting_ChoiceMission : V30_MapVoting_ChoiceResource {
	override V30_MapVoting_PreviewData GetPreviewData() {
		SCR_MissionHeader header = SCR_ConfigHelperT<SCR_MissionHeader>.GetConfigObject(m_ResourceName);
		if (!header) {
			PrintFormat("%1: failed to get config for '%2'", this, m_ResourceName);
			return super.GetPreviewData();
		};
		
		return V30_MapVoting_PreviewData.Create(header.m_sPreviewImage, header.m_sName, header.m_sGameMode);
	};
};

class V30_MapVoting_ChoiceWrapper : V30_MapVoting_Choice {
	V30_MapVoting_ChoiceId GetChoiceId();
	
	V30_MapVoting_Choice GetChoice() {
		return V30_MapVoting_GameModeComponent.GetInstance().GetChoice(GetChoiceId());
	};
	
	override V30_MapVoting_PreviewData GetPreviewData() {
		return GetChoice().GetPreviewData();
	};
};

class V30_MapVoting_ChoiceRandom : V30_MapVoting_ChoiceWrapper {
	protected V30_MapVoting_ChoiceId m_SelectedChoiceId;
	
	void V30_MapVoting_ChoiceRandom() {
		m_SelectedChoiceId = V30_MapVoting_NoChoice;
	};
	
	override V30_MapVoting_ChoiceId GetChoiceId() {
		return m_SelectedChoiceId;
	};
	
	bool IsChoiceDetermined() {
		return m_SelectedChoiceId != V30_MapVoting_NoChoice;
	};
	
	override void Play() {
		GetChoice().Play();
	};
	
	override void OnVoteEnd() {
		PrintFormat("%1.OnVoteEnd()", this);
		
		super.OnVoteEnd();
		
		auto ids = new array<V30_MapVoting_ChoiceId>();
		foreach (V30_MapVoting_ChoiceId choiceId, V30_MapVoting_Choice choice : V30_MapVoting_GameModeComponent.GetInstance().GetAllChoices()) if (!V30_MapVoting_ChoiceWrapper.Cast(choice)) ids.Insert(choiceId);
		PrintFormat("	Candidates:");
		foreach (V30_MapVoting_ChoiceId choiceId : ids) PrintFormat("		[%1] = %2", choiceId, V30_MapVoting_GameModeComponent.GetInstance().GetChoice(choiceId));
		
		m_SelectedChoiceId = ids.GetRandomElement();
		PrintFormat("	Selected: [%1] = %2",  m_SelectedChoiceId, V30_MapVoting_GameModeComponent.GetInstance().GetChoice(m_SelectedChoiceId));
	};
	
	override V30_MapVoting_PreviewData GetPreviewData() {
		if (IsChoiceDetermined()) {
			return GetChoice().GetPreviewData();
		}
		else {
			return V30_MapVoting_PreviewData.Create("{65B6F7199650B53A}UI/Textures/V30_MapVoting_Choice_RandomImage.edds", "#AR-V30_MapVoting_RandomChoiceTitle", "#AR-V30_MapVoting_RandomChoiceSubTitle");
		};
	};
};

class V30_MapVoting_ChoiceRandomList : V30_MapVoting_ChoiceWrapper {
	// TODO
};

class V30_MapVoting_ChoiceRepeat : V30_MapVoting_ChoiceWrapper {
	// TODO
};
