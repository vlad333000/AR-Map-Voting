[BaseContainerProps()]
sealed class V30_MapVoting_RestApiExternalRunner : V30_MapVoting_ExternalRunner {
    [Attribute("http://localhost:8080")]
    protected string host;

    [Attribute("/scenario")]
    protected string endpoint;

    [Attribute("$profile:V30/MapVoting/token.txt")]
    protected string bearerTokenFilePath;

    protected RestApi restApi;

    protected ref RestCallback restCallback;

    private string restHeaders;

    protected string restHeadersPublic;

    protected bool switchRequestSended;

    protected bool switchScenarioCalled;

    override void PrepareScenarioSwitch() {
        this.restApi = GetGame().GetRestApi();
        if (!this.restApi) {
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] PrepareScenarioSwitch(): Failed to get `RestApi` instance.", level: LogLevel.ERROR);
            return;
        };

        auto headers = "Content-Type,application/json";
        auto headersSecret = "Content-Type,application/json";

        auto token = GetToken();
        if (!token.IsEmpty()) {
            headers += ",Authorization,Bearer " + token;
            headersSecret += ",Authorization,Bearer ***";
        };

        this.restHeaders = headers;
        this.restHeadersPublic = headersSecret;

        this.restCallback = new RestCallback();
        restCallback.SetOnSuccess(OnRestCallback);
        restCallback.SetOnError(OnRestCallback);

        switchRequestSended = false;
        switchScenarioCalled = false;

        SendRequest();
    };

    override void SwitchScenario() {
        if (!switchRequestSended) {
            switchScenarioCalled = true;
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] SwitchScenario(): Failed to complete switch scenario request before this call. Error may occure or restart delay is too short.", level: LogLevel.WARNING);
            return;
        };

        super.SwitchScenario();
    };

    private sealed string GetToken() {
        auto filePath = this.bearerTokenFilePath;
        if (filePath.IsEmpty()) {
            return "";
        };

        auto filePathSanised = SCR_FileIOHelper.SanitiseFileName(filePath);
        if (filePathSanised != filePath) {
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] GetToken(): sanitised file path from \"%1\" to \"%2\".", filePath, filePathSanised, level: LogLevel.WARNING);
        };

        if (!FileIO.FileExists(filePathSanised)) {
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] GetToken(): file \"%1\" doesn't exists.", filePathSanised, level: LogLevel.ERROR);
            return "";
        };

        auto file = FileIO.OpenFile(filePathSanised, FileMode.READ);
        if (!file) {
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] GetToken(): failed to open file \"%1\" for read.", filePathSanised, level: LogLevel.ERROR);
            return "";
        };

        if (file.GetLength() == 0) {
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] GetToken(): file \"%1\" is empty.", filePathSanised, level: LogLevel.ERROR);
            return "";
        };

        auto token = "";
        if (file.ReadLine(token) <= 0) {
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] GetToken(): failed to read bearer token from file \"%1\". Probably, file begins with empty line, only first line will be readed.", filePathSanised, level: LogLevel.ERROR);
            return "";
        };

        return token;
    };

    protected void SendRequest() {
        auto data = GetJsonString();

        auto restContext = this.restApi.GetContext(this.host);
        if (!restContext) {
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] PrepareScenarioSwitch(): Failed to create `RestContext` for host \"%1\".", this.host, level: LogLevel.ERROR);
            return;
        };

        if (!restContext.SetHeaders(this.restHeaders)) {
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] PrepareScenarioSwitch(): Failed to set headers \"%1\".", this.restHeadersPublic, level: LogLevel.ERROR);
        };

	    auto result = restContext.POST(this.restCallback, this.endpoint, data);
        // TODO: process result when meaning of result will be known.
    };

    protected event void OnRestCallback(RestCallback cb = null) {
        if (!cb) {
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] OnRestCallback(): `cb` is null.", level: LogLevel.WARNING);
            cb = this.restCallback;
        };

		auto restResult = cb.GetRestResult();
		auto restResultOk = restResult == ERestResult.EREST_SUCCESS;
        auto httpCode = cb.GetHttpCode();
		auto httpCodeOk = httpCode >= 200 && httpCode < 300;
		auto cbOk = restResultOk || httpCodeOk;
        if (!cbOk) {
		    auto restResultName = SCR_Enum.GetEnumName(ERestResult, restResult);
            if (restResultName.IsEmpty())
                restResultName = "<Unknown>";
		    auto data = cb.GetData();
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] OnRestCallback(): Failed to send request for switching scenario. RestResult: %1 (%2), HttpCode: %3, Data: \"%4\".", restResult, restResultName, httpCode, data, level: LogLevel.ERROR);

            auto game = GetGame();
            auto callqueue = game.GetCallqueue();
            callqueue.CallLater(this.SendRequest, delay: 5 * 1000, repeat: false);
            return;
        };

        switchRequestSended = true;

        if (switchScenarioCalled)
            SwitchScenario();
    };
};
