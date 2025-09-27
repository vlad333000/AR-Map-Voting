class V30_MapVoting_RemoveVoteButtonWidgetComponent : V30_MapVoting_ButtonWidgetComponent {
    /*sealed*/ override void Setup(notnull V30_MapVoting_GameModeComponent votingComponent) {
        super.Setup(votingComponent);
        if (!GetVotingComponent().IsAllowsVoteRemove()) {
            auto widget = GetRootWidget();
            widget.SetVisible(false);
            widget.SetEnabled(false);
        };
    };

    /*sealed*/ override protected event void OnButtonClicked(notnull SCR_ButtonBaseComponent buttonWidgetComponent) {
        if (GetVotingComponent().IsAllowsVoteRemove())
            V30_MapVoting_PlayerControllerComponent.GetLocalInstance().ClearVote();
    };
};
