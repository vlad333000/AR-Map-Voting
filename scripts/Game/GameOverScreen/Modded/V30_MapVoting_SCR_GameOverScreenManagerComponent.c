//! Handles the showing of end screen and has a config with gameover screens
[ComponentEditorProps(category: "GameScripted/GameOver", description: "")]
class V30_MapVoting_GameOverScreenManagerComponentClass : SCR_GameOverScreenManagerComponentClass
{
}

class V30_MapVoting_GameOverScreenManagerComponent : SCR_GameOverScreenManagerComponent {
	override SCR_GameOverScreenUIComponent OpenGameOverScreen(ResourceName contentLayout, SCR_GameModeEndData endData, LocalizedString title = string.Empty, LocalizedString subtitle = string.Empty, LocalizedString debriefing = string.Empty, ResourceName imageTexture = ResourceName.Empty, ResourceName icon = ResourceName.Empty, Color vignetteColor = null, string titleParam = string.Empty, string subtitleParam = string.Empty, string debriefingParam = string.Empty) {
		SCR_HUDManagerComponent hudManager = GetGame().GetHUDManager();
		if (!hudManager)
			return null;
		
		// Fucking idiot!
		//This is just temporary hopefully. This bypasses a lot of the code here and in other files. If one wants to change the layout, do it in Menu Config.
		MenuBase menu = GetGame().GetMenuManager().OpenMenu(ChimeraMenuPreset.V30_MapVoting_EndgameScreen, 0, true);
		
		Widget screen = menu.GetRootWidget();
				
		SCR_GameOverScreenUIComponent screenUIComponent = SCR_GameOverScreenUIComponent.Cast(screen.FindHandler(SCR_GameOverScreenUIComponent));
		
		if (screenUIComponent)
		{
			SCR_GameOverScreenUIContentData content = new SCR_GameOverScreenUIContentData(contentLayout, endData, title, subtitle, debriefing, imageTexture, icon, vignetteColor, titleParam, subtitleParam, debriefingParam);
			screenUIComponent.InitGameOverScreen(content);
		}

		return screenUIComponent;
	};
};