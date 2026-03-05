[BaseContainerProps()]
class V30_MapVoting_FileExternalRunner : V30_MapVoting_ExternalRunner {
    [Attribute("$profile:V30/MapVoting/next.json")]
    protected string filePath;

    [Attribute("0", desc: "Format to use for file.", uiwidget: UIWidgets.ComboBox, enumType: V30_MapVoting_FileExternalRunner_EFormat)]
    protected V30_MapVoting_FileExternalRunner_EFormat format;

    override void PrepareScenarioSwitch() {
        auto filePathSanised = SCR_FileIOHelper.SanitiseFileName(filePath);
        if (filePathSanised != filePath) {
            PrintFormat("[V30][MapVoting][FileExternalRunner] Sanitised file path from \"%1\" to \"%2\".", filePath, filePathSanised, level: LogLevel.WARNING);
        };

        auto folderPath = FilePath.StripFileName(filePathSanised);
        if (!FileIO.MakeDirectory(folderPath)) {
            PrintFormat("[V30][MapVoting][FileExternalRunner] Failed to create folder \"%1\".", filePathSanised, level: LogLevel.ERROR);
        };

        auto file = FileIO.OpenFile(filePathSanised, FileMode.WRITE);
        if (!file) {
            PrintFormat("[V30][MapVoting][FileExternalRunner] Failed to open file \"%1\" for write.", filePathSanised, level: LogLevel.FATAL);
            return;
        };

        string data;
        switch (format) {
            case V30_MapVoting_FileExternalRunner_EFormat.JSON : {
                data = GetJsonString();
                break;
            };
            case V30_MapVoting_FileExternalRunner_EFormat.XML : {
                data = GetXmlString();
                break;
            };
            case V30_MapVoting_FileExternalRunner_EFormat.PlainText : {
                data = GetPlainTextString();
                break;
            };
            default : {
                auto formatName = SCR_Enum.GetEnumName(V30_MapVoting_FileExternalRunner_EFormat, format);
                PrintFormat("[V30][MapVoting][FileExternalRunner] PrepareScenarioSwitch: unsupported format %1 (%2)", format, formatName, level: LogLevel.ERROR);
                break;
            };
        };

        auto bytesWritten = file.Write(data);
        if (bytesWritten != data.Length()) {
            PrintFormat("[V30][MapVoting][FileExternalRunner] %2 bytes successfully written to file \"%1\", but %3 expected.", filePathSanised, bytesWritten, data.Length(), level: LogLevel.ERROR);
        };
        file.Close();
    };
};

enum V30_MapVoting_FileExternalRunner_EFormat {
    JSON,
    XML,
	PlainText
    // TODO: AUTO = -1 // Automatically determine format based on extension.
}
