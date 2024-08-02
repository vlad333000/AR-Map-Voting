[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class V30_MapVoting_AutoGameModeComponentClass : V30_MapVoting_GameModeComponentClass {
};

class V30_MapVoting_AutoGameModeComponent : V30_MapVoting_GameModeComponent {
	// [Attribute("true")]
	// protected bool m_TimeLimitEnable;
	
	[Attribute("300", desc: "Total time limit for viting (in seconds)", params: "0 inf", uiwidget: "editbox")]
	protected int m_TimeLimit;
	
	protected const float m_TimeForRun = 14.0;
	
	// [Attribute("30", params: "0 inf", category: "Threshold Time")]
	// protected int m_ThresholdTime;
	
	// [Attribute("true", category: "Threshold Time")]
	// protected bool m_VoteThresholdEnable;
	
	// [Attribute("0.5", uiwidget: "slider", params: "0.0 1.0", precision: 2, category: "Threshold Time")]
	// protected float m_VoteThreshold;
	
	// [Attribute("false", category: "Threshold Time")]
	// protected bool m_MinVoteCountEnable;
	
	// [Attribute("0", params: "0 inf", category: "Threshold Time")]
	// protected int m_MinVoteCount;
	
	
	protected bool m_TimeLeftTicking;
	
	protected WorldTimestamp m_TimeLeftSync;
	
	protected int m_TimeLeft;
	
	
	override void ParseConfig(notnull V30_Json_object config) {
		super.ParseConfig(config);
		
		auto time = config.GetAt("time");
		if (!time) {
			return;
		}
		else if (time.IsFloat()) {
			m_TimeLimit = time.AsFloat().Get();
		}
		else if (time.IsInt()) {
			m_TimeLimit = time.AsInt().Get();
		}
		else {
			PrintFormat("%1.ParseConfig(...) Error: 'time' is not a number! %2", this, time, level: LogLevel.ERROR);
		};
	};
	
	override void OnGameModeStart() {
		GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.V30_MapVoting_Menu);
		if (m_RplComponent.Role() != RplRole.Authority) return;
		m_TimeLeftTicking = false;
		StartVote();
	};
	
	protected void StartTimer() {
		if (m_TimeLeftTicking) return;
		GetGame().GetCallqueue().CallLater(EndGameMode, delay: m_TimeLimit * 1000);
		GetGame().GetCallqueue().CallLater(UpdateTimer, delay: 5 * 1000, repeat: true);
		m_TimeLeftTicking = true;
	};
	
	protected void EndTimer() {
		if (!m_TimeLeftTicking) return;
		GetGame().GetCallqueue().Remove(EndGameMode);
		GetGame().GetCallqueue().Remove(UpdateTimer);
		m_TimeLeftTicking = false;
	};
	
	override void OnVoteStarted() {
		super.OnVoteStarted();
		if (m_RplComponent.Role() != RplRole.Authority) return;
		if (GetGame().GetPlayerManager().GetPlayerCount() == 0) return;
		StartTimer();
	};
	
	protected void EndGameMode() {
		GetGameMode().EndGameMode(SCR_GameModeEndData.CreateSimple(EGameOverTypes.V30_MAPVOTING_NEXT_MAP));
	};
	
	override void OnGameModeEnd(SCR_GameModeEndData data) {
		if (m_RplComponent.Role() != RplRole.Authority) return;
		if (!IsVoteStarted()) return;
		EndVote();
	};
	
	override void OnVoteEnded(V30_MapVoting_ChoiceId winnerId) {
		super.OnVoteEnded(winnerId);
		if (m_RplComponent.Role() != RplRole.Authority) return;
		GetGame().GetCallqueue().Remove(UpdateTimer);
		GetGame().GetCallqueue().CallLater(RunWinner, delay: m_TimeForRun * 1000);
	};
	
	override protected void OnPlayerConnected(int playerId) {
		super.OnPlayerConnected(playerId);
		if (!IsVoteStarted()) return;
		StartTimer();
	};
	override protected void OnPlayerDisconnected(int playerId, KickCauseCode cause, int timeout) {
		super.OnPlayerDisconnected(playerId, cause, timeout);
		if (!IsVoteStarted()) return;
		if (GetGame().GetPlayerManager().GetPlayerCount() > 1) return;
		EndTimer();
	};
	
	
	
	override protected bool RplSave(ScriptBitWriter writer) {
		if (!super.RplSave(writer)) return false;
		if (GetGame().GetPlayerManager().GetPlayerCount() < 1)
			writer.WriteInt(GetTimeLimit() * 1000);
		else
			writer.WriteInt(GetRemainingTime());
		return true;
	};
	
	override protected bool RplLoad(ScriptBitReader reader) {
		if (!super.RplLoad(reader)) return false;
		reader.ReadInt(m_TimeLeft);
		m_TimeLeftSync = GetGame().GetWorld().GetTimestamp();
		return true;
	};
	
	protected void UpdateTimer() {
		Rpc(RpcDo_UpdateTimer, GetGame().GetCallqueue().GetRemainingTime(EndGameMode));
	};
	
	[RplRpc(channel: RplChannel.Unreliable, rcver: RplRcver.Broadcast)]
	protected void RpcDo_UpdateTimer(int timeLeft) {
		m_TimeLeft = timeLeft;
		m_TimeLeftSync = GetGame().GetWorld().GetTimestamp();
	};
	
	
	int GetTimeLimit() {
		return m_TimeLimit;
	};
	
	int GetRemainingTime() {
		if (m_RplComponent.Role() == RplRole.Authority) {
			return GetGame().GetCallqueue().GetRemainingTime(EndGameMode);
		}
		else {
			return m_TimeLeft - GetGame().GetWorld().GetTimestamp().DiffMilliseconds(m_TimeLeftSync);
		};
	};
};
