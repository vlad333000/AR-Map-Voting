[BaseContainerProps()]
class V30_MapVoting_FileExternalRunner : V30_MapVoting_ExternalRunner {
    [Attribute("$profile:V30/MapVoting/next.json")]
    protected string filePath;

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

        auto jsonString = GetJsonString();
        auto bytesWritten = file.Write(jsonString);
        if (bytesWritten != jsonString.Length()) {
            PrintFormat("[V30][MapVoting][FileExternalRunner] %2 bytes successfully written to file \"%1\", but %3 expected.", filePathSanised, bytesWritten, jsonString.Length(), level: LogLevel.ERROR);
        };
        file.Close();
    };
};
