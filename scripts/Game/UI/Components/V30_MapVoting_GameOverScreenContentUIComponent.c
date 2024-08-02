class V30_MapVoting_GameOverScreenContentUIComponent : SCR_GameOverScreenContentUIComponent {
	override void InitContent(SCR_GameOverScreenUIContentData endScreenUIContent) {
		auto preview = V30_MapVoting_GameModeComponent.GetInstance().GetWinnerPreview();
		endScreenUIContent.m_sImageTexture = preview.GetImage();
		endScreenUIContent.m_sTitle = preview.GetTitle();
		endScreenUIContent.m_sSubtitle = preview.GetSubTitle();
		super.InitContent(endScreenUIContent);
		auto w = ImageWidget.Cast(m_wRoot.FindAnyWidget(m_sImageName));
		if (!w) return;
		int x, y;
		w.GetImageSize(0, x, y);
		w.SetSize(x, y);
	};
};