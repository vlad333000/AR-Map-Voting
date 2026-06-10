[CmdletBinding()]
param(
	[string]$ComposeProject = $(if ($env:COMPOSE_PROJECT_NAME) { $env:COMPOSE_PROJECT_NAME } else { "v30-map-voting-reforger" }),
	[string]$Service = $(if ($env:SERVICE) { $env:SERVICE } else { "reforger" }),
	[switch]$NoBuild
)

$ErrorActionPreference = "Stop"
Set-StrictMode -Version Latest

function Invoke-Checked {
	param(
		[string]$FilePath,
		[string[]]$Arguments
	)

	& $FilePath @Arguments
	if ($LASTEXITCODE -ne 0) {
		throw "$FilePath failed with exit code $LASTEXITCODE"
	}
}

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path

Push-Location $scriptDir
try {
	if (-not $NoBuild) {
		Invoke-Checked docker @("compose", "-p", $ComposeProject, "build", $Service)
	}

	Invoke-Checked docker @("compose", "-p", $ComposeProject, "up", "-d", $Service)
	Invoke-Checked docker @("compose", "-p", $ComposeProject, "ps", $Service)
}
finally {
	Pop-Location
}

Write-Host "Deploy complete. Follow logs with: docker compose -p $ComposeProject logs -f $Service"
