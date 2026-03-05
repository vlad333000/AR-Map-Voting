[BaseContainerProps()]
sealed class V30_MapVoting_RestApiExternalRunner : V30_MapVoting_ExternalRunner {
    [Attribute("http://localhost:8080")]
    protected string host;

    [Attribute("/scenario")]
    protected string endpoint;

    [Attribute("$profile:V30/MapVoting/token.txt")]
    protected string bearerTokenFilePath;

    [Attribute("0", desc: "HTTP method to use for request.", uiwidget: UIWidgets.ComboBox, enumType: V30_MapVoting_RestApiExternalRunner_EMethod)]
    protected V30_MapVoting_RestApiExternalRunner_EMethod method;

    [Attribute("0", desc: "Format to use for request.", uiwidget: UIWidgets.ComboBox, enumType: V30_MapVoting_RestApiExternalRunner_EFormat)]
    protected V30_MapVoting_RestApiExternalRunner_EFormat format;

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

		string headers;
		string headersSecret;
        switch (this.format) {
            case V30_MapVoting_RestApiExternalRunner_EFormat.JSON : {
                headers = "Content-Type,application/json";
                headersSecret = "Content-Type,application/json";
                break;
            };
            default : {
                auto formatName = SCR_Enum.GetEnumName(V30_MapVoting_RestApiExternalRunner_EFormat, this.format);
                PrintFormat("[V30][MapVoting][RestApiExternalRunner] PrepareScenarioSwitch: unsupported format %1 (%2).", this.format, formatName, level: LogLevel.ERROR);
                break;
            };
        };

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
        string data;
        switch (this.format) {
            case V30_MapVoting_RestApiExternalRunner_EFormat.JSON : {
                data = GetJsonString();
                break;
            };
            default : {
                auto formatName = SCR_Enum.GetEnumName(V30_MapVoting_RestApiExternalRunner_EFormat, this.format);
                PrintFormat("[V30][MapVoting][RestApiExternalRunner] SendRequest: unsupported format %1 (%2).", this.format, formatName, level: LogLevel.ERROR);
                break;
            };
        };


        auto restContext = this.restApi.GetContext(this.host);
        if (!restContext) {
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] PrepareScenarioSwitch(): Failed to create `RestContext` for host \"%1\".", this.host, level: LogLevel.ERROR);
            return;
        };

        if (!restContext.SetHeaders(this.restHeaders)) {
            PrintFormat("[V30][MapVoting][RestApiExternalRunner] PrepareScenarioSwitch(): Failed to set headers \"%1\".", this.restHeadersPublic, level: LogLevel.ERROR);
        };

        switch (this.method) {
            case V30_MapVoting_RestApiExternalRunner_EMethod.POST : {
                auto result = restContext.POST(this.restCallback, this.endpoint, data);
                // TODO: parse result when meaning of int result will be known
                break;
            };
            case V30_MapVoting_RestApiExternalRunner_EMethod.POST_now : {
                auto result = restContext.POST_now(this.endpoint, data);
                // TODO: parse result for error
                break;
            };
            case V30_MapVoting_RestApiExternalRunner_EMethod.PUT : {
                auto result = restContext.PUT(this.restCallback, this.endpoint, data);
                // TODO: parse result when meaning of int result will be known
                break;
            };
            case V30_MapVoting_RestApiExternalRunner_EMethod.PUT_now : {
                auto result = restContext.PUT_now(this.endpoint, data);
                // TODO: parse result for error
                break;
            };
            default : {
                auto methodName = SCR_Enum.GetEnumName(V30_MapVoting_RestApiExternalRunner_EMethod, this.method);
                PrintFormat("[V30][MapVoting][RestApiExternalRunner] SendRequest: unsupported method %1 (%2).", this.method, methodName, level: LogLevel.ERROR);
                break;
            };
        };
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

enum V30_MapVoting_RestApiExternalRunner_EMethod {
    POST,
    POST_now,
    PUT,
    PUT_now
    // TODO: GET // Supports only URLQueryString format
    // TODO: GET_now // Supports only URLQueryString format
};

enum V30_MapVoting_RestApiExternalRunner_EFormat {
    JSON
    // TODO: URLQueryString // http://host/endpoint?missionHeader=...&worldSystemsConfig=...&addons=ADDON1%2CADDON2%2C...
    // TODO: URLEncoded // missionHeader=...&worldSystemsConfig=...&addons=ADDON1%2CADDON2%2C...
    // TODO: PlainText // ...\n....\n...\n
    // TODO: XML // <scenario><missionHeader>...</missionHeader><worldSystemsConfig>...</worldSystemsConfig><addons><addon>...</addon>...</addons></scenario>
}
