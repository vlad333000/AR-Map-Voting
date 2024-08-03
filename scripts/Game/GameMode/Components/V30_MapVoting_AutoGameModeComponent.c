[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class V30_MapVoting_AutoGameModeComponentClass : V30_MapVoting_GameModeComponentClass {
};

class V30_MapVoting_AutoGameModeComponent : V30_MapVoting_GameModeComponent {
	[Attribute("true", category: "Time")]
	protected bool m_EnableTimeLimit;
	
	[Attribute("300", desc: "Total time limit for viting (in seconds)", params: "0 inf", uiwidget: "editbox", category: "Time")]
	protected int m_TimeLimit;
	
	[Attribute("true", category: "Time")]
	protected bool m_EnableThresholdTime;
	
	[Attribute("30", params: "0 inf", category: "Time")]
	protected int m_ThresholdTime;
	
	[Attribute("true", category: "Time")]
	protected bool m_EnableThresholdPercentage;
	
	[Attribute("50", desc: "How many players in percentages should vote to start ThresholdTime.", uiwidget: "slider", params: "0 100", category: "Time")]
	protected int m_ThresholdPercentage;
	
	[Attribute("false", category: "Time")]
	protected bool m_EnableThresholdCount;
	
	[Attribute("1", desc: "How many players should vote to start ThresholdTime.", uiwidget: "editbox", params: "0 inf", category: "Time")]
	protected int m_ThresholdCount;
	
	
	
	protected const float m_TimeForRun = 14.0;
	
	protected bool m_TimeTicking;
	
	protected WorldTimestamp m_TimeStarted;
	protected WorldTimestamp m_ThresholdTimeStarted;
	
	protected bool m_ThresholdTimeTicking;
	
	protected bool m_UpdateTimerTicking;
	
	protected WorldTimestamp m_TimeLeftSync;
	
	protected int m_TimeLeft;
	
	void V30_MapVoting_AutoGameModeComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {
		m_TimeTicking = false;
		m_ThresholdTimeTicking = false;
		m_UpdateTimerTicking = false;
	};
	
	
	override void ParseConfig(notnull V30_Json_object config) {
		super.ParseConfig(config);
		
		auto time = config.GetAt("time");
		if (time) {
			switch (time.GetType()) {
				case V30_Json_EValueType.INT: {
					m_EnableTimeLimit = true;
					m_TimeLimit = time.AsInt().Get();
					break;
				};
				case V30_Json_EValueType.FLOAT: {
					m_EnableTimeLimit = true;
					m_TimeLimit = time.AsFloat().Get();
					break;
				};
				case V30_Json_EValueType.BOOL: {
					m_EnableTimeLimit = time.AsBool().Get();
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
					m_EnableThresholdTime = true;
					m_ThresholdTime = thresholdTime.AsInt().Get();
					break;
				};
				case V30_Json_EValueType.FLOAT: {
					m_EnableThresholdTime = true;
					m_ThresholdTime = thresholdTime.AsFloat().Get();
					break;
				};
				case V30_Json_EValueType.BOOL: {
					m_EnableThresholdTime = thresholdTime.AsBool().Get();
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
					m_EnableThresholdPercentage = true;
					m_ThresholdPercentage = Math.Clamp(thresholdPercentage.AsFloat().Get(), 0.0, 100.0);
					break;
				};
				case V30_Json_EValueType.INT: {
					m_EnableThresholdPercentage = true;
					m_ThresholdPercentage = Math.ClampInt(thresholdPercentage.AsInt().Get(), 0, 100);
					break;
				};
				case V30_Json_EValueType.BOOL: {
					m_EnableThresholdPercentage = thresholdPercentage.AsBool().Get();
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
					m_EnableThresholdCount = true;
					m_ThresholdCount = thresholdCount.AsInt().Get();
					break;
				};
				case V30_Json_EValueType.BOOL: {
					m_EnableThresholdCount = thresholdCount.AsBool().Get();
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
	
	
	
	override event void OnGameModeStart() {
		GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.V30_MapVoting_Menu);
		if (m_RplComponent.Role() != RplRole.Authority) return;
		StartVote();
	};
	
	override event void OnVoteStarted() {
		super.OnVoteStarted();
		if (m_RplComponent.Role() != RplRole.Authority) return;
		GetGame().GetCallqueue().CallLater(UpdateTimersTicking);
	};
	
	override event void OnGameModeEnd(SCR_GameModeEndData data) {
		if (m_RplComponent.Role() != RplRole.Authority) return;
		if (!IsVoteEnded()) EndVote();
	};
	
	override event void OnVoteEnded(V30_MapVoting_ChoiceId winnerId) {
		super.OnVoteEnded(winnerId);
		if (m_RplComponent.Role() != RplRole.Authority) return;
		GetGameMode().EndGameMode(SCR_GameModeEndData.CreateSimple(EGameOverTypes.V30_MAPVOTING_NEXT_MAP));
		GetGame().GetCallqueue().Remove(EndVoteTimer);
		GetGame().GetCallqueue().Remove(EndVoteThresholdTimer);
		GetGame().GetCallqueue().Remove(UpdateTimer);
		GetGame().GetCallqueue().CallLater(RunWinner, delay: m_TimeForRun * 1000);
	};
	
	
	
	override event void OnPlayerConnected(int playerId) {
		super.OnPlayerConnected(playerId);
		GetGame().GetCallqueue().CallLater(UpdateTimersTicking);
	};
	
	override event void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout) {
		super.OnPlayerDisconnected(playerId, cause, timeout);
		GetGame().GetCallqueue().CallLater(UpdateTimersTicking);
	};
	
	override event void OnPlayerVoteChanged(int playerId, V30_MapVoting_ChoiceId choiceId, V30_MapVoting_ChoiceId oldChoiceId) {
		super.OnPlayerVoteChanged(playerId, choiceId, oldChoiceId);
		GetGame().GetCallqueue().CallLater(UpdateTimersTicking);
	};
	
	protected void UpdateTimersTicking() {
		if (!IsVoteStarted() || GetGame().GetPlayerManager().GetPlayerCount() == 0) {
			if (IsTimerTicking()) StopTimer();
			if (IsThresholdTimerTicking()) StopThresholdTimer();
			return;
		};
		
		if (m_EnableTimeLimit) {
			if (!IsTimerTicking()) StartTimer();
		}
		else {
			if (IsTimerTicking()) StopTimer();
		};
		
		if (m_EnableThresholdTime && IsThresholdPass()) {
			if (!IsThresholdTimerTicking()) StartThresholdTimer();
		}
		else {
			if (IsThresholdTimerTicking()) StopThresholdTimer();
		};
	};
	
	
	
	int GetTimeLimit() {
		return m_TimeLimit;
	};
	
	bool IsTicking() {
		if (m_RplComponent.Role() == RplRole.Authority) {
			return IsTimerTicking() || IsThresholdTimerTicking();
		}
		else {
			return m_TimeLeft != int.MAX;
		};
	};
	
	int GetRemainingTime() {
		if (m_RplComponent.Role() == RplRole.Authority) {
			if (!IsTimerTicking() && !IsThresholdTimerTicking()) return int.MAX;
			auto timer = GetTimer();
			auto thresholdTimer = GetThreshouldTimer();
			if (thresholdTimer < timer) return thresholdTimer;
			return timer;
		}
		else if (m_TimeLeft != int.MAX) {
			return m_TimeLeft - GetGame().GetWorld().GetTimestamp().DiffMilliseconds(m_TimeLeftSync);
		}
		else {
			return int.MAX;
		};
	};
	
	bool IsThresholdPass() {
		float total = GetGame().GetPlayerManager().GetPlayerCount();
		float voted = CountVotedPlayers();
		float percentage = m_ThresholdPercentage;
		return (m_EnableThresholdCount && (voted >= m_ThresholdCount))
			|| (m_EnableThresholdPercentage && ((voted / total) >= (percentage / 100.0)));
	};
	
	protected void StartTimer() {
		if (m_TimeTicking) return;
		GetGame().GetCallqueue().CallLater(EndVoteTimer, delay: m_TimeLimit * 1000);
		m_TimeStarted = GetGame().GetWorld().GetTimestamp();
		m_TimeTicking = true;
		StartUpdateTimer();
	};
	
	protected void StopTimer() {
		if (!m_TimeTicking) return;
		GetGame().GetCallqueue().Remove(EndVoteTimer);
		m_TimeTicking = false;
		if (!IsThresholdTimerTicking()) StopUpdateTimer();
	};
	
	bool IsTimerTicking() {
		return m_TimeTicking;
	};
	
	int GetTimer() {
		if (!m_TimeTicking) return int.MAX;
		return m_TimeLimit * 1000 - GetGame().GetWorld().GetTimestamp().DiffMilliseconds(m_TimeStarted);
		//return GetGame().GetCallqueue().GetRemainingTime(EndVoteTimer);
	};
	
	protected void EndVoteTimer() {
		EndVote();
	};
	
	
	
	protected void StartThresholdTimer() {
		if (m_ThresholdTimeTicking) return;
		GetGame().GetCallqueue().CallLater(EndVoteThresholdTimer, delay: m_ThresholdTime * 1000);
		m_ThresholdTimeStarted = GetGame().GetWorld().GetTimestamp();
		m_ThresholdTimeTicking = true;
		StartUpdateTimer();
	};
	
	protected void StopThresholdTimer() {
		if (!m_ThresholdTimeTicking) return;
		auto t1 = GetGame().GetCallqueue().GetRemainingTime(EndVoteThresholdTimer);
		GetGame().GetCallqueue().Remove(EndVoteThresholdTimer);
		auto t2 = GetGame().GetCallqueue().GetRemainingTime(EndVoteThresholdTimer);
		m_ThresholdTimeTicking = false;
		if (!IsTimerTicking()) StopUpdateTimer();
	};
	
	bool IsThresholdTimerTicking() {
		return m_ThresholdTimeTicking;
	};
	
	int GetThreshouldTimer() {
		if (!m_ThresholdTimeTicking) return int.MAX;
		return m_ThresholdTime * 1000 - GetGame().GetWorld().GetTimestamp().DiffMilliseconds(m_ThresholdTimeStarted);
		//return GetGame().GetCallqueue().GetRemainingTime(EndVoteThresholdTimer);
	};
	
	void EndVoteThresholdTimer() {
		EndVote();
	};
	
	
	
	protected void StartUpdateTimer() {
		if (m_UpdateTimerTicking) return;
		GetGame().GetCallqueue().CallLater(UpdateTimer, delay: 5 * 1000, repeat: true);
		UpdateTimer();
	};
	
	protected void StopUpdateTimer() {
		if (!m_UpdateTimerTicking) return;
		GetGame().GetCallqueue().Remove(UpdateTimer);
		UpdateTimer();
	};
	
	protected bool IsUpdateTimerTicking() {
		return m_UpdateTimerTicking;
	};
	
	protected void UpdateTimer() {
		Rpc(RpcDo_UpdateTimer, GetRemainingTime());
	};
	
	[RplRpc(channel: RplChannel.Unreliable, rcver: RplRcver.Broadcast)]
	protected void RpcDo_UpdateTimer(int timeLeft) {
		m_TimeLeft = timeLeft;
		m_TimeLeftSync = GetGame().GetWorld().GetTimestamp();
	};
	
	
	
	override protected bool RplSave(ScriptBitWriter writer) {
		if (!super.RplSave(writer)) return false;
		writer.WriteInt(GetRemainingTime());
		return true;
	};
	
	override protected bool RplLoad(ScriptBitReader reader) {
		if (!super.RplLoad(reader)) return false;
		reader.ReadInt(m_TimeLeft);
		m_TimeLeftSync = GetGame().GetWorld().GetTimestamp();
		return true;
	};
};
