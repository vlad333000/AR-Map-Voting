void V30_MapVoting_PreGameGameModeStateComponent_OnMessageUpdated(notnull V30_MapVoting_PreGameGameModeStateComponent preGameGameModeStateComponent, string message);
typedef func V30_MapVoting_PreGameGameModeStateComponent_OnMessageUpdated;
typedef ScriptInvokerBase<V30_MapVoting_PreGameGameModeStateComponent_OnMessageUpdated> V30_MapVoting_PreGameGameModeStateComponent_OnMessageUpdatedInvoker;

class V30_MapVoting_PreGameGameModeStateComponentClass : SCR_BaseGameModeStateComponentClass {};

class V30_MapVoting_PreGameGameModeStateComponent : SCR_BaseGameModeStateComponent {
    protected /*private*/ ref V30_MapVoting_PreGameGameModeStateComponent_OnMessageUpdatedInvoker m_OnMessageUpdated;

    protected /*private*/ V30_MapVoting_VotingComponent m_VotingComponent;

    protected /*private*/ bool m_Finished;

    protected /*private*/ ref ScriptInvoker m_OnFinished;



    /*modded*/ string GetMessage();

    protected /*sealed*/ void OnMessageUpdated(string message) {
        m_OnMessageUpdated.Invoke(this, message);
    };

    /*sealed*/ V30_MapVoting_PreGameGameModeStateComponent_OnMessageUpdatedInvoker GetOnMessageUpdated() {
        return m_OnMessageUpdated;
    };

    /*sealed*/ V30_MapVoting_VotingComponent GetVotingComponent() {
        return m_VotingComponent;
    };



    /*sealed*/ void Finish() {
        Rpc(RpcDo_Finish);
        RpcDo_Finish();
    };

    /*saeled*/ bool IsFinished() {
        return m_Finished;
    };

    [RplRpc(channel: RplChannel.Reliable, rcver: RplRcver.Broadcast)]
    protected /*private*/ void RpcDo_Finish() {
        m_Finished = true;
        OnFinished();
    };

    protected /*private*/ event void OnFinished() {
        m_OnFinished.Invoke(this);
    };

    /*sealed*/ ScriptInvoker GetOnFinished() {
        return m_OnFinished;
    };



	override /*sealed*/ SCR_EGameModeState GetAffiliatedState() {
		return SCR_EGameModeState.PREGAME;
	};

    override /*sealed*/ bool CanAdvanceState(SCR_EGameModeState nextState) {
        return super.CanAdvanceState(nextState) && m_Finished;
    };



    void V30_MapVoting_PreGameGameModeStateComponent(IEntityComponentSource src, IEntity ent, IEntity parent) {
        m_OnMessageUpdated = new V30_MapVoting_PreGameGameModeStateComponent_OnMessageUpdatedInvoker();
        m_OnFinished = new ScriptInvoker();
    };

    override protected event void OnPostInit(IEntity owner) {
        super.OnPostInit(owner);
        SetEventMask(owner, EntityEvent.INIT);
    };

    override protected event void EOnInit(IEntity owner) {
        super.EOnInit(owner);
        m_VotingComponent = V30_MapVoting_ComponentHelper<V30_MapVoting_VotingComponent>.FindComponent(owner);
    };



    override protected event bool RplSave(ScriptBitWriter writer) {
        if (!super.RplSave(writer))
            return false;
        writer.WriteBool(m_Finished);
        return true;
    };

    override protected event bool RplLoad(ScriptBitReader reader) {
        if (!super.RplLoad(reader))
            return false;
        if (!reader.ReadBool(m_Finished))
            return false;
        if (m_Finished)
            OnFinished();
        return true;
    };
};
