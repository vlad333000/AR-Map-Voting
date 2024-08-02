typedef int V30_MapVoting_ChoiceId;

V30_MapVoting_ChoiceId V30_MapVoting_NoChoice = -1;

enum V30_MapVoting_EVoteState {
	NOT_STARTED,
	STARTED,
	ENDED
}

[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class V30_MapVoting_GameModeComponentClass : SCR_BaseGameModeComponentClass {
};

class V30_MapVoting_GameModeComponent : SCR_BaseGameModeComponent {
	protected RplComponent m_RplComponent;
	
	protected V30_MapVoting_ChoiceId m_ChoiceId;
	
	protected ref map<V30_MapVoting_ChoiceId, ref V30_MapVoting_Choice> m_AvaiableChoices;
	
	protected ref map<int, V30_MapVoting_ChoiceId> m_PlayerChoices;
	
	[RplProp(onRplName: "OnVoteStateUpdated")]
	protected V30_MapVoting_EVoteState m_VoteState;
	
	protected ref ScriptInvoker m_OnVoteStarted;
	
	protected ref ScriptInvoker m_OnVoteEnded;
	
	protected ref ScriptInvoker m_OnChoiceLoaded;
	
	protected ref ScriptInvoker m_OnAllChoicesLoaded;
	
	[RplProp()]
	protected V30_MapVoting_ChoiceId m_WinnerId;
	
	protected ref V30_MapVoting_Scenario m_WinnerScenario;
	
	protected ref V30_MapVoting_PreviewData m_WinnerPreview;
	
	protected ref map<V30_MapVoting_ChoiceId, ref V30_MapVoting_PreviewData> m_Previews;
	
	protected bool m_IsAllChoicesLoaded;
	
	protected ref ScriptInvoker m_OnPlayerVoteChanged;
	
	[Attribute("true", desc: "Set seed for randomization on vote end")]
	protected bool m_SetRandomizationSeed;
	
	[Attribute("-1", desc: "Seed for randomization (-1 for using current time as seed)")]
	protected int m_RandomizationSeed;
	
	[Attribute("true", desc: "Save choice id on vote end for 'skipRepeats'")]
	protected bool m_SaveLastChoiceId;
	
	void V30_MapVoting_GameModeComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {
		m_ChoiceId = 0;
		m_AvaiableChoices = new map<V30_MapVoting_ChoiceId, ref V30_MapVoting_Choice>();
		m_PlayerChoices = new map<int, V30_MapVoting_ChoiceId>();
		m_VoteState = V30_MapVoting_EVoteState.NOT_STARTED;
		m_OnVoteStarted = new ScriptInvoker();
		m_OnVoteEnded = new ScriptInvoker();
		m_OnChoiceLoaded = new ScriptInvoker();
		m_OnAllChoicesLoaded = new ScriptInvoker();
		m_IsAllChoicesLoaded = false;
		m_OnPlayerVoteChanged = new ScriptInvoker();
	};
	
	override protected event void OnPostInit(IEntity owner) {
		super.OnPostInit(owner);
		
		if (SCR_Global.IsEditMode()) {
			return;
		};
		
		SetEventMask(owner, EntityEvent.INIT);
	};
	
	override protected event void EOnInit(IEntity owner) {
		super.EOnInit(owner);
		
        m_RplComponent = RplComponent.Cast(owner.FindComponent(RplComponent));
	};
	
	override void OnGameModeEnd(SCR_GameModeEndData data) {
		StartVote();
	};
	
	V30_MapVoting_PreviewData GetWinnerPreview() {
		return m_WinnerPreview;
	};
	
	protected void RunWinner() {
		if (m_RplComponent.Role() != RplRole.Authority) {
			return;
		};
		
		auto game = GetGame();
		auto winner = GetChoice(m_WinnerId);
		winner.Play();
		//auto resourceName = m_WinnerScenario.GetResourceName();
		//auto addonsList = m_WinnerScenario.GetAddonsList();
		//PrintFormat("Next scenario: resourceName = '%1', addonList = '%1'", resourceName, addonsList);
		//m_WinnerScenario.Play();
	};
	
	static V30_MapVoting_GameModeComponent GetInstance() {
		auto game = GetGame();
		if (!game) {
			return null;
		};
		
		auto gameMode = game.GetGameMode();
		if (!gameMode) {
			return null;
		};
		
		return V30_MapVoting_GameModeComponent.Cast(gameMode.FindComponent(V30_MapVoting_GameModeComponent));
	};
	
	override protected bool RplSave(ScriptBitWriter writer) {
		PrintFormat("%1.RplSave(%2)", this, writer);
		writer.WriteInt(m_VoteState);
		PrintFormat("	m_VoteState = %1", m_VoteState);
		if (IsVoteStarted() || IsVoteEnded()) {
			RplSave_Choices(writer);
			RplSave_PlayerChoices(writer);
		};
		if (IsVoteEnded()) {
			writer.WriteInt(m_WinnerId);
			PrintFormat("	m_WinnerId = %1", m_WinnerId);
			RplSave_Preview(writer, m_WinnerPreview);
		};
		return true;
	};
	
	protected void RplSave_Choices(ScriptBitWriter writer) {
		auto n = m_AvaiableChoices.Count();
		writer.WriteInt(n);
		PrintFormat("	m_AvaiableChoices[%1]", n);
		foreach (V30_MapVoting_ChoiceId choiceId, V30_MapVoting_Choice choice : m_AvaiableChoices) {
			RplSave_Choice(writer, choiceId, choice);
		};
	};
	
	protected void RplSave_PlayerChoices(ScriptBitWriter writer) {
		auto n = m_PlayerChoices.Count();
		writer.WriteInt(n);
		PrintFormat("	m_PlayerChoices[%1]", n);
		foreach (int playerId, V30_MapVoting_ChoiceId playerChoiceId : m_PlayerChoices) {
			writer.WriteInt(playerId);
			writer.WriteInt(playerChoiceId);
			PrintFormat("		[%1] = %2", playerId, playerChoiceId);
		};
	};
	
	protected void RplSave_Choice(ScriptBitWriter writer, V30_MapVoting_ChoiceId choiceId, V30_MapVoting_Choice choice) {
		writer.WriteInt(choiceId);
		PrintFormat("		[%1]:", choiceId);
		RplSave_Preview(writer, choice.GetPreview());
	};
	
	protected void RplSave_Preview(ScriptBitWriter writer, V30_MapVoting_PreviewData choicePreview) {
		writer.WriteResourceName(choicePreview.GetImage());
		PrintFormat("			.m_Image = %1", choicePreview.GetImage());
		writer.WriteString(choicePreview.GetTitle());
		PrintFormat("			.m_Title = %1", choicePreview.GetTitle());
		writer.WriteString(choicePreview.GetSubTitle());
		PrintFormat("			.m_SubTitle = %1", choicePreview.GetSubTitle());
	};
	
	override protected bool RplLoad(ScriptBitReader reader) {
		PrintFormat("%1.RplLoad(%2)", this, reader);
		reader.ReadInt(m_VoteState);
		PrintFormat("	m_VoteState = %1", m_VoteState);
		if (IsVoteStarted() || IsVoteEnded()) {
			RplLoad_Choices(reader);
			RplLoad_PlayerChoices(reader);
			OnVoteStarted();
		};
		if (IsVoteEnded()) {
			int winnerId;
			reader.ReadInt(winnerId);
			m_WinnerId = winnerId;
			PrintFormat("	m_WinnerId = %1", m_WinnerId);
			RplLoad_Preview(reader, m_WinnerPreview);
			OnVoteEnded(m_WinnerId);
		};
		
		return true;
	};
	
	protected void RplLoad_Choices(ScriptBitReader reader) {
		int n;
		reader.ReadInt(n);
		PrintFormat("	m_AvaiableChoices[%1]", n);
		for (int i = 0; i < n; i++) {
			V30_MapVoting_ChoiceId choiceId;
			V30_MapVoting_Choice choice;
			RplLoad_Choice(reader, choiceId, choice);
			m_AvaiableChoices.Set(choiceId, choice);
			OnChoiceLoaded(choiceId, choice);
		};
		OnAllChoicesLoaded(m_AvaiableChoices);
	};
	
	protected void RplLoad_PlayerChoices(ScriptBitReader reader) {
		int n;
		reader.ReadInt(n);
		PrintFormat("	m_PlayerChoices[%1]", n);
		for (int i = 0; i < n; i++) {
			int playerId;
			reader.ReadInt(playerId);
			int playerChoiceIdInt;
			reader.ReadInt(playerChoiceIdInt);
			V30_MapVoting_ChoiceId playerChoiceId = playerChoiceIdInt;
			m_PlayerChoices.Set(playerId, playerChoiceId);
			PrintFormat("		[%1] = %2", playerId, playerChoiceId);
			if (playerChoiceId != V30_MapVoting_NoChoice) OnPlayerVoteChanged(playerId, playerChoiceId, V30_MapVoting_NoChoice);
		};
		// Next RplDo_OnPlayerConnected will do this
		// auto playerId = GetGame().GetPlayerController().GetPlayerId();
		// m_PlayerChoices.Set(playerId, V30_MapVoting_NoChoice);
	};
	
	protected void RplLoad_Choice(ScriptBitReader reader, out V30_MapVoting_ChoiceId choiceId, out V30_MapVoting_Choice choice) {
		int choiceIdInt;
		reader.ReadInt(choiceIdInt);
		choiceId = choiceIdInt;
		V30_MapVoting_PreviewData choicePreviewData; 
		RplLoad_Preview(reader, choicePreviewData);
		choice = new V30_MapVoting_ChoiceProxy(choicePreviewData);
	};
	
	protected void RplLoad_Preview(ScriptBitReader reader, out V30_MapVoting_PreviewData choicePreviewData) {
		ResourceName resourceName;
		reader.ReadResourceName(resourceName);
		string title;
		reader.ReadString(title);
		string subTitle;
		reader.ReadString(subTitle);
		choicePreviewData = V30_MapVoting_PreviewData.Create(resourceName, title, subTitle);
	};
	
	protected event void OnVoteStateUpdated() {
		PrintFormat("%1.OnVoteStateUpdated()", this);
		PrintFormat("	m_VoteState = %1", m_VoteState);
		switch (m_VoteState) {
			case V30_MapVoting_EVoteState.NOT_STARTED: {
				// Tothing to do
				break;
			};
			case V30_MapVoting_EVoteState.STARTED: {
				OnVoteStarted();
				break;
			};
			case V30_MapVoting_EVoteState.ENDED: {
				OnVoteEnded(m_WinnerId);
				break;
			};
		};
	};
	
	
	
	protected void LoadConfig() {
		if (!FileIO.FileExists("$profile:V30/MapVoting/config.json")) {
			PrintFormat("%1.LoadConfig() Missing config file, it should be at: '$profile:V30/MapVoting/config.json'.", this, level: LogLevel.FATAL);
			debug;
			return;
		};
		
		auto jsonConfig = V30_Json_FileDeserializer("$profile:V30/MapVoting/config.json").Deserialize();
		if (!jsonConfig) {
			PrintFormat("%1.LoadConfig() Failed to deserialize config.", this, level: LogLevel.FATAL);
			debug;
			return;
		};
		
		auto jsonConfigObject = jsonConfig.AsObject();
		if (!jsonConfigObject) {
			PrintFormat("%1.LoadConfig() Root value is not an object - %2.", this, jsonConfig, level: LogLevel.FATAL);
			debug;
			return;
		};
		
		ParseConfig(jsonConfigObject);
		
		foreach (V30_MapVoting_ChoiceId id, V30_MapVoting_Choice choice : m_AvaiableChoices) {
			OnChoiceLoaded(id, choice);
		};
		OnAllChoicesLoaded(m_AvaiableChoices);
	};
	
	protected void ParseConfig(notnull V30_Json_object config) {
		// mode
		auto jsonMode = config.GetAt("mode");
		if (jsonMode.IsString()) {
			ParseConfigMode(jsonMode.AsString().Get(), config);
		}
		else if (jsonMode.IsNull()) {
			ParseConfigMode("whitelist", config);
		}
		else {
			PrintFormat("%1.ParseConfig(...) Entry 'mode' is not a string - %2.", this, jsonMode, level: LogLevel.ERROR);
		};
		
		// skipRepeats
		auto jsonSkipRepeats = config.GetAt("skipRepeats");
		if (jsonSkipRepeats.IsBool() || jsonSkipRepeats.IsInt()) {
			int skipRepeats;
			if (jsonSkipRepeats.IsBool()) {
				if (jsonSkipRepeats.AsBool().Get()) skipRepeats = 1;
				else skipRepeats = 0;
			}
			else {
				skipRepeats = jsonSkipRepeats.AsInt().Get();
			};
			
			if (skipRepeats == 2) {
				m_SaveLastChoiceId = true;
			}
			else if (skipRepeats == 1 && m_SaveLastChoiceId) {
				PrintFormat("'skipRepeats' is set, but 'Save Last Choice Id' attribute on game mode component is disabled.", level: LogLevel.WARNING);
			};
			
			if (skipRepeats > 0) {
				auto path = "$profile:V30/MapVoting/last.json";
				if (FileIO.FileExists(path)) {
					auto value = V30_Json_FileDeserializer(path).Deserialize();
					if (value.IsInt()) {
						V30_MapVoting_ChoiceId skipChoiceId = value.AsInt().Get();
						PrintFormat("Skipping choice with id %1 (%2) due to it was used in last vote.", skipChoiceId, GetChoice(skipChoiceId), level: LogLevel.WARNING);
						m_AvaiableChoices.Remove(skipChoiceId);
					}
					else {
						PrintFormat("'%1' contains bad data, it shoud be only integer.", path, level: LogLevel.WARNING);
						FileIO.DeleteFile(path);
					};
				};
			};
		}
		else if (!jsonSkipRepeats.IsNull()) {
			PrintFormat("%1.ParseConfig(...) Entry 'mode' is not a string - %2.", this, jsonSkipRepeats, level: LogLevel.ERROR);
		};
		
		// count
		auto jsonCount = config.GetAt("count");
		if (jsonCount.IsInt()) {
			auto preserveRandom = true;
			auto jsonPreserveRandom = config.GetAt("countPreserveRandom");
			if (!jsonPreserveRandom.IsNull()) {
				if (jsonPreserveRandom.IsBool()) {
					preserveRandom = jsonPreserveRandom.AsBool().Get();
				}
				else {
					PrintFormat("%1.ParseConfig(...) Entry 'countPreserveRandom' is not a bool - %2.", this, jsonPreserveRandom, level: LogLevel.ERROR);
				};
			};
			
			auto count = jsonCount.AsInt().Get();
			if (count > 1) {
				if (count < m_AvaiableChoices.Count()) {
					auto ids = new array<V30_MapVoting_ChoiceId>();
					foreach (V30_MapVoting_ChoiceId choiceId, V30_MapVoting_Choice choice : m_AvaiableChoices)
						if (!preserveRandom || !V30_MapVoting_ChoiceRandom.Cast(choice))
							ids.Insert(choiceId);
					
					PrintFormat("	Removing %1 choices...", m_AvaiableChoices.Count() - count);
					while (m_AvaiableChoices.Count() > count) {
						auto i = ids.GetRandomIndex();
						auto choiceId = ids.Get(i);
						ids.Remove(i);
						
						PrintFormat("		Removing: [%1] = %2", choiceId, GetChoice(choiceId));
						m_AvaiableChoices.Remove(choiceId);
					};
				};
			}
			else {
				PrintFormat("%1.ParseConfig(...) Entry 'count' less then 2 - %2.", this, count, level: LogLevel.ERROR);
			};
		}
		else if (!jsonCount.IsNull()) {
			PrintFormat("%1.ParseConfig(...) Entry 'count' is not a number - %2.", this, jsonCount, level: LogLevel.ERROR);
		};
	};
	
	protected void ParseConfigMode(string mode, notnull V30_Json_object config) {
		if (mode == "whitelist") {
			// list
			auto jsonList = config.GetAt("list");
			if (!jsonList) {
				PrintFormat("%1.ParseConfig(...) Missing 'list' entry.", this, level: LogLevel.ERROR);
				return;
			}
			else if (!jsonList.IsArray()) {
				PrintFormat("%1.ParseConfig(...) Entry 'list' is not a array - %2.", this, jsonList, level: LogLevel.ERROR);
				return;
			};
			
			ParseConfigWhitelist(jsonList.AsArray().Get(), config);
		}
		else {
			PrintFormat("%1.ParseConfigMode(...) Unknown mode '%2'!", this, mode, level: LogLevel.ERROR);
		};
	};
	
	protected void ParseConfigWhitelist(notnull array<ref V30_Json_Value> list, notnull V30_Json_object config) {
		foreach (int i, V30_Json_Value v : list) {
			m_AvaiableChoices.Insert(i, ParseConfigListEntry(i, v, "whitelist", config));
		};
	};
	
	protected V30_MapVoting_Choice ParseConfigListEntry(V30_MapVoting_ChoiceId id, notnull V30_Json_Value entry, string mode, notnull V30_Json_object config) {
		if (entry.IsString()) {
			auto path = entry.AsString().Get();
			if (path == "#random") {
				return new V30_MapVoting_ChoiceRandom();
			}
			else {
				return new V30_MapVoting_ChoiceMission(path);
			};
		}
		else if (entry.IsObject()) {
			return null;
		}
		else {
			PrintFormat("%1.ParseConfigMode(...) List entry at %2 is not a string nor object - %3.", this, id, entry, level: LogLevel.ERROR);
			return null;
		};
	};
	
	protected event void OnChoiceLoaded(V30_MapVoting_ChoiceId choiceId, notnull V30_MapVoting_Choice choice) {
		PrintFormat("%1.OnChoiceLoaded(%2, %3)", this, choiceId, choice);
		m_OnChoiceLoaded.Invoke(this, choiceId, choice);
	};
	
	ScriptInvoker GetOnChoiceLoaded() {
		return m_OnChoiceLoaded;
	};
	
	bool IsAllChoicesLoaded() {
		return m_IsAllChoicesLoaded;
	};
	
	protected event void OnAllChoicesLoaded(notnull map<V30_MapVoting_ChoiceId, ref V30_MapVoting_Choice> choices) {
		PrintFormat("%1.OnChoiceLoaded(%2[%3])", this, choices, choices.Count());
		m_IsAllChoicesLoaded = true;
		m_OnAllChoicesLoaded.Invoke(this, choices);
	};
	
	ScriptInvoker GetOnAllChoicesLoaded() {
		return m_OnAllChoicesLoaded;
	};
	
	bool IsValidChoiceId(V30_MapVoting_ChoiceId choiceId) {
		return choiceId != V30_MapVoting_NoChoice && m_AvaiableChoices.Contains(choiceId);
	};
	
	V30_MapVoting_Choice GetChoice(V30_MapVoting_ChoiceId choiceId) {
		if (choiceId == V30_MapVoting_NoChoice) {
			PrintFormat("'No-choice' id is passed.", level: LogLevel.ERROR);
			return null;
		};
		
		V30_MapVoting_Choice choice;
		auto found = m_AvaiableChoices.Find(choiceId, choice);
		
		if (!found) {
			PrintFormat("No choice with id %1 found.", choiceId, level: LogLevel.ERROR);
			return null;
		};
		
		return choice;
	};
	
	map<V30_MapVoting_ChoiceId, ref V30_MapVoting_Choice> GetAllChoices() {
		return m_AvaiableChoices;
	};
	
	
	
	protected void SendAllChoices() {
		foreach (V30_MapVoting_ChoiceId choiceId, V30_MapVoting_Choice choice : m_AvaiableChoices) {
			Rpc(RpcDo_SendChoice, choiceId, choice.GetPreview());
		};
		Rpc(RpcDo_SendAllChoices, m_AvaiableChoices.Count());
	};
	
	[RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
	protected void RpcDo_SendChoice(V30_MapVoting_ChoiceId choiceId, V30_MapVoting_PreviewData choicePreview) {
		auto choice = new V30_MapVoting_ChoiceProxy(choicePreview);
		m_AvaiableChoices.Insert(choiceId, choice);
		OnChoiceLoaded(choiceId, choice);
	};
	
	[RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
	protected void RpcDo_SendAllChoices(int count) {
		m_IsAllChoicesLoaded = true;
		OnAllChoicesLoaded(m_AvaiableChoices);
	};
	
	
	
	V30_MapVoting_EVoteState GetVoteState() {
		return m_VoteState;
	};
	
	protected void StartVote() {
		if (m_RplComponent.Role() != RplRole.Authority) {
			return;
		};
		
		if (m_VoteState != V30_MapVoting_EVoteState.NOT_STARTED) {
			return;
		};
		
		LoadConfig();
		
		SendAllChoices();
		
		m_VoteState = V30_MapVoting_EVoteState.STARTED;
		
		Replication.BumpMe();
		
		OnVoteStarted();
	};
	
	bool IsVoteStarted() {
		return m_VoteState == V30_MapVoting_EVoteState.STARTED;
	};
	
	protected event void OnVoteStarted() {
		PrintFormat("%1.OnVoteStarted()", this);
		m_OnVoteStarted.Invoke(this);
	};
	
	ScriptInvoker GetOnVoteStarted() {
		return m_OnVoteStarted;
	};
	
	protected void EndVote() {
		PrintFormat("%1.EndVote()", this);
		
		if (m_RplComponent.Role() != RplRole.Authority) {
			debug;
			return;
		};
		
		if (m_SetRandomizationSeed) {
			PrintFormat("	Randomize with seed %1", Math.Randomize(m_RandomizationSeed));
		};
		
		foreach (V30_MapVoting_Choice choice : m_AvaiableChoices) {
			choice.OnVoteEnd();
		};
		
		SelectWinner();
		
		if (m_SaveLastChoiceId) {
			auto choiceId = m_WinnerId;
			while (V30_MapVoting_ChoiceWrapper.Cast(GetChoice(choiceId))) choiceId = V30_MapVoting_ChoiceWrapper.Cast(GetChoice(choiceId)).GetChoiceId();
			V30_Json_FileSerializer("$profile:V30/MapVoting/last.json").Serialize(V30_Json_int(choiceId));
		};
		
		Rpc(RpcDo_OnVoteEnded, m_WinnerId, m_WinnerPreview);
		OnVoteEnded(m_WinnerId);
	};
	
	protected void SelectWinner() {
		PrintFormat("%1.SelectWinner()", this);
		
		auto candidates = GetTopChoices();
		PrintFormat("	Candidates:");
		foreach (V30_MapVoting_ChoiceId choiceId : candidates) PrintFormat("		[%1] = %2", choiceId, m_AvaiableChoices.Get(choiceId));
		
		m_WinnerId = candidates.GetRandomElement();
		PrintFormat("	Winner: [%1] = %2", m_WinnerId, m_AvaiableChoices.Get(m_WinnerId));
		GetChoice(m_WinnerId).OnWinner();
		
		m_WinnerPreview = GetChoice(m_WinnerId).GetPreview();
		m_VoteState = V30_MapVoting_EVoteState.ENDED;
	};
	
	bool IsVoteEnded() {
		return m_VoteState == V30_MapVoting_EVoteState.ENDED;
	};
	
	[RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
	protected event void RpcDo_OnVoteEnded(V30_MapVoting_ChoiceId winnerId, V30_MapVoting_PreviewData winnerPreiview) {
		m_WinnerId = winnerId;
		m_WinnerPreview = winnerPreiview;
		OnVoteEnded(m_WinnerId);
	};
	
	protected event void OnVoteEnded(V30_MapVoting_ChoiceId winnerId) {
		PrintFormat("%1.OnVoteEnded(%2)", this, winnerId);
		m_OnVoteEnded.Invoke(this, winnerId);
	};
	
	ScriptInvoker GetOnVoteEnded() {
		return m_OnVoteEnded;
	};
	
	array<V30_MapVoting_ChoiceId> GetTopChoices() {
		array<V30_MapVoting_ChoiceId> choicesIds = {};
		
		int maxVotes = 0;
		foreach (V30_MapVoting_ChoiceId choiceId, V30_MapVoting_Choice choice : m_AvaiableChoices) {
			int votes = choice.CountVotes();
			if (votes == maxVotes) {
				choicesIds.Insert(choiceId);
			}
			else if (votes > maxVotes) {
				choicesIds.Clear();
				choicesIds.Insert(choiceId);
				maxVotes = votes;
			};
		};
		
		return choicesIds;
	};
	
	V30_MapVoting_ChoiceId GetWinnerChoice() {
		if (!IsVoteEnded()) {
			PrintFormat("Vote is not ended yet.", level: LogLevel.ERROR);
			return V30_MapVoting_NoChoice;
		};
		
		return m_WinnerId;
	};
	
	
	
	void SetPlayerChoice(int playerId, V30_MapVoting_ChoiceId choiceId) {
		if (m_RplComponent.Role() != RplRole.Authority) {
			PrintFormat("%1.SetPlayerChoice: Called on non-authority.", level: LogLevel.ERROR);
			return;
		};
		
		if (!m_AvaiableChoices.Contains(choiceId)) {
			PrintFormat("%1: No choice with id %1 found.", this, choiceId, level: LogLevel.ERROR);
			return;
		};
		
		Rpc(RplDo_SetPlayerChoice, playerId, choiceId);
		RplDo_SetPlayerChoice(playerId, choiceId);
	};
	
	[RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
	protected void RplDo_SetPlayerChoice(int playerId, V30_MapVoting_ChoiceId choiceId) {
		auto oldChoiceId = m_PlayerChoices.Get(playerId);
		m_PlayerChoices.Set(playerId, choiceId);
		OnPlayerVoteChanged(playerId, choiceId, oldChoiceId);
	};
	
	V30_MapVoting_ChoiceId GetPlayerChoice(int playerId) {
		return m_PlayerChoices.Get(playerId);
	};
	
	bool IsPlayerVoted(int playerId) {
		return m_PlayerChoices.Get(playerId) != V30_MapVoting_NoChoice;
	};
	
	protected event void OnPlayerVoteChanged(int playerId, V30_MapVoting_ChoiceId choiceId, V30_MapVoting_ChoiceId oldChoiceId) {
		PrintFormat("%1.OnPlayerVoteChanged(%2, %3, %4)", this, playerId, choiceId, oldChoiceId);
		if (oldChoiceId != V30_MapVoting_NoChoice) m_AvaiableChoices.Get(oldChoiceId).RemovePlayerVote(playerId);
		if (choiceId != V30_MapVoting_NoChoice) m_AvaiableChoices.Get(choiceId).AddPlayerVote(playerId);
		m_OnPlayerVoteChanged.Invoke(this, playerId, choiceId, oldChoiceId);
	};
	
	ScriptInvoker GetOnPlayerVoteChanged() {
		return m_OnPlayerVoteChanged;
	};
	
	override protected void OnPlayerConnected(int playerId) {
		super.OnPlayerConnected(playerId);
		Rpc(RplDo_OnPlayerConnected, playerId);
		RplDo_OnPlayerConnected(playerId);
	};
	
	[RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
	protected void RplDo_OnPlayerConnected(int playerId) {
		PrintFormat("%1.OnPlayerConnected(%2)", this, playerId);
		m_PlayerChoices.Set(playerId, V30_MapVoting_NoChoice);
	};
	
	override protected void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout) {
		super.OnPlayerDisconnected(playerId, cause, timeout);
		Rpc(RplDo_OnPlayerDisconnected, playerId, cause, timeout);
		RplDo_OnPlayerDisconnected(playerId, cause, timeout);
	};
	
	[RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
	protected void RplDo_OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout) {
		PrintFormat("%1.OnPlayerDisconnected(%2, %3, %4)", this, playerId, cause, timeout);
		auto playerVote = GetPlayerChoice(playerId);
		if (playerVote != V30_MapVoting_NoChoice) OnPlayerVoteChanged(playerId, V30_MapVoting_NoChoice, playerVote);
		m_PlayerChoices.Remove(playerId);
	};
};