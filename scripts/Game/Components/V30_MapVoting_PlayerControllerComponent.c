[ComponentEditorProps(category: "GameScripted/Misc", description: "")]
class V30_MapVoting_PlayerControllerComponentClass : ScriptComponentClass {
}

class V30_MapVoting_PlayerControllerComponent : ScriptComponent {
	protected V30_MapVoting_ChoiceId m_ChoiceId;
	
	protected ref ScriptInvoker m_OnVoteChanged;
	
	void V30_MapVoting_PlayerControllerComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {
		// TODO: Add check for adding to PlayerController
		m_ChoiceId = V30_MapVoting_NoChoice;
		m_OnVoteChanged = new ScriptInvoker();
		if (!GetGame().InPlayMode()) return;
		SetEventMask(ent, EntityEvent.INIT);
	};
	
	override void EOnInit(IEntity owner) {
		super.EOnInit(owner);
	};
	
	void SetVote(V30_MapVoting_ChoiceId choiceId) {
		if (choiceId == m_ChoiceId) return;
		auto oldChoiceId = m_ChoiceId;
		m_ChoiceId = choiceId;
		SendVote();
		OnVoteChanged(m_ChoiceId, oldChoiceId);
	};
	
	void EndVote() {
		if (!SCR_Global.IsAdmin()) return;
		Rpc(RpcAsk_EndVote);
	};
	
	[RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Server)]
	protected void RpcAsk_EndVote() {
		if (!SCR_Global.IsAdmin(PlayerController.Cast(GetOwner()).GetPlayerId())) return;
		V30_MapVoting_GameModeComponent.GetInstance().EndVote();
	};
	
	void ClearVote() {
		SetVote(V30_MapVoting_NoChoice);
	};
	
	V30_MapVoting_ChoiceId GetVote() {
		return m_ChoiceId;
	};
	
	bool IsVoted() {
		return m_ChoiceId != V30_MapVoting_NoChoice;
	};
	
	protected void SendVote() {
		Rpc(RpcAsk_SendVote, m_ChoiceId);
	};
	
	protected void OnVoteChanged(V30_MapVoting_ChoiceId choiceId, V30_MapVoting_ChoiceId oldChoiceId) {
		PrintFormat("%1.OnVoteChanged(%2, %3)", this, choiceId, oldChoiceId);
		m_OnVoteChanged.Invoke(this, choiceId, oldChoiceId);
	};
	
	ScriptInvoker GetOnVoteChanged() {
		return m_OnVoteChanged;
	};
	
	[RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Server)]
	protected void RpcAsk_SendVote(V30_MapVoting_ChoiceId choiceId) {
		V30_MapVoting_GameModeComponent.GetInstance().SetPlayerChoice(PlayerController.Cast(GetOwner()).GetPlayerId(), choiceId);
	};
	
	static V30_MapVoting_PlayerControllerComponent GetInstance() {
		auto game = GetGame();
		if (!game) return null;
		auto playerController = game.GetPlayerController();
		if (!playerController) return null;
		return V30_MapVoting_PlayerControllerComponent.Cast(playerController.FindComponent(V30_MapVoting_PlayerControllerComponent));
	};
};
