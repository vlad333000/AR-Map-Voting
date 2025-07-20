typedef int V30_MapVoting_ChoiceId;

V30_MapVoting_ChoiceId V30_MapVoting_NoChoice = -1;

enum V30_MapVoting_EVoteState {
	NOT_STARTED,
	STARTED,
	ENDED
}

enum V30_MapVoting_EGiveVoteAbility {
	ALL,
	ADMINISTRATORS,
	SESSION_ADMINISTRATORS,
	GAME_MASTERS,
	NONE
};


[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
// TODO: Move logic to GameSystem

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

	protected ref ScriptInvoker m_OnPlayerVoteAbilityChanged;

	protected ref ScriptInvoker m_OnConfigLoaded;

	[RplProp()]
	protected V30_MapVoting_ChoiceId m_WinnerId;

	protected ref V30_MapVoting_Scenario m_WinnerScenario;

	protected ref V30_MapVoting_PreviewData m_WinnerPreview;

	protected ref map<V30_MapVoting_ChoiceId, ref V30_MapVoting_PreviewData> m_Previews;

	protected bool m_IsAllChoicesLoaded;

	protected ref ScriptInvoker m_OnPlayerVoteChanged;

	[Attribute("true", desc: "Set seed for randomization on vote end", category: "Vote")]
	protected bool m_SetRandomizationSeed;

	[Attribute("-1", desc: "Seed for randomization (-1 for using current time as seed)", uiwidget: "editbox", category: "Vote")]
	protected int m_RandomizationSeed;

	[Attribute("true", desc: "Allows to remember previous choices for skipRepeats config parameter.", category: "Vote")]
	protected bool m_RememberPreviousChoices;

	[Attribute("-1", desc: "How many previous choices will be remembered, -1 - for equal to skipRepeats config parameter.", params: "-1 inf", category: "Vote")]
	protected int m_RememberPreviousChoicesCount;

	[Attribute("true", desc: "Allows player to remove his vote.")]
	protected bool m_AllowsVoteRemove;

	[Attribute("0", desc: "Who can vote (NONE for full scripted-base).", uiwidget: UIWidgets.ComboBox, category: "Vote", enums: ParamEnumArray.FromEnum(V30_MapVoting_EGiveVoteAbility))]
	protected V30_MapVoting_EGiveVoteAbility m_GiveVoteAbility;

	protected ref set<int> m_Voters;

	protected int m_VotersCount;

	protected ref V30_MapVoting_Runner m_Runner;

	void V30_MapVoting_GameModeComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {
		m_ChoiceId = 0;
		m_AvaiableChoices = new map<V30_MapVoting_ChoiceId, ref V30_MapVoting_Choice>();
		m_PlayerChoices = new map<int, V30_MapVoting_ChoiceId>();
		m_VoteState = V30_MapVoting_EVoteState.NOT_STARTED;
		m_OnVoteStarted = new ScriptInvoker();
		m_OnVoteEnded = new ScriptInvoker();
		m_OnChoiceLoaded = new ScriptInvoker();
		m_OnAllChoicesLoaded = new ScriptInvoker();
		m_OnConfigLoaded = new ScriptInvoker();
		m_IsAllChoicesLoaded = false;
		m_OnPlayerVoteChanged = new ScriptInvoker();
		m_GiveVoteAbility = V30_MapVoting_EGiveVoteAbility.ALL;
		m_Voters = new set<int>();
		m_OnPlayerVoteAbilityChanged = new ScriptInvoker();
		m_VotersCount = 0;
	};

	bool IsAllowsVoteRemove() {
		return m_AllowsVoteRemove;
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

	void RunWinner() {
		if (m_RplComponent.Role() != RplRole.Authority) {
			return;
		};

		auto runner = GetRunner();
		auto winnerId = GetWinnerChoice();
		auto winner = GetChoice(winnerId);
		auto winnerRunData = winner.GetRunData();
		runner.Run(winnerRunData);
	};

	V30_MapVoting_Runner GetRunner() {
		return m_Runner;
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
		//PrintFormat("%1.RplSave(%2)", this, writer);
		writer.WriteInt(m_VoteState);
		//PrintFormat("	m_VoteState = %1", m_VoteState);
		if (IsVoteStarted() || IsVoteEnded()) {
			RplSave_Choices(writer);
			RplSave_PlayerChoices(writer);
		};
		if (IsVoteEnded()) {
			writer.WriteInt(m_WinnerId);
			//PrintFormat("	m_WinnerId = %1", m_WinnerId);
			RplSave_Preview(writer, m_WinnerPreview);
		};
		if (!IsVoteEnded()) {
			writer.WriteInt(m_VotersCount);
		};
		return true;
	};

	protected void RplSave_Choices(ScriptBitWriter writer) {
		auto n = m_AvaiableChoices.Count();
		writer.WriteInt(n);
		//PrintFormat("	m_AvaiableChoices[%1]", n);
		foreach (V30_MapVoting_ChoiceId choiceId, V30_MapVoting_Choice choice : m_AvaiableChoices) {
			RplSave_Choice(writer, choiceId, choice);
		};
	};

	protected void RplSave_PlayerChoices(ScriptBitWriter writer) {
		auto n = m_PlayerChoices.Count();
		writer.WriteInt(n);
		//PrintFormat("	m_PlayerChoices[%1]", n);
		foreach (int playerId, V30_MapVoting_ChoiceId playerChoiceId : m_PlayerChoices) {
			writer.WriteInt(playerId);
			writer.WriteInt(playerChoiceId);
			//PrintFormat("		[%1] = %2", playerId, playerChoiceId);
		};
	};

	protected void RplSave_Choice(ScriptBitWriter writer, V30_MapVoting_ChoiceId choiceId, V30_MapVoting_Choice choice) {
		writer.WriteInt(choiceId);
		//PrintFormat("		[%1]:", choiceId);
		RplSave_Preview(writer, choice.GetPreviewData());
	};

	protected void RplSave_Preview(ScriptBitWriter writer, V30_MapVoting_PreviewData choicePreview) {
		writer.WriteResourceName(choicePreview.GetImage());
		//PrintFormat("			.m_Image = %1", choicePreview.GetImage());
		writer.WriteString(choicePreview.GetTitle());
		//PrintFormat("			.m_Title = %1", choicePreview.GetTitle());
		writer.WriteString(choicePreview.GetSubTitle());
		//PrintFormat("			.m_SubTitle = %1", choicePreview.GetSubTitle());
	};

	override protected bool RplLoad(ScriptBitReader reader) {
		PrintFormat("%1.RplLoad(%2)", this, reader);
		reader.ReadInt(m_VoteState);
		PrintFormat("	m_VoteState = %1", m_VoteState);
		if (IsVoteStarted() || IsVoteEnded()) {
			RplLoad_Choices(reader);
			RplLoad_PlayerChoices(reader);

			foreach (V30_MapVoting_ChoiceId choiceId, V30_MapVoting_Choice choice : m_AvaiableChoices) {
				OnChoiceLoaded(choiceId, choice);
			};
			OnAllChoicesLoaded(m_AvaiableChoices);

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
		if (!IsVoteEnded()) {
			int votersCount;
			reader.ReadInt(votersCount);
			m_VotersCount = votersCount;
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
		};
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
		PrintFormat("		[%1] = %2", choiceId, choice);
		PrintFormat("			.m_Title = '%1'", choicePreviewData.GetTitle());
		PrintFormat("			.m_SubTitle = '%1'", choicePreviewData.GetSubTitle());
		PrintFormat("			.m_Image = '%1'", choicePreviewData.GetImage());
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
		PrintFormat("%1.OnVoteStateUpdated(%2)", this, m_VoteState);
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
		PrintFormat("%1.LoadConfig()", this);

		if (!FileIO.FileExists("$profile:V30/MapVoting/config.json")) {
			PrintFormat("	Missing config file, it should be at '$profile:V30/MapVoting/config.json'.", level: LogLevel.ERROR);
			return;
		};

		auto jsonConfigDeserializer = V30_Json_FileDeserializer("$profile:V30/MapVoting/config.json");
		if (!jsonConfigDeserializer) {
			PrintFormat("	Failed to create deserializer config.", level: LogLevel.ERROR);
			return;
		}
		else if (!jsonConfigDeserializer.IsOpen()) {
			PrintFormat("	Failed to open config.", level: LogLevel.ERROR);
			return;
		};

		auto jsonConfig = jsonConfigDeserializer.Deserialize();
		if (!jsonConfig) {
			PrintFormat("	Failed to deserialize config.", level: LogLevel.ERROR);
			jsonConfigDeserializer.Close();
			return;
		}
		else if (jsonConfig.IsNull()) {
			PrintFormat("	Config is empty.", level: LogLevel.WARNING);
			jsonConfigDeserializer.Close();
			return;
		};

		auto config = jsonConfig.AsObject();
		if (!config) {
			PrintFormat("	Root value of config is not an object %1.", jsonConfig, level: LogLevel.ERROR);
			jsonConfigDeserializer.Close();
			return;
		};

		ParseConfig(config);
		jsonConfigDeserializer.Close();

		foreach (V30_MapVoting_ChoiceId id, V30_MapVoting_Choice choice : m_AvaiableChoices) {
			OnChoiceLoaded(id, choice);
		};
		OnAllChoicesLoaded(m_AvaiableChoices);
		OnConfigLoaded(config);
	};

	protected void ParseConfig(notnull V30_Json_object config) {
		// mode
		auto jsonMode = config.GetAt("mode");
		if (jsonMode.IsString()) {
			auto mode = jsonMode.AsString().Get();
			PrintFormat("	'mode' = %1.", mode);
			ParseConfigMode(mode, config);
		}
		else if (jsonMode.IsNull()) {
			PrintFormat("	'mode' is missing, 'whitelist' will be used.");
			ParseConfigMode("whitelist", config);
		}
		else {
			PrintFormat("	'mode' is not a string - %1.", jsonMode, level: LogLevel.ERROR);
			return;
		};

		// skipRepeats
		auto jsonSkipRepeats = config.GetAt("skipRepeats");
		if (jsonSkipRepeats.IsBool()/* || jsonSkipRepeats.IsInt()*/) {
			int skipRepeats;
			if (jsonSkipRepeats.IsBool()) {
				auto skipRepeatsBool = jsonSkipRepeats.AsBool().Get();
				PrintFormat("	'skipRepeats' = %1.", skipRepeatsBool);
				if (skipRepeatsBool) {
					skipRepeats = 1/*m_SaveLastChoicesCount*/;
				}
				else {
					skipRepeats = 0;
				};
			}
			//else /*if (jsonSkipRepeats.IsInt())*/ {
			//    TODO: How many previous votes to count
			//};

			if (skipRepeats > 0 && !m_RememberPreviousChoices) {
				PrintFormat("		m_RememberPreviousChoices attribute is set to false.", level: LogLevel.WARNING);
			}
			//else if (m_RememberPreviousChoicesCount != 0 && skipRepeats > m_RememberPreviousChoicesCount) {
			//	PrintFormat("		m_RememberPreviousChoicesCount is less than 'skipRepeats' - %1.", m_RememberPreviousChoicesCount, level: LogLevel.WARNING);
			//};

			if (skipRepeats > 0) {
				auto path = "$profile:V30/MapVoting/last.json";
				if (FileIO.FileExists(path)) {
					auto valueDeserializer = V30_Json_FileDeserializer(path);
					auto value = valueDeserializer.Deserialize();
					valueDeserializer.Close();

					if (!value) {
						PrintFormat("		Failed to deserialize '%1'.", path, level: LogLevel.ERROR);
					}
					else if (value.IsInt()) {
						V30_MapVoting_ChoiceId skipChoiceId = value.AsInt().Get();
						PrintFormat("		Skipping previous voted choice: %1.", skipChoiceId);
						m_AvaiableChoices.Remove(skipChoiceId);
					}
					//else if (value.IsArray()) {
					//    TODO
					//}
					else if (!value.IsNull()) {
						PrintFormat("		'%1' contains bad data, it shoud be only integer.", path, level: LogLevel.ERROR);
					};
				};
			};
		}
		else if (!jsonSkipRepeats.IsNull()) {
			PrintFormat("	'skipRepeats' is not a bool - %1.", jsonSkipRepeats, level: LogLevel.ERROR);
		};

		// count
		auto jsonCount = config.GetAt("count");
		if (jsonCount.IsInt()) {
			auto preserveRandom = true;
			auto jsonPreserveRandom = config.GetAt("countPreserveRandom");
			if (!jsonPreserveRandom.IsNull()) {
				if (jsonPreserveRandom.IsBool()) {
					preserveRandom = jsonPreserveRandom.AsBool().Get();
					PrintFormat("	'countPreserveRandom' = %1.", preserveRandom);
				}
				else {
					PrintFormat("	'countPreserveRandom' is not a bool - %1.", jsonPreserveRandom, level: LogLevel.ERROR);
				};
			};

			auto count = jsonCount.AsInt().Get();
			if (count >= 2) {
				PrintFormat("	'count' = %1.", count);
				if (count <= m_AvaiableChoices.Count()) {
					if (count != m_AvaiableChoices.Count()) {
						auto ids = new array<V30_MapVoting_ChoiceId>();
						foreach (V30_MapVoting_ChoiceId choiceId, V30_MapVoting_Choice choice : m_AvaiableChoices)
							if (!preserveRandom || !V30_MapVoting_ChoiceRandom.Cast(choice))
								ids.Insert(choiceId);

						PrintFormat("		Removing %1 choices:", m_AvaiableChoices.Count() - count);
						while (m_AvaiableChoices.Count() > count) {
							auto i = ids.GetRandomIndex();
							auto choiceId = ids.Get(i);
							ids.Remove(i);

							PrintFormat("			[%1]", choiceId);
							m_AvaiableChoices.Remove(choiceId);
						};
					}
					else {
						PrintFormat("		No options for remove.");
					};

				}
				else {
					PrintFormat("	'count' is less than number of avaiable options - %1.", m_AvaiableChoices.Count(), level: LogLevel.WARNING);
				};
			}
			else {
				PrintFormat("	'count' is less than 2.", level: LogLevel.ERROR);
			};
		}
		else if (!jsonCount.IsNull()) {
			PrintFormat("	'count' is not a number - %1.", jsonCount, level: LogLevel.ERROR);
		};


		auto voteAbility = config.GetAt("giveVoteAbility");
		if (voteAbility) {
			switch (voteAbility.GetType()) {
				case V30_Json_EValueType.STRING: {
					switch (voteAbility.AsString().Get()) {
						case "administrators": {
							m_GiveVoteAbility = V30_MapVoting_EGiveVoteAbility.ADMINISTRATORS;
							break;
						};
						case "session_administrators": {
							m_GiveVoteAbility = V30_MapVoting_EGiveVoteAbility.SESSION_ADMINISTRATORS;
							break;
						};
						case "game_masters": {
							m_GiveVoteAbility = V30_MapVoting_EGiveVoteAbility.GAME_MASTERS;
							break;
						};
						case "all": {
							m_GiveVoteAbility = V30_MapVoting_EGiveVoteAbility.ALL;
							break;
						};
						case "none": {
							m_GiveVoteAbility = V30_MapVoting_EGiveVoteAbility.NONE;
							break;
						};
						default: {
							PrintFormat("    'voteAbility' has bad option, available options: 'all', 'game_masters', 'session_administrators', 'administrators' or 'none'", level: LogLevel.ERROR);
							break;
						};
					};
					break;
				};
				case V30_Json_EValueType.NULL: {
					break;
				};
				default: {
					PrintFormat("    'voteAbility' is not a string", level: LogLevel.ERROR);
					break;
				};
			};
		};

		auto runMethod = config.GetAt("runMethod");
		if (runMethod && !runMethod.IsNull()) {
			switch (runMethod.GetType()) {
				case V30_Json_EValueType.STRING: {
					switch (runMethod.AsString().Get()) {
						case "RequestScenarioChangeTransition": {
							m_Runner = new V30_MapVoting_Runner_RequestScenarioChangeTransition();
							break;
						};
						case "Podval": {
							auto url = config.GetAt("podvalUrl").AsString().Get();
							auto serverId = config.GetAt("podvalServerId").AsString().Get();
							m_Runner = new V30_MapVoting_Runner_Podval(url, serverId);
							break;
						};
						case "ScriptedRestart": {
							m_Runner = new V30_MapVoting_Runner_ScriptedRestart();
							break;
						};
						default : {
							PrintFormat("    Option 'runMethod' has unsupported value!", level: LogLevel.ERROR);
							break;
						};
					};
					break;
				};
				default : {
					PrintFormat("    Option 'runMethod' has unsupported type!", level: LogLevel.ERROR);
					break;
				};
			};
		}
		else {
			m_Runner = new V30_MapVoting_Runner_RequestScenarioChangeTransition();
		};
	};

	protected void ParseConfigMode(string mode, notnull V30_Json_object config) {
		if (mode == "whitelist") {
			// list
			auto jsonList = config.GetAt("list");
			if (!jsonList) {
				PrintFormat("	'list' is missing.", level: LogLevel.ERROR);
				return;
			}
			else if (!jsonList.IsArray()) {
				PrintFormat("	'list' is not a array - %1.", jsonList, level: LogLevel.ERROR);
				return;
			};

			auto list = jsonList.AsArray().Get();
			if (list.IsEmpty()) {
				PrintFormat("	'list' is empty.", level: LogLevel.ERROR);
				return;
			}
			else if (list.Count() < 2) {
				PrintFormat("	'list' contains less than 2 options.", level: LogLevel.WARNING);
			};

			PrintFormat("	'list':");
			ParseConfigWhitelist(list, config);
		}
		else {
			PrintFormat("	Unknown mode '%1',", mode, level: LogLevel.ERROR);
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
			PrintFormat("		[%1] = '%2'.", id, path);
			if (path == "#random") {
				return new V30_MapVoting_ChoiceRandom();
			}
			else {
				return new V30_MapVoting_ChoiceMission(path);
			};
		}
		//else if (entry.IsObject()) {
		//    TODO;
		//}
		else {
			PrintFormat("		[%1] is not a string - %2.", id, entry, level: LogLevel.ERROR);
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
		PrintFormat("%1.OnAllChoicesLoaded(%2[%3])", this, choices, choices.Count());
		m_IsAllChoicesLoaded = true;
		m_OnAllChoicesLoaded.Invoke(this, choices);
	};

	protected event void OnConfigLoaded(notnull V30_Json_object config) {
		m_OnConfigLoaded.Invoke(this, config);
	};

	ScriptInvoker GetOnConfigLoaded() {
		return m_OnConfigLoaded;
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
			Rpc(RpcDo_SendChoice, choiceId, choice.GetPreviewData());
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

	void StartVote() {
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

		auto game = GetGame();
		auto playerManager = game.GetPlayerManager();
		array<int> playerIds = new array<int>();
		playerManager.GetPlayers(playerIds);
		foreach (int playerId : playerIds) {
			switch (m_GiveVoteAbility) {
				case V30_MapVoting_EGiveVoteAbility.ADMINISTRATORS: {
					if (playerManager.GetPlayerRoles(playerId) & (EPlayerRole.ADMINISTRATOR)) {
						GivePlayerVoteAbility(playerId);
					};
					break;
				};
				case V30_MapVoting_EGiveVoteAbility.SESSION_ADMINISTRATORS: {
					if (playerManager.GetPlayerRoles(playerId) & (EPlayerRole.ADMINISTRATOR | EPlayerRole.SESSION_ADMINISTRATOR)) {
						GivePlayerVoteAbility(playerId);
					};
					break;
				};
				case V30_MapVoting_EGiveVoteAbility.GAME_MASTERS: {
					if (playerManager.GetPlayerRoles(playerId) & (EPlayerRole.ADMINISTRATOR | EPlayerRole.SESSION_ADMINISTRATOR | EPlayerRole.GAME_MASTER)) {
						GivePlayerVoteAbility(playerId);
					};
					break;
				};
				case V30_MapVoting_EGiveVoteAbility.ALL: {
					GivePlayerVoteAbility(playerId);
					break;
				};
			};
		};
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

	void EndVote() {
		PrintFormat("%1.EndVote()", this);

		if (m_RplComponent.Role() != RplRole.Authority) {
			PrintFormat("	Called on Non-Authority.", level: LogLevel.ERROR);
			return;
		};

		if (m_SetRandomizationSeed) {
			PrintFormat("	Randomize with seed %1.", Math.Randomize(m_RandomizationSeed));
		};

		foreach (V30_MapVoting_Choice choice : m_AvaiableChoices) {
			choice.OnVoteEnd();
		};

		SelectWinner();

		if (m_RememberPreviousChoices) {
			auto choiceId = m_WinnerId;
			while (V30_MapVoting_ChoiceWrapper.Cast(GetChoice(choiceId))) choiceId = V30_MapVoting_ChoiceWrapper.Cast(GetChoice(choiceId)).GetChoiceId();
			auto serializer = V30_Json_FileSerializer("$profile:V30/MapVoting/last.json");
			serializer.Serialize(V30_Json_int(choiceId));
			serializer.Close();
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

		m_WinnerPreview = GetChoice(m_WinnerId).GetPreviewData();
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

		if (choiceId != V30_MapVoting_NoChoice && !m_AvaiableChoices.Contains(choiceId)) {
			PrintFormat("%1: No choice with id %1 found.", this, choiceId, level: LogLevel.ERROR);
			return;
		};

		Rpc(RplDo_SetPlayerChoice, playerId, choiceId);
		RplDo_SetPlayerChoice(playerId, choiceId);
	};

	void RemovePlayerChoice(int playerId) {
		SetPlayerChoice(playerId, V30_MapVoting_NoChoice);
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

	int CountVotedPlayers() {
		int count = 0;
		foreach (V30_MapVoting_ChoiceId playerChoiceId : m_PlayerChoices) {
			if (playerChoiceId != V30_MapVoting_NoChoice) {
				count++;
			};
		};
		return count;
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
		if (IsVoteStarted() && m_GiveVoteAbility == V30_MapVoting_EGiveVoteAbility.ALL) GivePlayerVoteAbility(playerId);
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
		TakePlayerVoterAbility(playerId);
	};

	[RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
	protected void RplDo_OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout) {
		PrintFormat("%1.OnPlayerDisconnected(%2, %3, %4)", this, playerId, cause, timeout);
		auto playerVote = GetPlayerChoice(playerId);
		if (playerVote != V30_MapVoting_NoChoice) OnPlayerVoteChanged(playerId, V30_MapVoting_NoChoice, playerVote);
		m_PlayerChoices.Remove(playerId);
	};

	override void OnPlayerRoleChange(int playerId, EPlayerRole roleFlags) {
		if (!IsVoteStarted()) return;

		switch (m_GiveVoteAbility) {
			case V30_MapVoting_EGiveVoteAbility.ADMINISTRATORS: {
				SetPlayerVoteAbility(playerId, (roleFlags & (EPlayerRole.ADMINISTRATOR)) != 0);
				break;
			};
			case V30_MapVoting_EGiveVoteAbility.SESSION_ADMINISTRATORS: {
				SetPlayerVoteAbility(playerId, (roleFlags & (EPlayerRole.ADMINISTRATOR | EPlayerRole.SESSION_ADMINISTRATOR)) != 0);
				break;
			};
			case V30_MapVoting_EGiveVoteAbility.GAME_MASTERS: {
				SetPlayerVoteAbility(playerId, (roleFlags & (EPlayerRole.ADMINISTRATOR | EPlayerRole.SESSION_ADMINISTRATOR | EPlayerRole.GAME_MASTER)) != 0);
				break;
			};
		};
	};



	// --------- Vote Ability --------- //
	void SetPlayerVoteAbility(int playerId, bool voteAbility) {
		if (m_RplComponent.Role() != RplRole.Authority) return;
		if (!GetGame().GetPlayerManager().IsPlayerConnected(playerId)) return;
		if (voteAbility) GivePlayerVoteAbility(playerId);
		else TakePlayerVoterAbility(playerId);
	};

	void GivePlayerVoteAbility(int playerId) {
		if (m_RplComponent.Role() != RplRole.Authority) return;
		if (!GetGame().GetPlayerManager().IsPlayerConnected(playerId)) return;
		if (IsPlayerHasVoteAbility(playerId)) return;
		m_Voters.Insert(playerId);
		m_VotersCount++;
		Rpc(RpcDo_UpdateVotersCount, m_VotersCount);
		OnPlayerVoteAbilityChanged(playerId, true);
		GetPlayerControllerComponent(playerId).OnVoteAbilityChanged(true);
	};

	void TakePlayerVoterAbility(int playerId) {
		if (m_RplComponent.Role() != RplRole.Authority) return;
		if (!GetGame().GetPlayerManager().IsPlayerConnected(playerId)) return;
		if (!IsPlayerHasVoteAbility(playerId)) return;
		m_Voters.RemoveItem(playerId);
		m_VotersCount--;
		Rpc(RpcDo_UpdateVotersCount, m_VotersCount);
		OnPlayerVoteAbilityChanged(playerId, false);
		RemovePlayerChoice(playerId);
		GetPlayerControllerComponent(playerId).OnVoteAbilityChanged(false);
	};

	[RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
	protected void RpcDo_UpdateVotersCount(int votersCount) {
		m_VotersCount = votersCount;
	};

	bool IsPlayerHasVoteAbility(int playerId) {
		return m_Voters.Contains(playerId);
	};

	int CountPlayersWithVoteAbility() {
		return m_VotersCount; //m_Voters.Count();
	};

	protected void OnPlayerVoteAbilityChanged(int playerId, bool hasVoteAbility) {
		PrintFormat("%1.OnPlayerVoteAbilityChanged(%2, %3)", this, playerId, hasVoteAbility);
		m_OnPlayerVoteAbilityChanged.Invoke(this, playerId, hasVoteAbility);
	};

	ScriptInvoker GetOnPlayerVoteAbilityChanged() {
		return m_OnPlayerVoteAbilityChanged;
	};

	V30_MapVoting_PlayerControllerComponent GetLocalPlayerControllerComponent() {
		return V30_MapVoting_PlayerControllerComponent.GetLocalInstance();
	};

	V30_MapVoting_PlayerControllerComponent GetPlayerControllerComponent(int playerId) {
		return V30_MapVoting_PlayerControllerComponent.GetInstance(playerId);
	};
};
