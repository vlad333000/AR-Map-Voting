modded class SCR_DebriefingScreenMenu {
    override void OnMenuOpen() {
        super.OnMenuOpen();

        Widget pauseButton = GetRootWidget().FindAnyWidget("PauseButton");
		if (pauseButton)
		{
			auto pauseButtonComponent = SCR_InputButtonComponent.Cast(pauseButton.FindHandler(SCR_InputButtonComponent));
			if (pauseButtonComponent) {
				pauseButtonComponent.SetLabel("#AR-Menu_Back");
                pauseButtonComponent.SetAction("MenuBack");
            };
		};

		Widget closeButton = GetRootWidget().FindAnyWidget("CloseButton");
		if (closeButton) {
			auto closeButtonComponent = SCR_InputButtonComponent.Cast(closeButton.FindHandler(SCR_InputButtonComponent));
			if (closeButtonComponent) {
                closeButtonComponent.SetLabel("#AR-DeployMenu_PauseButton");
                closeButtonComponent.SetAction("MenuSelect");
			};
		};
    };

	override protected void OpenPauseMenu() {
		Close();
	};

    override void ReturnToMenu() {
		MenuBase menu = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.PauseMenu, 0, true, false);

		PauseMenuUI pauseMenu = PauseMenuUI.Cast(menu);
		if (pauseMenu) {
			pauseMenu.FadeBackground(true, true);
			pauseMenu.DisableGameMaster();
			pauseMenu.DisableArmaVision();
		};
	};
};
