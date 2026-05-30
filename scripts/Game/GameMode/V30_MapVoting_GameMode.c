// TODO: Move logic to GameSystem

[EntityEditorProps(category: "GameScripted/Misc", description: "")]
class V30_MapVoting_GameModeClass : SCR_BaseGameModeClass {
};

class V30_MapVoting_GameMode : SCR_BaseGameMode {
	[Attribute("true", category: "Time")]
	protected bool m_V30_MapVoting_EnableTimeLimit;

	[Attribute("300", desc: "Total time limit for viting (in seconds)", params: "0 inf", uiwidget: "editbox", category: "Time")]
	protected int m_V30_MapVoting_TimeLimit;

	[Attribute("true", category: "Time")]
	protected bool m_V30_MapVoting_EnableThresholdTime;

	[Attribute("30", params: "0 inf", category: "Time")]
	protected int m_V30_MapVoting_ThresholdTime;

	[Attribute("true", category: "Time")]
	protected bool m_V30_MapVoting_EnableThresholdPercentage;

	[Attribute("50", desc: "How many players in percentages should vote to start ThresholdTime.", uiwidget: "slider", params: "0 100", category: "Time")]
	protected int m_V30_MapVoting_ThresholdPercentage;

	[Attribute("false", category: "Time")]
	protected bool m_V30_MapVoting_EnableThresholdCount;

	[Attribute("1", desc: "How many players should vote to start ThresholdTime.", uiwidget: "editbox", params: "0 inf", category: "Time")]
	protected int m_V30_MapVoting_ThresholdCount;

	[Attribute(desc: "Time between vote is ended and starting of loading next scenario (in ms)", defvalue: "14000", params: "0 inf")]
	protected int m_V30_MapVoting_StartVotedMissionDelay;

	protected bool m_V30_MapVoting_TimeTicking = false;

	protected WorldTimestamp m_V30_MapVoting_TimeStarted;
	protected WorldTimestamp m_V30_MapVoting_ThresholdTimeStarted;

	protected bool m_V30_MapVoting_ThresholdTimeTicking = false;

	protected bool m_V30_MapVoting_UpdateTimerTicking = false;

	protected WorldTimestamp m_V30_MapVoting_TimeLeftSync;

	protected int m_V30_MapVoting_TimeLeft;


	protected V30_MapVoting_GameModeComponent m_V30_MapVoting_GameModeComponent;

	void V30_MapVoting_GameMode(IEntitySource src, IEntity parent) {
		SetEventMask(EntityEvent.INIT);
	};

	override protected event void EOnInit(IEntity owner) {
		super.EOnInit(owner);
		m_V30_MapVoting_GameModeComponent = V30_MapVoting_GameModeComponent.Cast(FindComponent(V30_MapVoting_GameModeComponent));
		m_V30_MapVoting_GameModeComponent.GetOnConfigLoaded().Insert(OnConfigLoaded);
		m_V30_MapVoting_GameModeComponent.GetOnVoteStarted().Insert(OnVoteStarted);
		m_V30_MapVoting_GameModeComponent.GetOnVoteEnded().Insert(OnVoteEnded);
		m_V30_MapVoting_GameModeComponent.GetOnPlayerVoteChanged().Insert(OnPlayerVoteChanged);
	};

	protected V30_MapVoting_GameModeComponent V30_MapVoting_GetGameModeComponent() {
		return m_V30_MapVoting_GameModeComponent;
	};

	override protected void OnGameModeStart() {
		super.OnGameModeStart();

		if (IsMaster()) {
			m_V30_MapVoting_GameModeComponent.StartVote();
		};

		if (!System.IsConsoleApp()) {
			GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.V30_MapVoting_Menu);
		};
	};

	override protected void OnGameModeEnd(SCR_GameModeEndData endData) {
		super.OnGameModeEnd(endData);

		if (!IsMaster()) {
			return;
		};

		if (!m_V30_MapVoting_GameModeComponent.IsVoteEnded()) {
			m_V30_MapVoting_GameModeComponent.EndVote();
		};

		GetGame().GetCallqueue().Remove(V30_MapVoting_StartVotingMission);
	};

	override float GetAutoReloadDelay() {
		if (m_V30_MapVoting_OverrideOnGameModeEnd) {
			return super.GetAutoReloadDelay();
		};

		return ((float)m_V30_MapVoting_StartVotedMissionDelay) / 1000.0;
	};

	protected void V30_MapVoting_StartVotedMission() {
		if (!IsMaster())
			return;

		m_V30_MapVoting_GameModeComponent.RunWinner();
	};

	protected event void OnConfigLoaded(V30_MapVoting_GameModeComponent votingGameModeComponent, V30_Json_object config) {
		auto time = config.GetAt("time");
		if (time) {
			switch (time.GetType()) {
				case V30_Json_EValueType.INT: {
					m_V30_MapVoting_EnableTimeLimit = true;
					m_V30_MapVoting_TimeLimit = time.AsInt().Get();
					break;
				};
				case V30_Json_EValueType.FLOAT: {
					m_V30_MapVoting_EnableTimeLimit = true;
					m_V30_MapVoting_TimeLimit = time.AsFloat().Get();
					break;
				};
				case V30_Json_EValueType.BOOL: {
					m_V30_MapVoting_EnableTimeLimit = time.AsBool().Get();
					break;
				};
				case V30_Json_EValueType.NULL: {
					break;
				};
				default: {
					PrintFormat("	'time' is not a number or bool - %1", time, level: LogLevel.ERROR);
					break;
				};
			};
		};

		auto thresholdTime = config.GetAt("thresholdTime");
		if (thresholdTime) {
			switch (thresholdTime.GetType()) {
				case V30_Json_EValueType.INT: {
					m_V30_MapVoting_EnableThresholdTime = true;
					m_V30_MapVoting_ThresholdTime = thresholdTime.AsInt().Get();
					break;
				};
				case V30_Json_EValueType.FLOAT: {
					m_V30_MapVoting_EnableThresholdTime = true;
					m_V30_MapVoting_ThresholdTime = thresholdTime.AsFloat().Get();
					break;
				};
				case V30_Json_EValueType.BOOL: {
					m_V30_MapVoting_EnableThresholdTime = thresholdTime.AsBool().Get();
					break;
				};
				case V30_Json_EValueType.NULL: {
					break;
				};
				default: {
					PrintFormat("	'thresholdTime' is not a number or bool - %1.", thresholdTime, level: LogLevel.ERROR);
					break;
				};
			};
		};

		auto thresholdPercentage = config.GetAt("thresholdPercentage");
		if (thresholdPercentage) {
			switch (thresholdPercentage.GetType()) {
				case V30_Json_EValueType.FLOAT: {
					m_V30_MapVoting_EnableThresholdPercentage = true;
					m_V30_MapVoting_ThresholdPercentage = Math.Clamp(thresholdPercentage.AsFloat().Get(), 0.0, 100.0);
					break;
				};
				case V30_Json_EValueType.INT: {
					m_V30_MapVoting_EnableThresholdPercentage = true;
					m_V30_MapVoting_ThresholdPercentage = Math.ClampInt(thresholdPercentage.AsInt().Get(), 0, 100);
					break;
				};
				case V30_Json_EValueType.BOOL: {
					m_V30_MapVoting_EnableThresholdPercentage = thresholdPercentage.AsBool().Get();
					break;
				};
				case V30_Json_EValueType.NULL: {
					break;
				};
				default: {
					PrintFormat("	'thresholdPercentage' is not a number or bool - %1.", thresholdPercentage, level: LogLevel.ERROR);
					break;
				};
			};
		};

		auto thresholdCount = config.GetAt("thresholdCount");
		if (thresholdCount) {
			switch (thresholdCount.GetType()) {
				case V30_Json_EValueType.INT: {
					m_V30_MapVoting_EnableThresholdCount = true;
					m_V30_MapVoting_ThresholdCount = thresholdCount.AsInt().Get();
					break;
				};
				case V30_Json_EValueType.BOOL: {
					m_V30_MapVoting_EnableThresholdCount = thresholdCount.AsBool().Get();
					break;
				};
				case V30_Json_EValueType.NULL: {
					break;
				};
				default: {
					PrintFormat("	'thresholdCount' is not a number or bool - %1.", thresholdCount, level: LogLevel.ERROR);
					break;
				};
			};
		};
	};

	protected event void OnVoteStarted(V30_MapVoting_GameModeComponent votingGameModeComponent) {
		if (!IsMaster()) {
			return;
		};

		GetGame().GetCallqueue().CallLater(UpdateTimersTicking);
	};

	protected event void OnVoteEnded(V30_MapVoting_GameModeComponent votingGameModeComponent, V30_MapVoting_ChoiceId winnerId) {
		if (IsRunning()) {
			EndGameMode(SCR_GameModeEndData.CreateSimple(EGameOverTypes.V30_MAPVOTING_NEXT_MAP));
		};

		GetGame().GetCallqueue().Remove(EndVoteTimer);
		GetGame().GetCallqueue().Remove(EndVoteThresholdTimer);
		GetGame().GetCallqueue().Remove(UpdateTimer);
		GetGame().GetCallqueue().CallLater(V30_MapVoting_StartVotedMission, m_V30_MapVoting_StartVotedMissionDelay, false);
	};

	event void OnPlayerVoteChanged(V30_MapVoting_GameModeComponent votingGameModeComponent, int playerId, V30_MapVoting_ChoiceId choiceId, V30_MapVoting_ChoiceId oldChoiceId) {
		GetGame().GetCallqueue().CallLater(UpdateTimersTicking);
	};

	override event void OnPlayerConnected(int playerId) {
		super.OnPlayerConnected(playerId);
		GetGame().GetCallqueue().CallLater(UpdateTimersTicking);
	};

	override event void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout) {
		super.OnPlayerDisconnected(playerId, cause, timeout);
		GetGame().GetCallqueue().CallLater(UpdateTimersTicking);
	};

	protected void UpdateTimersTicking() {
		if (!m_V30_MapVoting_GameModeComponent.IsVoteStarted() || GetGame().GetPlayerManager().GetPlayerCount() == 0) {
			if (IsTimerTicking()) StopTimer();
			if (IsThresholdTimerTicking()) StopThresholdTimer();
			return;
		};

		if (m_V30_MapVoting_EnableTimeLimit) {
			if (!IsTimerTicking()) StartTimer();
		}
		else {
			if (IsTimerTicking()) StopTimer();
		};

		if (m_V30_MapVoting_EnableThresholdTime && IsThresholdPass()) {
			if (!IsThresholdTimerTicking()) StartThresholdTimer();
		}
		else {
			if (IsThresholdTimerTicking()) StopThresholdTimer();
		};
	};

	int GetVotingTimeLimit() {
		return m_V30_MapVoting_TimeLimit;
	};

	override float GetTimeLimit() {
		auto votingTimeLimit = GetVotingTimeLimit();
		if (votingTimeLimit == int.MAX) {
			return 0;
		};
		return votingTimeLimit;
	};

	bool IsTicking() {
		if (m_RplComponent.Role() == RplRole.Authority) {
			return IsTimerTicking() || IsThresholdTimerTicking();
		}
		else {
			return m_V30_MapVoting_TimeLeft != int.MAX;
		};
	};

	int GetVotingRemainingTime() {
		if (m_RplComponent.Role() == RplRole.Authority) {
			if (!IsTimerTicking() && !IsThresholdTimerTicking()) return int.MAX;
			auto timer = GetTimer();
			auto thresholdTimer = GetThreshouldTimer();
			if (thresholdTimer < timer) return thresholdTimer;
			return timer;
		}
		else if (m_V30_MapVoting_TimeLeft != int.MAX) {
			auto time = m_V30_MapVoting_TimeLeft - GetGame().GetWorld().GetTimestamp().DiffMilliseconds(m_V30_MapVoting_TimeLeftSync);
			if (time < 0)
				return 0;
			return time;
		}
		else {
			return int.MAX;
		};
	};

	override float GetRemainingTime() {
		auto votingRemainingTime = GetVotingRemainingTime();
		if (votingRemainingTime == int.MAX) {
			return -1;
		};

		//float fvotingRemainingTime = votingRemainingTime;
		return ((float)votingRemainingTime) / 1000.0;
	};

	bool IsThresholdPass() {
		float total = GetGame().GetPlayerManager().GetPlayerCount();
		float voted = m_V30_MapVoting_GameModeComponent.CountVotedPlayers();
		float percentage = m_V30_MapVoting_ThresholdPercentage;
		return (m_V30_MapVoting_EnableThresholdCount && (voted >= m_V30_MapVoting_ThresholdCount))
			|| (m_V30_MapVoting_EnableThresholdPercentage && ((voted / total) >= (percentage / 100.0)));
	};

	protected void StartTimer() {
		if (m_V30_MapVoting_TimeTicking) return;
		GetGame().GetCallqueue().CallLater(EndVoteTimer, delay: m_V30_MapVoting_TimeLimit * 1000);
		m_V30_MapVoting_TimeStarted = GetGame().GetWorld().GetTimestamp();
		m_V30_MapVoting_TimeTicking = true;
		StartUpdateTimer();
		UpdateTimer();
	};

	protected void StopTimer() {
		if (!m_V30_MapVoting_TimeTicking) return;
		GetGame().GetCallqueue().Remove(EndVoteTimer);
		m_V30_MapVoting_TimeTicking = false;
		if (!IsThresholdTimerTicking()) StopUpdateTimer();
		UpdateTimer();
	};

	bool IsTimerTicking() {
		return m_V30_MapVoting_TimeTicking;
	};

	int GetTimer() {
		if (!m_V30_MapVoting_TimeTicking) return int.MAX;
		auto time = m_V30_MapVoting_TimeLimit * 1000 - GetGame().GetWorld().GetTimestamp().DiffMilliseconds(m_V30_MapVoting_TimeStarted);
		if (time < 0)
			return 0;
		return time;
		//return GetGame().GetCallqueue().GetVotingRemainingTime(EndVoteTimer);
	};

	protected void EndVoteTimer() {
		m_V30_MapVoting_GameModeComponent.EndVote();
	};



	protected void StartThresholdTimer() {
		if (m_V30_MapVoting_ThresholdTimeTicking) return;
		GetGame().GetCallqueue().CallLater(EndVoteThresholdTimer, delay: m_V30_MapVoting_ThresholdTime * 1000);
		m_V30_MapVoting_ThresholdTimeStarted = GetGame().GetWorld().GetTimestamp();
		m_V30_MapVoting_ThresholdTimeTicking = true;
		StartUpdateTimer();
		UpdateTimer();
	};

	protected void StopThresholdTimer() {
		if (!m_V30_MapVoting_ThresholdTimeTicking) return;
		auto t1 = GetGame().GetCallqueue().GetRemainingTime(EndVoteThresholdTimer);
		GetGame().GetCallqueue().Remove(EndVoteThresholdTimer);
		auto t2 = GetGame().GetCallqueue().GetRemainingTime(EndVoteThresholdTimer);
		m_V30_MapVoting_ThresholdTimeTicking = false;
		if (!IsTimerTicking()) StopUpdateTimer();
		UpdateTimer();
	};

	bool IsThresholdTimerTicking() {
		return m_V30_MapVoting_ThresholdTimeTicking;
	};

	int GetThreshouldTimer() {
		if (!m_V30_MapVoting_ThresholdTimeTicking) return int.MAX;
		auto time = m_V30_MapVoting_ThresholdTime * 1000 - GetGame().GetWorld().GetTimestamp().DiffMilliseconds(m_V30_MapVoting_ThresholdTimeStarted);
		if (time < 0)
			return 0;
		return time;
		//return GetGame().GetCallqueue().GetVotingRemainingTime(EndVoteThresholdTimer);
	};

	void EndVoteThresholdTimer() {
		m_V30_MapVoting_GameModeComponent.EndVote();
	};



	protected void StartUpdateTimer() {
		if (m_V30_MapVoting_UpdateTimerTicking) return;
		GetGame().GetCallqueue().CallLater(UpdateTimer, delay: 5 * 1000, repeat: true);
		UpdateTimer();
	};

	protected void StopUpdateTimer() {
		if (!m_V30_MapVoting_UpdateTimerTicking) return;
		GetGame().GetCallqueue().Remove(UpdateTimer);
		UpdateTimer();
	};

	protected bool IsUpdateTimerTicking() {
		return m_V30_MapVoting_UpdateTimerTicking;
	};

	protected void UpdateTimer() {
		Rpc(RpcDo_UpdateTimer, GetVotingRemainingTime());
	};

	[RplRpc(channel: RplChannel.Unreliable, rcver: RplRcver.Broadcast)]
	protected void RpcDo_UpdateTimer(int timeLeft) {
		m_V30_MapVoting_TimeLeft = timeLeft;
		m_V30_MapVoting_TimeLeftSync = GetGame().GetWorld().GetTimestamp();
	};



	override protected bool RplSave(ScriptBitWriter writer) {
		if (!super.RplSave(writer)) return false;
		writer.WriteInt(GetVotingRemainingTime());
		return true;
	};

	override protected bool RplLoad(ScriptBitReader reader) {
		if (!super.RplLoad(reader)) return false;
		reader.ReadInt(m_V30_MapVoting_TimeLeft);
		m_V30_MapVoting_TimeLeftSync = GetGame().GetWorld().GetTimestamp();
		return true;
	};
};
