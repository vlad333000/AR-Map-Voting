enum V30_MapVoting_State {
    INITIAL,
    CREATED,
    SCHEDULED,
    STARTED,
    ENDED,
    CANCELLED
};

class V30_MapVoting_Vote {
    protected V30_MapVoting_State state;

    protected ref array<ref V30_MapVoting_Choice> choices;

    protected ref set<int> playersWithVoteAbility;

    protected ref map<int, int> playerVotes;

    protected int winnerChoiceId;

    void V30_MapVoting_Vote(array<ref V30_MapVoting_Choice> choices = null) {
        this.state = V30_MapVoting_State.CREATED;
        this.choices = new array<ref V30_MapVoting_Choice>();
        if (choices) {
            this.choices.Reserve(choices.Count());
            foreach (auto choice : choices)
                this.choices.Insert(choice);
        };
        this.playersWithVoteAbility = new set<int>();
        this.playerVotes = new map<int, int>();
        this.winnerChoiceId = -1;
    };



    V30_MapVoting_State GetState() {
        return this.state;
    };

    bool IsCreated() {
        return this.state == V30_MapVoting_State.CREATED;
    };

    bool IsScheduled() {
        return this.state == V30_MapVoting_State.SCHEDULED;
    };

    bool IsStarted() {
        return this.state == V30_MapVoting_State.STARTED;
    };

    bool IsEnded() {
        return this.state == V30_MapVoting_State.ENDED;
    };

    bool IsCancelled() {
        return this.state == V30_MapVoting_State.CANCELLED;
    };

    bool IsEvaluated() {
        return IsScheduled() || IsStarted() || IsEnded() || IsCancelled();
    };

    bool IsInProgress() {
        return IsScheduled() || IsStarted();
    };

    bool IsFinished() {
        return IsEnded() || IsCancelled();
    };

    void Schedule() {
        this.state = V30_MapVoting_State.SCHEDULED;
    };

    void Start() {
        this.state = V30_MapVoting_State.STARTED;
    };

    int End() {
        auto winnerCandidates = new array<int>();
        GetWinnerCandidates(winnerCandidates);
        auto winnerChoiceId = winnerCandidates.GetRandomElement();
        End(winnerChoiceId);
        return winnerChoiceId;
    };

    void End(int winnerChoiceId) {
        this.winnerChoiceId = winnerChoiceId;
        this.state = V30_MapVoting_State.ENDED;
    };

    void Cancel() {
        this.winnerChoiceId = -1;
        this.state = V30_MapVoting_State.CANCELLED;
    };



    int AddChoice(V30_MapVoting_Choice choice) {
        return this.choices.Insert(choice);
    };

    void RemoveChoice(int choiceId) {
        this.choices.Remove(choiceId);
    };

    V30_MapVoting_Choice GetChoice(int choiceId) {
        return this.choices.Get(choiceId);
    };

    int CountChoices() {
        return this.choices.Count();
    };

    void GetAllChoices(notnull array<ref V30_MapVoting_Choice> outChoices) {
        foreach (auto choice : this.choices)
            outChoices.Insert(choice);
    };



    void AddPlayerWithVoteAbility(int playerId) {
        this.playersWithVoteAbility.Insert(playerId);
        this.playerVotes.Insert(playerId, -1);
    };

    void RemovePlayerWithVoteAbility(int playerId) {
        this.playersWithVoteAbility.Remove(playerId);
        RemovePlayerVote(playerId);
        this.playerVotes.Remove(playerId);
    };

    bool IsPlayerHasVoteAbility(int playerId) {
        return this.playersWithVoteAbility.Contains(playerId);
    };

    int CountPlayersWithVoteAbility() {
        return this.playersWithVoteAbility.Count();
    };

    void GetAllPlayersWithVoteAbility(notnull array<int> outPlayerIds) {
        foreach (auto playerId : this.playersWithVoteAbility)
            outPlayerIds.Insert(playerId);
    };



    bool AddPlayerVote(int playerId, int choiceId) {
        return this.playerVotes.Insert(playerId, choiceId);
    };

    void SetPlayerVote(int playerId, int choiceId) {
        this.playerVotes.Set(playerId, choiceId);
    };

    void RemovePlayerVote(int playerId) {
        this.playerVotes.Remove(playerId);
    };

    bool IsPlayerVoted(int playerId) {
        return this.playerVotes.Contains(playerId);
    };

    int GetPlayerVote(int playerId) {
        int choiceId;
        if (!this.playerVotes.Find(playerId, choiceId))
            return -1;
        return choiceId;
    };

    int CountVotedPlayers() {
        return this.playerVotes.Count();
    };

    void GetVotedPlayers(notnull array<int> outPlayerIds) {
        foreach (auto playerId, auto choiceId : this.playerVotes)
            outPlayerIds.Insert(playerId);
    };

    void GetChoiceVotedPlayers(int choiceId, notnull array<int> outPlayerIds) {
        foreach (auto playerId, auto playerChoiceId : this.playerVotes)
            if (playerChoiceId == choiceId)
                outPlayerIds.Insert(playerId);
    };

    void GetAllPlayerVotes(notnull map<int, int> outPlayerVotes) {
        foreach (auto playerId, auto choiceId : this.playerVotes)
            outPlayerVotes.Set(playerId, choiceId);
    };



    bool HasWinner() {
        return this.winnerChoiceId != -1;
    };

    int GetWinnerChoiceId() {
        return this.winnerChoiceId;
    };

    V30_MapVoting_Choice GetWinnerChoice() {
        return this.choices.Get(this.winnerChoiceId);
    };



    int CountChoiceVotedPlayers(int choiceId) {
        int count = 0;
        foreach (auto playerId, auto playerChoiceId : this.playerVotes)
            if (playerChoiceId == choiceId)
                count++;
        return count;
    };

    void GetWinnerCandidates(notnull array<int> outChoiceIds) {
        int maxVotes = 0;
        foreach (auto choiceId, auto choice : this.choices) {
            int votesCount = CountChoiceVotedPlayers(choiceId);
            if (votesCount == maxVotes) {
                outChoiceIds.Insert(choiceId);
            }
            else if (votesCount > maxVotes) {
                outChoiceIds.Clear();
                outChoiceIds.Insert(choiceId);
                maxVotes = votesCount;
            };
        };
    };
};

class V30_MapVoting_VotingWorldSystem : WorldSystem {
    protected ref array<ref V30_MapVoting_Vote> votes;
	
    protected V30_MapVoting_State state;

    protected int currentVoteIndex = 0;

    protected int finalVoteIndex = -1;

    protected ref array<ref V30_MapVoting_Choice> choices;
	
	protected int winnerChoiceId;
	
	protected ref array<int> playersWithVoteAbility;
	
	protected ref map<int, int> playerVotes;



    override static void InitInfo(WorldSystemInfo outInfo) {
        super.InitInfo(outInfo);
        outInfo.SetAbstract(false);
        outInfo.SetUnique(true);
        outInfo.SetLocation(WorldSystemLocation.Server);
    };

    static V30_MapVoting_VotingWorldSystem GetInstance() {
        return GetWorldInstance(GetGame().GetWorld());
    };

    static V30_MapVoting_VotingWorldSystem GetWorldInstance(notnull World world) {
        return V30_MapVoting_VotingWorldSystem.Cast(world.FindSystem(V30_MapVoting_VotingWorldSystem));
    };



    [EventAttribute()]
    void OnVoteScheduled(notnull V30_MapVoting_Vote vote, int order);

    [EventAttribute()]
    void OnVoteStarted(notnull V30_MapVoting_Vote vote);

    [EventAttribute()]
    void OnVoteEnded(notnull V30_MapVoting_Vote vote, int winnerChoiceId);

    [EventAttribute()]
    void OnVoteCancelled(notnull V30_MapVoting_Vote vote);

    [EventAttribute()]
    void OnPlayerVoteAbilityChanged(notnull V30_MapVoting_Vote vote, int playerId, bool hasVoteAbility);

    [EventAttribute()]
    void OnPlayerVoteChanged(notnull V30_MapVoting_Vote vote, int playerId, int choiceId, int oldChoiceId);



    void ScheduleVote(notnull V30_MapVoting_Vote vote) {
        auto index = this.votes.Insert(vote);
        auto order = index - currentVoteIndex;
        PrintFormat("[V30][MapVoting] Vote <%1> scheduled in %2.", vote, order);
        ThrowEvent(OnVoteScheduled, vote, order);

        if (order == 0)
            StartVote();
    };

    bool IsVoteScheduled(notnull V30_MapVoting_Vote vote) {
        return this.votes.Find(vote) > currentVoteIndex;
    };

    bool IsVoteScheduledNext(notnull V30_MapVoting_Vote vote) {
        return this.votes.Find(vote) == currentVoteIndex + 1;
    };

    int GetVoteScheduleOrder(notnull V30_MapVoting_Vote vote) {
        auto index = this.votes.Find(vote);
        if (index == -1 || index <= currentVoteIndex)
            return -1;
        return index - currentVoteIndex - 1;
    };

    int CountScheduledVotes() {
        return this.votes.Count() - currentVoteIndex;
    };



    // Begins vote (e.g. allows player to vote)
    void StartVote() {
        //if (IsVoteStarted()) {
        //    PrintFormat("[V30][MapVoting] Vote <%1> is already started.", GetCurrentVote());
        //    return;
        //};
        //auto vote = GetCurrentVote();
        //vote.Start();
        //PrintFormat("[V30][MapVoting] Vote <%1> started.", vote);
    };

    void StartVote(notnull V30_MapVoting_Vote vote) {
        // Cancel all scheduled votes before this vote
        for (; currentVoteIndex < this.votes.Count(); currentVoteIndex++) {
            auto v = this.votes.Get(currentVoteIndex);
            if (v == vote)
                break;
            if (!v.IsFinished())
                v.Cancel();
        };
    };

    // End vote and choice winner acording to the votes.
    void EndVote() {
        //if (IsVoteEnded()) {
        //    Debug.Error("[V30][MapVoting] Vote is already ended.");
        //    return;
        //};
        //if (!IsVoteInProgress()) {
        //    Debug.Error("[V30][MapVoting] Vote is not in progress.");
        //    return;
        //};
        //auto winnerCandidates = new array<int>();
        //GetWinnerCandidates(winnerCandidates);
        //this.winnerChoiceId = winnerCandidates.GetRandomElement();
        //this.state = V30_MapVoting_State.ENDED;
        //PrintFormat("[V30][MapVoting] Vote ended.");
        //PrintFormat("\tVotes:");
        //foreach (auto choiceId, auto choice : choices) {
        //    int votesCount = CountChoiceVotedPlayers(choiceId);
        //    PrintFormat("\t\t%3 -> %1 (%2)", choiceId, choice, votesCount);
        //};
        //PrintFormat("\tWinner: %1 (%2).", winnerChoiceId, GetChoice(winnerChoiceId));
        //ThrowEvent(OnVoteEnded, winnerChoiceId);
    };

    // End vote with specified winner choice without counting votes.
    void EndVote(int winnerChoiceId) {
        //if (IsVoteEnded()) {
        //    Debug.Error("[V30][MapVoting] Vote is already ended.");
        //    return;
        //};
        //if (!IsVoteInProgress()) {
        //    Debug.Error("[V30][MapVoting] Vote is not in progress.");
        //    return;
        //};
        //this.winnerChoiceId = winnerChoiceId;
        //this.state = V30_MapVoting_State.ENDED;
        //PrintFormat("[V30][MapVoting] Vote ended.");
        //PrintFormat("\tWinner: %1 (%2).", winnerChoiceId, GetChoice(winnerChoiceId));
        //ThrowEvent(OnVoteEnded, winnerChoiceId);
    };

    // Cancels vote without any winner.
    void CancelVote() {
        //if (IsVoteEnded()) {
        //    Debug.Error("[V30][MapVoting] Vote is already ended.");
        //    return;
        //};
        //if (!IsVoteInProgress()) {
        //    Debug.Error("[V30][MapVoting] Vote is not in progress.");
        //    return;
        //};
        //this.winnerChoiceId = -1;
        //this.state = V30_MapVoting_State.CANCELLED;
        //PrintFormat("[V30][MapVoting] Vote cancelled.");
        //ThrowEvent(OnVoteCancelled);
    };

    // Returns true if vote is started.
    bool IsVoteStarted() {
        return state >= V30_MapVoting_State.STARTED;
    };

    // Returns true if vote is in progress (e.g. started but not ended).
    bool IsVoteInProgress() {
        return state == V30_MapVoting_State.STARTED;
    };

    // Returns true if vote is ended or canceled.
    bool IsVoteEnded() {
        return state == V30_MapVoting_State.ENDED || state == V30_MapVoting_State.CANCELLED;
    };

    // Returns true if vote is ended and has winner (e.g. not cancelled).
    bool IsVoteSuccessful() {
        return state == V30_MapVoting_State.ENDED;
    };

    // Returns true if vote is cancelled.
    bool IsVoteCancelled() {
        return state == V30_MapVoting_State.CANCELLED;
    };



    // Returns number of choices in current vote.
    int CountChoices() {
        return choices.Count();
    };

    // Returns choice by its id.
    V30_MapVoting_Choice GetChoice(int choiceId) {
        return choices.Get(choiceId);
    };

    // Returns all choices in current vote.
    void GetAllChoices(notnull array<V30_MapVoting_Choice> outChoices) {
        foreach (auto choice : this.choices) {
			outChoices.Insert(choice);
		};
    };



    // Returns choices with the most votes.
    void GetWinnerCandidates(notnull array<int> outChoiceIds) {
        if (!IsVoteStarted()) {
            Debug.Error("[V30][MapVoting] Vote is not started yet.");
            return;
        };
        int maxVotes = 0;
        foreach (auto choiceId, auto choice : choices) {
            int votesCount = CountChoiceVotedPlayers(choiceId);
            if (votesCount == maxVotes) {
                outChoiceIds.Insert(choiceId);
            }
            else if (votesCount > maxVotes) {
                outChoiceIds.Clear();
                outChoiceIds.Insert(choiceId);
                maxVotes = votesCount;
            };
        };
    };

    // Returns winner choice id of ended vote.
    int GetWinner() {
        if (!IsVoteSuccessful()) {
            Debug.Error("[V30][MapVoting] Vote is not successful.");
            return -1;
        };
        return winnerChoiceId;
    };



    // Allows player to vote.
    void AddPlayerVoteAbility(int playerId) {
        playersWithVoteAbility.Insert(playerId);
        playerVotes.Insert(playerId, -1);
    };

    // Removes player's ability to vote. If player has already voted, his vote will be removed.
    void RemovePlayerVoteAbility(int playerId) {
        playersWithVoteAbility.Remove(playerId);
        RemovePlayerVote(playerId);
        playerVotes.Remove(playerId);
    };

    // Returns true if player has ability to vote.
    bool IsPlayerHasVoteAbility(int playerId) {
        return playersWithVoteAbility.Contains(playerId);
    };

    // Returns number of players with vote ability.
    int CountPlayersWithVoteAbility() {
        return playersWithVoteAbility.Count();
    };

    // Returns ids of players with vote ability.
    void GetAllPlayersWithVoteAbility(notnull array<int> outPlayerIds) {
        foreach (auto playerId : playersWithVoteAbility) {
            outPlayerIds.Insert(playerId);
        };
    };



    void SetPlayerVote(int playerId, int choiceId) {
        playerVotes.Set(playerId, choiceId);
    };

    void RemovePlayerVote(int playerId) {
        playerVotes.Set(playerId, -1);
    };

    int GetPlayerVote(int playerId) {
        return playerVotes.Get(playerId);
    };

    bool IsPlayerVoted(int playerId) {
        return playerVotes.Get(playerId) != -1;
    };

    int CountVotedPlayers() {
        int count = 0;
        foreach (auto playerId, auto choiceId : playerVotes) {
            if (choiceId != -1) {
                count++;
            };
        };
        return count;
    };

    void GetVotedPlayers(notnull array<int> outPlayerIds) {
        foreach (auto playerId, auto choiceId : playerVotes) {
            if (choiceId != -1) {
                outPlayerIds.Insert(playerId);
            };
        };
    };



    int CountChoiceVotedPlayers(int choiceId) {
        int count = 0;
        foreach (auto playerId, auto playerChoiceId : playerVotes) {
            if (playerChoiceId == choiceId) {
                count++;
            };
        };
        return count;
    };

    void GetChoiceVotedPlayers(int choiceId, notnull array<int> outPlayerIds) {
        foreach (auto playerId, auto playerChoiceId : playerVotes) {
            if (playerChoiceId == choiceId) {
                outPlayerIds.Insert(playerId);
            };
        };
    };
};


class V30_MapVoting_VoteSchedulerWorldSystem : WorldSystem {
    protected ref array<ref V30_MapVoting_Vote> votesQueue;

    int ScheduleVote(notnull V30_MapVoting_Vote vote);

    bool IsVoteScheduled(notnull V30_MapVoting_Vote vote);

    void UnscheduleVote(notnull V30_MapVoting_Vote vote);

    int CountQueue();



    [EventAttribute()]
    void OnVoteScheduled(notnull V30_MapVoting_Vote vote, int order);

    [EventAttribute()]
    void OnVoteStarted(notnull V30_MapVoting_Vote vote);

    [EventAttribute()]
    void OnVoteEnded(notnull V30_MapVoting_Vote vote, int winnerChoiceId);

    [EventAttribute()]
    void OnVoteCancelled(notnull V30_MapVoting_Vote vote);
};
