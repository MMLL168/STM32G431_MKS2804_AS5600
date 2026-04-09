param(
    [string]$Python = "python"
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

& $Python "$PSScriptRoot\angle_monitor_app.py"
