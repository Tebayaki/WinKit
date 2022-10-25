$manifestPath = $PSScriptRoot + "\manifest.json"
if (Test-Path $manifestPath) {
    Set-ItemProperty "HKLM:\SOFTWARE\Microsoft\Edge\NativeMessagingHosts\com.ime" -Name "(Default)" -Value $manifestPath
}