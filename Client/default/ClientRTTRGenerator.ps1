param (
    [string]$InputDir,
    [string]$OutputFile
)

function Print-Header {
    Write-Host "========================================"
    Write-Host "Client RTTR Registration Code Generator (Sandbox Mode)"
    Write-Host "========================================"
    Write-Host ""
}

Print-Header

if (-Not (Test-Path $InputDir)) {
    Write-Host "===== [ERROR] Input directory not found: $InputDir"
    exit 1
}

if (-Not (Test-Path $OutputFile)) {
    Write-Host "===== [ERROR] Target File not found! Run the initial setup for $OutputFile"
    exit 1
}

$processedClasses = @{}
$newIncludes = ""
$newRttrBlocks = ""

# Pre-scan output file for already manually registered classes
if (Test-Path $OutputFile) {
    try {
        $existingContent = [System.IO.File]::ReadAllText($OutputFile, [System.Text.Encoding]::UTF8)
        $manualContent = [regex]::Replace($existingContent, '(?s)//\s*<AUTO_GENERATED_RTTR>.*?//\s*</AUTO_GENERATED_RTTR>', '')
        $matches = [regex]::Matches($manualContent, 'rttr::registration::class_<([A-Za-z0-9_]+)>')
        foreach ($match in $matches) {
            $manualClass = $match.Groups[1].Value
            $processedClasses[$manualClass] = $true
            # Write-Host "==========      [Info] Found manual registration: $manualClass"
        }
    } catch {
        Write-Host "==========      [Warning] Could not pre-scan manual registrations: $_"
    }
}

$headerFiles = Get-ChildItem -Path $InputDir -Filter "*.h" | Sort-Object Name

foreach ($file in $headerFiles) {
    try {
        $content = [System.IO.File]::ReadAllText($file.FullName, [System.Text.Encoding]::UTF8)
    }
    catch {
        Write-Host "===== [ERROR] Failed to read $($file.Name): $_"
        continue
    }

    if ($content -notmatch "\bRTTR_ENABLE\b") {
        continue
    }

    $isTarget = ($content -match ":\s*public\s+GameObject") -or 
                ($content -match ":\s*public\s+Script") -or 
                ($content -match ":\s*public\s+Component") -or 
                ($content -match "\bRTTR_ENABLE\s*\(\s*(GameObject|Script|Component|.*?)\s*\)")

    if (-not $isTarget) {
        continue
    }

    if ($content -match "\bclass\s+(?:CLIENT_DLL\s+)?([A-Za-z0-9_]+)(?:\s+(?:final|abstract))?\s*[:{]") {
        $className = $Matches[1]

        if ($processedClasses.ContainsKey($className)) {
            continue
        }
        $processedClasses[$className] = $true

        $hasClone = $content -match "\bClone\s*\("
        $hasCreate = $content -match "\bstatic\s+.*\bCreate\s*\("

        if (-not $hasCreate) {
            continue
        }

        $targetLevel = "0"
        if ($content -match "//\s*\[RTTR_LEVEL\]\s*(LEVEL::[a-zA-Z0-9_]+|\d+)") {
            $targetLevel = $Matches[1]
        }

        $methodRegistrations = ""
        if ($hasClone) {
            $methodRegistrations += "`n        .method(`"Clone`", &$className::Clone)"
        }
        if ($hasCreate) {
            $methodRegistrations += "`n        .method(`"Create`", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> Shared<GameObject> { return $className::Create(device, context); })(rttr::metadata(`"Level`", $targetLevel))"
        }

        $rttrBlock = "    rttr::registration::class_<$className>(`"$className`")`n        .constructor<>()" + $methodRegistrations + ";`n`n"
        
        $newIncludes += "#include `"$className.h`"`n"
        $newRttrBlocks += $rttrBlock
        Write-Host "==========      [Auto] Discovered target class: $className"
    }
}

if ($newIncludes -eq "") {
    Write-Host "==========      [INFO] No Target RTTR classes found."
    exit 0
}

try {
    $outContent = [System.IO.File]::ReadAllText($OutputFile, [System.Text.Encoding]::UTF8)
}
catch {
    Write-Host "===== [ERROR] Could not read $($OutputFile): $_"
    exit 1
}

# Regex replacement for includes
$outContent = [regex]::Replace($outContent, '(?s)(//\s*<AUTO_GENERATED_INCLUDES>\r?\n).*?(\r?\n//\s*</AUTO_GENERATED_INCLUDES>)', "`${1}$newIncludes`${2}")

# Regex replacement for RTTR blocks
$outContent = [regex]::Replace($outContent, '(?s)(//\s*<AUTO_GENERATED_RTTR>\r?\n).*?(\r?\n//\s*</AUTO_GENERATED_RTTR>)', "`${1}$newRttrBlocks`${2}")

try {
    [System.IO.File]::WriteAllText($OutputFile, $outContent, [System.Text.Encoding]::UTF8)
    Write-Host "========== [SUCCESS] Sandboxed rewrite complete: $(Split-Path $OutputFile -Leaf)"
}
catch {
    Write-Host "===== [ERROR] Failed to write changes: $_"
    exit 1
}
exit 0
