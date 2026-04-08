param(
    [string]$LogPath = "devlog.md",
    [string]$Title,
    [string]$Reason,
    [string]$Purpose,
    [string[]]$Methods,
    [datetime]$Timestamp = (Get-Date),
    [switch]$RebuildIndexOnly
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$LabelIndex = "## " + [string]([char]0x7D22) + [char]0x5F15
$LabelReason = "**" + [string]([char]0x539F) + [char]0x56E0 + [char]0xFF1A + "** "
$LabelPurpose = "**" + [string]([char]0x76EE) + [char]0x7684 + [char]0xFF1A + "** "
$LabelMethods = "**" + [string]([char]0x4FEE) + [char]0x6539 + [char]0x65B9 + [char]0x5F0F + [char]0xFF1A + "**"
$LabelDatetime = "**" + [string]([char]0x65E5) + [char]0x671F + [char]0x6642 + [char]0x9593 + [char]0xFF1A + "** "

function Read-Utf8NoBom([string]$Path) {
    return [System.IO.File]::ReadAllText($Path, [System.Text.UTF8Encoding]::new($false))
}

function Write-Utf8NoBom([string]$Path, [string]$Content) {
    [System.IO.File]::WriteAllText($Path, $Content, [System.Text.UTF8Encoding]::new($false))
}

function Remove-ExistingIndex([System.Collections.Generic.List[string]]$Lines, [string]$IndexLabel) {
    $start = $Lines.IndexOf("<!-- devlog-index:start -->")
    $end = $Lines.IndexOf("<!-- devlog-index:end -->")
    if (($start -lt 0) -or ($end -lt 0) -or ($end -lt $start)) {
        return
    }

    $removeFrom = $start
    if (($start -gt 0) -and ($Lines[$start - 1] -eq $IndexLabel)) {
        $removeFrom = $start - 1
    }

    for ($i = $end; $i -ge $removeFrom; $i--) {
        $Lines.RemoveAt($i)
    }

    while (($removeFrom -lt $Lines.Count) -and ($Lines[$removeFrom] -eq "")) {
        $Lines.RemoveAt($removeFrom)
    }
}

function New-IndexBlock([string[]]$FinalLines, [string]$IndexLabel) {
    $items = [System.Collections.Generic.List[string]]::new()
    for ($i = 0; $i -lt $FinalLines.Length; $i++) {
        if (($FinalLines[$i] -like "## *") -and ($FinalLines[$i] -ne $IndexLabel)) {
            $items.Add("- L$($i + 1): $($FinalLines[$i].Substring(3))")
        }
    }

    $block = [System.Collections.Generic.List[string]]::new()
    $block.Add($IndexLabel)
    $block.Add("<!-- devlog-index:start -->")
    foreach ($item in $items) {
        $block.Add($item)
    }
    $block.Add("<!-- devlog-index:end -->")
    $block.Add("")
    return $block
}

if (-not (Test-Path -LiteralPath $LogPath)) {
    throw "Cannot find devlog file: $LogPath"
}

$content = Read-Utf8NoBom -Path $LogPath
$normalized = $content -replace "`r`n", "`n"
$lines = [System.Collections.Generic.List[string]]::new()
foreach ($line in ($normalized -split "`n")) {
    $lines.Add($line)
}

Remove-ExistingIndex -Lines $lines -IndexLabel $LabelIndex

if (-not $RebuildIndexOnly) {
    if ([string]::IsNullOrWhiteSpace($Title) -or
        [string]::IsNullOrWhiteSpace($Reason) -or
        ($null -eq $Methods) -or
        ($Methods.Count -eq 0)) {
        throw "Title, Reason, and at least one Methods item are required unless -RebuildIndexOnly is used."
    }

    $timestampText = $Timestamp.ToString("yyyy-MM-dd HH:mm:ss")
    $entry = [System.Collections.Generic.List[string]]::new()
    $entry.Add("## $timestampText ($Title)")
    $entry.Add("")
    $entry.Add($LabelReason + $Reason)
    if (-not [string]::IsNullOrWhiteSpace($Purpose)) {
        $entry.Add("")
        $entry.Add($LabelPurpose + $Purpose)
    }
    $entry.Add("")
    $entry.Add($LabelMethods)
    foreach ($method in $Methods) {
        $entry.Add("- $method")
    }
    $entry.Add("")
    $entry.Add($LabelDatetime + $timestampText)
    $entry.Add("")
    $entry.Add("---")
    $entry.Add("")

    $insertAfter = $lines.IndexOf("---")
    if ($insertAfter -lt 0) {
        throw "Cannot find the first separator ('---') in $LogPath"
    }

    for ($i = $entry.Count - 1; $i -ge 0; $i--) {
        $lines.Insert($insertAfter + 1, $entry[$i])
    }
}

$titleIndex = -1
for ($i = 0; $i -lt $lines.Count; $i++) {
    if ($lines[$i] -like "# *") {
        $titleIndex = $i
        break
    }
}
if ($titleIndex -lt 0) {
    throw "Cannot find the title heading in $LogPath"
}

while ((($titleIndex + 1) -lt $lines.Count) -and ($lines[$titleIndex + 1] -eq "")) {
    $lines.RemoveAt($titleIndex + 1)
}
$lines.Insert($titleIndex + 1, "")

$placeholderCount = 0
for ($i = 0; $i -lt $lines.Count; $i++) {
    if ($lines[$i] -like "## *") {
        $placeholderCount++
    }
}

$placeholderIndex = [System.Collections.Generic.List[string]]::new()
$placeholderIndex.Add($LabelIndex)
$placeholderIndex.Add("<!-- devlog-index:start -->")
for ($i = 0; $i -lt $placeholderCount; $i++) {
    $placeholderIndex.Add("- L0: pending")
}
$placeholderIndex.Add("<!-- devlog-index:end -->")
$placeholderIndex.Add("")

for ($i = $placeholderIndex.Count - 1; $i -ge 0; $i--) {
    $lines.Insert($titleIndex + 2, $placeholderIndex[$i])
}

$previewLines = $lines.ToArray()
$realIndex = New-IndexBlock -FinalLines $previewLines -IndexLabel $LabelIndex

for ($i = $titleIndex + 2 + $placeholderIndex.Count - 1; $i -ge $titleIndex + 2; $i--) {
    $lines.RemoveAt($i)
}
for ($i = $realIndex.Count - 1; $i -ge 0; $i--) {
    $lines.Insert($titleIndex + 2, $realIndex[$i])
}

$output = ($lines -join "`r`n").TrimEnd("`r", "`n") + "`r`n"
Write-Utf8NoBom -Path $LogPath -Content $output
