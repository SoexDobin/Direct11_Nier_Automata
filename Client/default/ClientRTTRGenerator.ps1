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

# Existing registrations are immutable schema keys. Preserve all existing blocks
# and append only newly discovered classes.
if (Test-Path $OutputFile) {
    try {
        $existingContent = [System.IO.File]::ReadAllText($OutputFile, [System.Text.Encoding]::UTF8)
        $matches = [regex]::Matches($existingContent, 'rttr::registration::class_<([A-Za-z0-9_]+)>')
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

    # 상속 관계를 분석하여 반환 타입(GameObject vs Component) 결정
    # Component 계열 감지 (부모 클래스 이름 또는 직접 상속 텍스트 확인)
    $isComponent = ($content -match ":\s*public\s+(?:Script|Component|StateMachine)") -or 
                   ($content -match "\bRTTR_ENABLE\s*\(\s*(?:Script|Component|StateMachine)\s*\)")
    
    $returnType = "Shared<GameObject>"
    if ($isComponent) {
        $returnType = "Shared<Component>"
    }

    # RTTR_ENABLE과 static Create가 모두 있으면 타겟으로 간주 (상속 깊이 상관없음)
    $hasRttr = $content -match "\bRTTR_ENABLE\b"
    $hasCreate = $content -match "\bstatic\s+.*\bCreate\s*\("

    if (-not ($hasRttr -and $hasCreate)) {
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
            $methodRegistrations += "`r`n        .method(`"Clone`", &$className::Clone)"
        }
        if ($hasCreate) {
            $methodRegistrations += "`r`n        .method(`"Create`", [](const ComPtr<ID3D11Device>& device, const ComPtr<ID3D11DeviceContext>& context) -> $returnType { return $className::Create(device, context); })(rttr::metadata(`"Level`", $targetLevel))"
        }

        $rttrBlock = "    rttr::registration::class_<$className>(`"$className`")`r`n        .constructor<>()" + $methodRegistrations + ";`r`n`r`n"
        
        $newIncludes += "#include `"$className.h`"`r`n"
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

if ($outContent -notmatch '//\s*</AUTO_GENERATED_INCLUDES>' -or
    $outContent -notmatch '//\s*</AUTO_GENERATED_RTTR>') {
    Write-Host "===== [ERROR] Auto-generated markers are missing"
    exit 1
}

$outContent = [regex]::Replace($outContent, '//\s*</AUTO_GENERATED_INCLUDES>',
    "$newIncludes// </AUTO_GENERATED_INCLUDES>", 1)
$outContent = [regex]::Replace($outContent, '//\s*</AUTO_GENERATED_RTTR>',
    "$newRttrBlocks// </AUTO_GENERATED_RTTR>", 1)

try {
    # Replace all LFs with CRLFs to ensure consistency
    $outContent = $outContent -replace "(?<!`r)`n", "`r`n"
    [System.IO.File]::WriteAllText($OutputFile, $outContent, [System.Text.Encoding]::UTF8)
    Write-Host "========== [SUCCESS] Sandboxed rewrite complete: $(Split-Path $OutputFile -Leaf)"
}
catch {
    Write-Host "===== [ERROR] Failed to write changes: $_"
    exit 1
}
exit 0
