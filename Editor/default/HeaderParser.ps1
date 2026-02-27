# HeaderParser.ps1 — Client Header Parser for Prefab Metadata
# Usage: powershell -ExecutionPolicy Bypass -File HeaderParser.ps1 -InputDir <path> -OutputJson <path>

param(
    [Parameter(Mandatory=$true)]
    [string]$InputDir,

    [Parameter(Mandatory=$true)]
    [string]$OutputJson
)

Write-Host "========================================" 
Write-Host "Header Parser - Prefab Metadata Generator"
Write-Host "========================================" 
Write-Host ""
Write-Host "  [INFO] Input:  $InputDir"
Write-Host "  [INFO] Output: $OutputJson"
Write-Host ""

if (-not (Test-Path $InputDir)) {
    Write-Host "  [ERROR] Input directory not found: $InputDir"
    exit 1
}

$outputDir = Split-Path $OutputJson -Parent
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir -Force | Out-Null
}

# ============================================================
# Engine 내장 클래스 (상속 참조용 — 멤버 포함)
# ============================================================
$engineClasses = @{
    "Object" = @{
        namespace = "Engine"
        base_class = ""
        has_create = $false
        has_clone = $false
        category = "other"
        members = @()
    }
    "GameObject" = @{
        namespace = "Engine"
        base_class = "Object"
        has_create = $false
        has_clone = $true
        category = "gameobject"
        members = @(
            @{ name = "m_Device"; type = "ComPtr<ID3D11Device>"; default = "nullptr"; access = "protected" }
            @{ name = "m_Context"; type = "ComPtr<ID3D11DeviceContext>"; default = "nullptr"; access = "protected" }
            @{ name = "m_LayerMask"; type = "LayerMask"; default = ""; access = "protected" }
            @{ name = "m_TagMask"; type = "TagMask"; default = ""; access = "protected" }
        )
    }
    "Level" = @{
        namespace = "Engine"
        base_class = "Object"
        has_create = $false
        has_clone = $false
        category = "level"
        members = @(
            @{ name = "m_Device"; type = "ComPtr<ID3D11Device>"; default = "nullptr"; access = "protected" }
            @{ name = "m_Context"; type = "ComPtr<ID3D11DeviceContext>"; default = "nullptr"; access = "protected" }
        )
    }
    "Component" = @{
        namespace = "Engine"
        base_class = "Object"
        has_create = $false
        has_clone = $true
        category = "component"
        members = @(
            @{ name = "m_Device"; type = "ComPtr<ID3D11Device>"; default = "nullptr"; access = "protected" }
            @{ name = "m_Context"; type = "ComPtr<ID3D11DeviceContext>"; default = "nullptr"; access = "protected" }
            @{ name = "m_Owner"; type = "Weak<GameObject>"; default = ""; access = "protected" }
        )
    }
    "Transform" = @{
        namespace = "Engine"
        base_class = "Component"
        has_create = $true
        has_clone = $true
        category = "component"
        members = @(
            @{ name = "m_LocalScale"; type = "Vector3"; default = "Vector3::One"; access = "private" }
            @{ name = "m_LocalRotation"; type = "Quaternion"; default = "Quaternion::Identity"; access = "private" }
            @{ name = "m_LocalPosition"; type = "Vector3"; default = "Vector3::Zero"; access = "private" }
            @{ name = "m_WorldMatrix"; type = "Matrix"; default = "Matrix::Identity"; access = "private" }
            @{ name = "m_IsDirty"; type = "Bool"; default = "true"; access = "private" }
        )
    }
    "UIObject" = @{
        namespace = "Engine"
        base_class = "GameObject"
        has_create = $false
        has_clone = $false
        category = "gameobject"
        members = @()
    }
}

# ============================================================
# 카테고리 결정 함수 — 상속 체인 탐색
# ============================================================
function Get-Category($baseClass) {
    $current = $baseClass
    while ($current -ne "" -and $current -ne $null) {
        if ($current -eq "GameObject" -or $current -eq "UIObject") { return "gameobject" }
        if ($current -eq "Level") { return "level" }
        if ($current -eq "Component") { return "component" }
        if ($engineClasses.ContainsKey($current)) {
            $current = $engineClasses[$current].base_class
        } else {
            break
        }
    }
    return "other"
}

# ============================================================
# 상속 체인으로 모든 부모 멤버 수집
# ============================================================
function Get-InheritedMembers($baseClass, $allClasses) {
    $inherited = @()
    $current = $baseClass
    while ($current -ne "" -and $current -ne $null) {
        if ($engineClasses.ContainsKey($current)) {
            $inherited = $engineClasses[$current].members + $inherited
            $current = $engineClasses[$current].base_class
        } elseif ($allClasses.ContainsKey($current)) {
            $inherited = $allClasses[$current].members + $inherited
            $current = $allClasses[$current].base_class
        } else {
            break
        }
    }
    return $inherited
}

# ============================================================
# 클라이언트 헤더 파싱
# ============================================================
$classes = @{}

foreach ($file in Get-ChildItem -Path $InputDir -Filter "*.h" | Sort-Object Name) {
    $content = Get-Content $file.FullName -Raw -Encoding UTF8

    if ($file.Name -match "_Define\.h|_Function\.h") { continue }
    if ($content -notmatch "\bclass\s+") { continue }

    # Namespace
    $ns = ""
    if ($content -match "NS_BEGIN\s*\(\s*(\w+)\s*\)") {
        $ns = $Matches[1]
    }

    # Class extraction
    if ($content -notmatch "class\s+(?:\w+_DLL\s+)?(\w+)\s*(?:final|abstract)?\s*(?::([^{]*))?{") {
        continue
    }
    $className = $Matches[1]
    $inheritance = if ($Matches[2]) { $Matches[2] } else { "" }

    # Base class
    $baseClass = ""
    if ($inheritance -match "public\s+(\w+)") {
        $baseClass = $Matches[1]
    }

    # static Create
    $hasCreate = $content -match "\bstatic\s+.*\bCreate\s*\("
    if (-not $hasCreate) { continue }

    # Clone
    $hasClone = [bool]($content -match "\bClone\s*\(")

    # Category
    $category = Get-Category $baseClass

    # Member extraction (own members only)
    $members = @()
    $classStart = $content.IndexOf("{", $content.IndexOf("class"))
    if ($classStart -ge 0) {
        $depth = 0
        $bodyEnd = $classStart
        for ($i = $classStart; $i -lt $content.Length; $i++) {
            if ($content[$i] -eq '{') { $depth++ }
            elseif ($content[$i] -eq '}') {
                $depth--
                if ($depth -eq 0) { $bodyEnd = $i; break }
            }
        }

        $body = $content.Substring($classStart + 1, $bodyEnd - $classStart - 1)
        $lines = $body -split "`n"
        $currentAccess = "private"

        foreach ($line in $lines) {
            $trimmed = $line.Trim()

            if ($trimmed -match "^public:") { $currentAccess = "public"; continue }
            if ($trimmed -match "^private:") { $currentAccess = "private"; continue }
            if ($trimmed -match "^protected:") { $currentAccess = "protected"; continue }

            if ([string]::IsNullOrWhiteSpace($trimmed)) { continue }
            if ($trimmed.StartsWith("//") -or $trimmed.StartsWith("/*")) { continue }
            if ($trimmed -match "\(") { continue }
            if ($trimmed.StartsWith("using ") -or $trimmed.StartsWith("NO_COPY") -or $trimmed.StartsWith("DECLARE_")) { continue }

            if ($trimmed -match "^([\w:<>]+(?:\s*<[^>]+>)?)\s+(m_\w+)\s*(?:=\s*\{?\s*([^};]*?)\s*\}?)?\s*;") {
                $memberType = $Matches[1].Trim()
                $memberName = $Matches[2].Trim()
                $memberDefault = if ($Matches[3]) { $Matches[3].Trim() } else { "" }

                $members += @{
                    name = $memberName
                    type = $memberType
                    default = $memberDefault
                    access = $currentAccess
                }
            }
        }
    }

    $classes[$className] = @{
        namespace = $ns
        base_class = $baseClass
        has_create = $hasCreate
        has_clone = $hasClone
        category = $category
        members = $members
    }

    Write-Host "  [OK] $className [$category] ($($members.Count) own members, base: $baseClass)"
}

# ============================================================
# 상속 멤버 병합 (inherited_members 필드 추가)
# ============================================================
foreach ($className in @($classes.Keys)) {
    $info = $classes[$className]
    $inherited = Get-InheritedMembers $info.base_class $classes
    $info["inherited_members"] = $inherited

    # Transform 멤버 자동 포함 (GameObject 계열)
    if ($info.category -eq "gameobject") {
        $transformMembers = $engineClasses["Transform"].members
        $info["transform_members"] = $transformMembers
    }
}

# ============================================================
# 출력
# ============================================================
$output = @{
    version = "1.0"
    classes = $classes
}

$json = $output | ConvertTo-Json -Depth 6
[System.IO.File]::WriteAllText($OutputJson, $json, [System.Text.Encoding]::UTF8)

Write-Host ""
Write-Host "  [RESULT] $($classes.Count) classes exported to $(Split-Path $OutputJson -Leaf)"
Write-Host "========================================" 
