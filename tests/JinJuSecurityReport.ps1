[CmdletBinding()]
param(
    [ValidateRange(1,168)][int]$Hours = 2,
    [string]$OutputPath = (Join-Path $PSScriptRoot 'JinJuSecurity.log'),
    [datetime]$Since = [datetime]::MinValue,
    [switch]$SelfTest
)
$ErrorActionPreference = 'Stop'
$names = @('JinJuApp.exe','JinJuXaml.dll','JinJuXamlUnload.exe','JinJuMarquee.exe')
$lines = [System.Collections.Generic.List[string]]::new()
function Add-Line([string]$Text) { $lines.Add($Text) }
function Add-Section([string]$Title, [scriptblock]$Read) {
    Add-Line ""
    Add-Line "=== $Title ==="
    try {
        $result = @(& $Read)
        if ($result.Count -eq 0) { Add-Line 'No records returned.' }
        else { Add-Line (($result | Out-String -Width 240).TrimEnd()) }
    } catch { Add-Line ("Unavailable: " + $_.Exception.Message) }
}
function Get-BinaryReport([string]$Directory) {
    foreach ($name in $names) {
        $path = Join-Path $Directory $name
        if (-not (Test-Path -LiteralPath $path -PathType Leaf)) {
            [pscustomobject]@{ File=$name; State='Missing' }
            continue
        }
        $hash = Get-FileHash -LiteralPath $path -Algorithm SHA256
        $sig = Get-AuthenticodeSignature -LiteralPath $path
        $manifest = "$path.sha256"
        $manifestStatus = 'Missing'
        if (Test-Path -LiteralPath $manifest -PathType Leaf) {
            $expected = ((Get-Content -LiteralPath $manifest -TotalCount 1) -split '\s+')[0]
            $manifestStatus = if ($expected -eq $hash.Hash) { 'Match' } else { 'Mismatch' }
        }
        [pscustomobject]@{
            File=$name; State='Present'; SHA256=$hash.Hash
            Manifest=$manifestStatus; Signature=$sig.Status
            Signer=$sig.SignerCertificate.Subject
            LastWriteUtc=(Get-Item -LiteralPath $path).LastWriteTimeUtc.ToString('o')
        }
    }
}
function Get-ProjectMatches($Records) {
    # Only show events mentioning this product. Do not dump unrelated threats.
    $pattern = '(?i)JinJuApp\.exe|JinJuXaml\.dll|JinJuXamlUnload\.exe|JinJuMarquee\.exe'
    foreach ($record in $Records) {
        $fields = @($record.Message, $record.ProcessName, $record.Resources) -join ' '
        if ($fields -match $pattern) { $record }
    }
}
function Read-Events([string]$Channel,[int[]]$Ids,[datetime]$Start) {
    try {
        @(Get-WinEvent -FilterHashtable @{LogName=$Channel;Id=$Ids;StartTime=$Start} -ErrorAction Stop)
    } catch {
        if ($_.FullyQualifiedErrorId -like 'NoMatchingEventsFound*') { return }
        throw
    }
}
function Get-XamlRecords([string[]]$Records,[datetime]$Start) {
    foreach ($line in $Records) {
        if ($line -match '^(\d{4})-(\d{1,2})-(\d{1,2})T(\d{1,2}):(\d{1,2}):(\d{1,2})\.(\d{1,3})Z ') {
            try {
                $time = [datetime]::new([int]$Matches[1],[int]$Matches[2],[int]$Matches[3],[int]$Matches[4],[int]$Matches[5],[int]$Matches[6],[int]$Matches[7],[DateTimeKind]::Utc)
                if ($time -ge $Start.ToUniversalTime()) { $line }
            } catch { continue }
        }
    }
}
if ($SelfTest) {
    $sample = @([pscustomobject]@{Message='Detected JinJuXaml.dll'},[pscustomobject]@{Message='Other application'})
    if (@(Get-ProjectMatches $sample).Count -ne 1) { throw 'Project filter test failed' }
    $resource = [pscustomobject]@{Resources=@('file:_C:\test\JinJuXaml.dll');ProcessName='explorer.exe'}
    if (@(Get-ProjectMatches @($resource)).Count -ne 1) { throw 'Resource match failed' }
    $sampleLog = @('2026-11-25T18:1:11.449Z pid=18832 Launcher: Explorer PID=7640','2026-11-25T17:15:41.348Z old')
    $cutoff = [datetime]::new(2026,11,25,18,0,0,[DateTimeKind]::Utc)
    if (@(Get-XamlRecords $sampleLog $cutoff).Count -ne 1) { throw 'Time filter failed' }
    $temp = Join-Path ([IO.Path]::GetTempPath()) ([guid]::NewGuid().ToString())
    try {
        New-Item -ItemType Directory -Path $temp | Out-Null
        if (@(Get-BinaryReport $temp | Where-Object State -eq 'Missing').Count -ne 4) { throw 'Missing binary test failed' }
        Add-Section 'Synthetic failure' { throw 'test error' }
        if (($lines -join ' ') -notmatch 'Unavailable: test error') { throw 'Error handling test failed' }
    } finally { Remove-Item -LiteralPath $temp -Recurse -Force }
    Write-Output 'Security report self-test passed (no Defender settings accessed or changed).'
    return
}
$start = if ($Since -eq [datetime]::MinValue) { (Get-Date).AddHours(-$Hours) } else { $Since }
if ($start -gt (Get-Date)) { throw 'Since must not be in the future.' }
Add-Line 'JinJu security diagnostic report (read-only)'
Add-Line ("Generated UTC: " + [DateTime]::UtcNow.ToString('o'))
Add-Line ("Event window start UTC: " + $start.ToUniversalTime().ToString('o'))
Add-Line 'No protection settings, exclusions or threat actions are changed.'
Add-Section 'Windows version' {
    Get-CimInstance Win32_OperatingSystem | Select-Object Caption,Version,BuildNumber | Format-List
}
Add-Section 'Defender status' {
    Get-MpComputerStatus | Select-Object AMRunningMode,AMServiceEnabled,AntivirusEnabled,RealTimeProtectionEnabled,BehaviorMonitorEnabled,IoavProtectionEnabled,AntivirusSignatureVersion,AntivirusSignatureLastUpdated,AMProductVersion,AMEngineVersion | Format-List
}
Add-Section 'Binary integrity and Authenticode' {
    Get-BinaryReport $PSScriptRoot | Format-List
}
Add-Section 'Recent product-related threat detections' {
    $detections = @(Get-MpThreatDetection | Where-Object { $_.InitialDetectionTime -ge $start })
    $matched = @(Get-ProjectMatches $detections)
    if ($matched.Count -eq 0) { 'No matching detection in the requested time window.' }
    else {
        $threats = @(Get-MpThreat)
        foreach ($d in $matched) {
            $name = ($threats | Where-Object ThreatID -eq $d.ThreatID | Select-Object -First 1).ThreatName
            $d | Select-Object InitialDetectionTime,LastThreatStatusChangeTime,ThreatID,@{Name='ThreatName';Expression={$name}},ActionSuccess,ThreatStatusID,ProcessName,Resources | Format-List
        }
    }
}
Add-Section 'Recent product-related Defender events' {
    $events = @(Read-Events 'Microsoft-Windows-Windows Defender/Operational' @(1116,1117,1121,1122) $start)
    "Total events in selected categories: $($events.Count)"
    Get-ProjectMatches ($events | Select-Object TimeCreated,Id,Message) | Format-List
}
Add-Section 'Defender protection state changes (historical)' {
    Read-Events 'Microsoft-Windows-Windows Defender/Operational' @(5000,5001) $start |
        Select-Object TimeCreated,Id,Message | Format-List
}
Add-Section 'Recent product-related Code Integrity events' {
    $events = @(Read-Events 'Microsoft-Windows-CodeIntegrity/Operational' @(3033,3034,3076,3077,3089) $start)
    "Total events in selected categories: $($events.Count)"
    Get-ProjectMatches ($events | Select-Object TimeCreated,Id,Message) | Format-List
}
Add-Section 'Recent product-related AppLocker events' {
    $events = @(Read-Events 'Microsoft-Windows-AppLocker/EXE and DLL' @(8002,8003,8004) $start)
    "Total events in selected categories: $($events.Count)"
    Get-ProjectMatches ($events | Select-Object TimeCreated,Id,Message) | Format-List
}
Add-Section 'Recent XAML initialization and error milestones (UTC in source)' {
    $path = Join-Path $PSScriptRoot 'JinJuXaml.log'
    if (-not (Test-Path -LiteralPath $path -PathType Leaf)) { 'XAML log missing.' }
    else {
        Get-XamlRecords @(Get-Content -LiteralPath $path -Tail 20000) $start |
            Where-Object { $_ -match 'Launcher:|VisualDiagConnection\d+|DllGetClassObject|SetSite|AdviseVisualTreeChange|TextBlock appended|exception|P1 layout attempt=' } |
            Select-Object -Last 100
    }
}
Add-Line ''
Add-Line 'No matching events does not prove that Defender allowed the operation. Current Defender status is not historical status. Keep protection enabled. Review paths and messages before sharing.'
$lines | Set-Content -LiteralPath $OutputPath -Encoding UTF8
Write-Output "Report: $OutputPath"
