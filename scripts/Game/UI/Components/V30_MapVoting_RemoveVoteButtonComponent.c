class V30_MapVoting_RemoveVoteButtonWidgetComponent : V30_MapVoting_ButtonWidgetComponent {
    /*sealed*/ override void Setup(notnull V30_MapVoting_VotingWorldSystem votingWorldSystem) {
        super.Setup(votingWorldSystem);
    };

    /*sealed*/ override protected event void OnButtonClicked(notnull SCR_ButtonBaseComponent buttonWidgetComponent) {
		return;
    };
};
