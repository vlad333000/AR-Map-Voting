class V30_MapVoting_VoteButtonWidgetComponent : V30_MapVoting_ButtonWidgetComponent {
    /*sealed*/ override protected event void OnButtonClicked(notnull SCR_ButtonBaseComponent buttonWidgetComponent) {
        auto focusedWidget = GetGame().GetWorkspace().GetFocusedWidget();
        if (!focusedWidget)
            return;

        auto choiceUiComponent = V30_MapVoting_ChoiceUIComponent.GetInstance(focusedWidget);
        if (!choiceUiComponent)
            return;

		choiceUiComponent.Vote();
    };
};
