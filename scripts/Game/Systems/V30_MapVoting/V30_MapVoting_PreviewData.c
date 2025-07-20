class V30_MapVoting_PreviewData {
	protected ResourceName m_Image;

	protected string m_Title;

	protected string m_SubTitle;

	void V30_MapVoting_PreviewData() {
		return;
	};

	static V30_MapVoting_PreviewData Create(ResourceName image = "{31F6670FAB57B6FE}UI/Textures/V30_MapVoting_Choice_UnknownImage.edds", string title = "#AR-V30_MapVoting_UnknownChoiceTitle", string subTitle = "#AR-V30_MapVoting_UnknownChoiceSubTitle") {
		auto data = new V30_MapVoting_PreviewData();
		data.m_Image = image;
		data.m_Title = title;
		data.m_SubTitle = subTitle;
		return data;
	};

	ResourceName GetImage() {
		return m_Image;
	};

	string GetTitle() {
		return m_Title;
	};

	string GetSubTitle() {
		return m_SubTitle;
	};

	static bool Extract(V30_MapVoting_PreviewData instance, ScriptCtx ctx, SSnapSerializerBase snapshot) {
		snapshot.SerializeString(instance.m_Image);
		snapshot.SerializeString(instance.m_Title);
		snapshot.SerializeString(instance.m_SubTitle);
		return true;
	};

	static bool Inject(SSnapSerializerBase snapshot, ScriptCtx ctx, V30_MapVoting_PreviewData instance) {
		snapshot.SerializeString(instance.m_Image);
		snapshot.SerializeString(instance.m_Title);
		snapshot.SerializeString(instance.m_SubTitle);
		return true;
	};

	static void Encode(SSnapSerializerBase snapshot, ScriptCtx ctx, ScriptBitSerializer packet) {
		snapshot.EncodeString(packet); // m_Image
		snapshot.EncodeString(packet); // m_Title
		snapshot.EncodeString(packet); // m_SubTitle
	};

	static bool Decode(ScriptBitSerializer packet, ScriptCtx ctx, SSnapSerializerBase snapshot) {
		snapshot.DecodeString(packet); // m_Image
		snapshot.DecodeString(packet); // m_Title
		snapshot.DecodeString(packet); // m_SubTitle
		return true;
	};

	static bool SnapCompare(SSnapSerializerBase lhs, SSnapSerializerBase rhs, ScriptCtx ctx) {
		return lhs.CompareStringSnapshots(rhs) && // m_Image
			lhs.CompareStringSnapshots(rhs) && // m_Title
			lhs.CompareStringSnapshots(rhs); // m_SubTitle
	};

	static bool PropCompare(V30_MapVoting_PreviewData instance, SSnapSerializerBase snapshot, ScriptCtx ctx) {
		return snapshot.CompareString(instance.m_Image) &&
			snapshot.CompareString(instance.m_Title) &&
			snapshot.CompareString(instance.m_SubTitle);
	};
};
