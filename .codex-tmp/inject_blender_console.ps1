param(
    [Parameter(Mandatory = $true)]
    [long]$WindowHandle,

    [Parameter(Mandatory = $true)]
    [string]$CommandPath,

    [Parameter(Mandatory = $true)]
    [string]$CompletionPath,

    [int]$InitialDelayMilliseconds = 2500,

    [double]$ClickXRatio = 0.5,

    [double]$ClickYRatio = 0.5,

    [switch]$SkipConsoleSwitch
)

$ErrorActionPreference = 'Stop'

$signature = @'
[DllImport("user32.dll")] public static extern bool SetForegroundWindow(IntPtr hWnd);
[DllImport("user32.dll")] public static extern bool ShowWindowAsync(IntPtr hWnd, int nCmdShow);
[DllImport("user32.dll")] public static extern bool BringWindowToTop(IntPtr hWnd);
[DllImport("user32.dll")] public static extern void keybd_event(byte vk, byte scan, uint flags, UIntPtr extra);
[DllImport("user32.dll")] public static extern bool GetWindowRect(IntPtr hWnd, out RECT rect);
[DllImport("user32.dll")] public static extern bool GetCursorPos(out POINT point);
[DllImport("user32.dll")] public static extern bool SetCursorPos(int x, int y);
[DllImport("user32.dll")] public static extern void mouse_event(uint flags, uint dx, uint dy, uint data, UIntPtr extra);
public struct RECT { public int Left; public int Top; public int Right; public int Bottom; }
public struct POINT { public int X; public int Y; }
'@

Add-Type -MemberDefinition $signature -Name NativeMethods -Namespace CodexBlender
Add-Type -AssemblyName System.Windows.Forms

$completion = [ordered]@{
    success = $false
    window_handle = $WindowHandle
    error = $null
}

$previousClipboard = $null
$hadClipboard = $false
$cursor = New-Object CodexBlender.NativeMethods+POINT
$hadCursor = [CodexBlender.NativeMethods]::GetCursorPos([ref]$cursor)

try {
    Start-Sleep -Milliseconds $InitialDelayMilliseconds

    $Command = Get-Content -LiteralPath $CommandPath -Raw -Encoding UTF8

    try {
        $previousClipboard = Get-Clipboard -Raw -ErrorAction Stop
        $hadClipboard = $true
    }
    catch {
        $hadClipboard = $false
    }

    Set-Clipboard -Value $Command

    $handle = [IntPtr]$WindowHandle
    [CodexBlender.NativeMethods]::ShowWindowAsync($handle, 9) | Out-Null
    [CodexBlender.NativeMethods]::keybd_event(0x12, 0, 0, [UIntPtr]::Zero)
    [CodexBlender.NativeMethods]::keybd_event(0x12, 0, 2, [UIntPtr]::Zero)
    [CodexBlender.NativeMethods]::BringWindowToTop($handle) | Out-Null
    if (-not [CodexBlender.NativeMethods]::SetForegroundWindow($handle)) {
        throw 'SetForegroundWindow failed.'
    }

    $rect = New-Object CodexBlender.NativeMethods+RECT
    if (-not [CodexBlender.NativeMethods]::GetWindowRect($handle, [ref]$rect)) {
        throw 'GetWindowRect failed.'
    }

    $x = [int]($rect.Left + (($rect.Right - $rect.Left) * $ClickXRatio))
    $y = [int]($rect.Top + (($rect.Bottom - $rect.Top) * $ClickYRatio))
    [CodexBlender.NativeMethods]::SetCursorPos($x, $y) | Out-Null
    [CodexBlender.NativeMethods]::mouse_event(2, 0, 0, 0, [UIntPtr]::Zero)
    [CodexBlender.NativeMethods]::mouse_event(4, 0, 0, 0, [UIntPtr]::Zero)

    Start-Sleep -Milliseconds 350
    if (-not $SkipConsoleSwitch) {
        [System.Windows.Forms.SendKeys]::SendWait('+{F4}')
        Start-Sleep -Milliseconds 800
        [CodexBlender.NativeMethods]::SetCursorPos($x, $y) | Out-Null
        [CodexBlender.NativeMethods]::mouse_event(2, 0, 0, 0, [UIntPtr]::Zero)
        [CodexBlender.NativeMethods]::mouse_event(4, 0, 0, 0, [UIntPtr]::Zero)
        Start-Sleep -Milliseconds 250
    }
    [System.Windows.Forms.SendKeys]::SendWait('^v')
    Start-Sleep -Milliseconds 250
    [System.Windows.Forms.SendKeys]::SendWait('{ENTER}')
    Start-Sleep -Milliseconds 1000

    $completion.success = $true
}
catch {
    $completion.error = $_.Exception.Message
}
finally {
    if ($hadClipboard) {
        Set-Clipboard -Value $previousClipboard
    }
    if ($hadCursor) {
        [CodexBlender.NativeMethods]::SetCursorPos($cursor.X, $cursor.Y) | Out-Null
    }

    $completion | ConvertTo-Json | Set-Content -LiteralPath $CompletionPath -Encoding UTF8
}
