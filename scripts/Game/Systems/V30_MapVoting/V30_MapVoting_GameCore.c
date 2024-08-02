typedef int V30_MapId;

void V30_MapVoting_OnPlayerVoteMapDelegate(int playerId, int mapId);
typedef func V30_MapVoting_OnPlayerVoteMapDelegate;
typedef ScriptInvokerBase<V30_MapVoting_OnPlayerVoteMapDelegate> V30_MapVoting_OnPlayerVoteMapInvoker;

[BaseContainerProps()]
class V30_MapVoting_GameCore : SCR_GameCoreBase {
	protected ref map<int, int> m_mVotes;
	
	protected ref array<ref V30_MapVoting_Choice> m_aChoices;
	
	protected ref V30_MapVoting_OnPlayerVoteMapInvoker m_OnPlayerVoteMapInvoker;
	
	override void OnGameStart() {
		if (!Replication.IsServer()) return;
		
		if (!FileIO.FileExists("$profile:V30/Voting/config.json")) return;
		
		super.OnGameStart();
		auto game = GetGame();
		auto gameMode = SCR_BaseGameMode.Cast(game.GetGameMode());
		gameMode.GetOnPlayerConnected().Insert(OnPlayerConnected);
		gameMode.GetOnPlayerDisconnected().Insert(OnPlayerDisconnected);
		gameMode.GetOnGameModeEnd().Insert(OnGameModeEnded);
	};
	
	protected void OnPlayerConnected(int playerId) {
		PrintFormat("[V30_MapVoting_GameCore] OnPlayerConnected(%1)", playerId);
		m_mVotes.Insert(playerId, -1);
	};
	
	protected void OnPlayerDisconnected(int playerId) {
		PrintFormat("[V30_MapVoting_GameCore] OnPlayerDisconnected(%1)", playerId);
		m_mVotes.Remove(playerId);
	};
	
	void Vote(int playerId, int mapId) {
		m_mVotes.Set(playerId, mapId);
		OnPlayerVoteMap(playerId, mapId);
	};
	
	protected void OnPlayerVoteMap(int playerId, int mapId) {
		if (!m_OnPlayerVoteMapInvoker) return;
		m_OnPlayerVoteMapInvoker.Invoke(playerId, mapId);
	};
	
	V30_MapVoting_OnPlayerVoteMapInvoker GetOnPlayerVoteMap() {
		if (!m_OnPlayerVoteMapInvoker) m_OnPlayerVoteMapInvoker = new V30_MapVoting_OnPlayerVoteMapInvoker();
		return m_OnPlayerVoteMapInvoker;
	};
	
	protected void OnGameModeEnded(SCR_GameModeEndData endData) {
		
	};
	
	protected void StartVoting() {
	};
	
	protected void EndVoting() {
	};
};