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

        $methodRegistrations = ""
        if ($hasClone) {
            $methodRegistrations += "`n        .method(`"Clone`", &$className::Clone)"
        }
        if ($hasCreate) {
            $methodRegistrations += "`n        .method(`"Create`", &$className::Create)"
        }

        $rttrBlock = "    rttr::registration::class_<$className>(L`"$className`")`n        .constructor<>()" + $methodRegistrations + ";`n`n"
        
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
$outContent = [regex]::Replace($outContent, '(?s)(//\s*<AUTO_GENERATED_INCLUDES>\n).*?(\n//\s*</AUTO_GENERATED_INCLUDES>)', "`${1}$newIncludes`${2}")

# Regex replacement for RTTR blocks
$outContent = [regex]::Replace($outContent, '(?s)(//\s*<AUTO_GENERATED_RTTR>\n).*?(\n//\s*</AUTO_GENERATED_RTTR>)', "`${1}$newRttrBlocks`${2}")

try {
    [System.IO.File]::WriteAllText($OutputFile, $outContent, [System.Text.Encoding]::UTF8)
    Write-Host "========== [SUCCESS] Sandboxed rewrite complete: $(Split-Path $OutputFile -Leaf)"
}
catch {
    Write-Host "===== [ERROR] Failed to write changes: $_"
    exit 1
}
exit 0
