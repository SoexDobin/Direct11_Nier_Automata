param(
    [Parameter(Mandatory=$true)]
    [string]$InputDir,
    
    [Parameter(Mandatory=$true)]
    [string]$OutputDir
)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "RTTR Registration Code Generator" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

Write-Host "========== [RTTR] Scanning directory: $InputDir ==========" -ForegroundColor Yellow
Write-Host "========== [RTTR] Output directory: $OutputDir ==========" -ForegroundColor Yellow
Write-Host ""

if (-not (Test-Path $InputDir)) {
    Write-Host "===== [ERROR] Input directory not found: $InputDir" -ForegroundColor Red
    exit 1
}

# 출력 디렉터리 생성
if (-not (Test-Path $OutputDir)) {
    New-Item -ItemType Directory -Path $OutputDir -Force | Out-Null
}

$count = 0

# 모든 .h 파일 처리
Get-ChildItem -Path $InputDir -Filter "*.h" | ForEach-Object {
    $headerFile = $_
    $content = Get-Content $headerFile.FullName -Raw -Encoding UTF8
    
    # 클래스 이름 추출
if ($content -match 'class\s+(?:ENGINE_DLL\s+)?(\w+)\s*(?::\s*public\s+(\w+))?') {
    $className = $Matches[1]
    $parentClass = if ($Matches[2]) { $Matches[2] } else { "" }  # ← 없으면 빈 문자열
    
    # RTTR 등록 코드 생성 (wstring 사용)
    $rttrCode = @"
#include "$className.h"
#include <rttr/registration>
using namespace rttr;
using namespace Engine;
RTTR_REGISTRATION
{
    registration::class_<$className>(L"$className")
        (
            rttr::metadata("parent", L"$parentClass")
        )
        .constructor<>()
    ;
}
"@
    
    # 파일 저장
    $outputFile = Join-Path $OutputDir "${className}_rttr.cpp"
    $rttrCode | Out-File -FilePath $outputFile -Encoding UTF8
    
    Write-Host "==========      [OK] ${className}_rttr.cpp (parent: $parentClass)" -ForegroundColor Green
    $count++
}
    else {
        Write-Host "==========      [!WARN] $($headerFile.Name) (class not found)" -ForegroundColor Yellow
    }
}

Write-Host ""
Write-Host "========== [RTTR] Total $count files generated ==========" -ForegroundColor Cyan
