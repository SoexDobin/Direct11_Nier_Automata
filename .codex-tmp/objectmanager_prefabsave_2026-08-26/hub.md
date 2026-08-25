---
type: project
date: 2026-08-13
status: active
repo_path: C:\Users\a9018\Desktop\Direct11_Nier_Automata
legacy_status: live
---

# Direct11_Nier_Automata

## Summary

NieR:Automata를 모작하는 DirectX 11 기반 C++ 프로젝트.

주 소스 저장소와 대용량 원본 리소스·추출 도구를 분리해서 관리한다. Project Vault에는 경로 역할, 결정, 위험, 작업 요약처럼 재사용할 문맥만 보관한다.

## Repository

주 프로젝트 저장소:

`C:\Users\a9018\Desktop\Direct11_Nier_Automata`

- 저장소 로컬 `AGENTS.md`: `C:\Users\a9018\Desktop\Direct11_Nier_Automata\AGENTS.md`
- 마지막 확인 브랜치: `develop`
- 마지막 확인 커밋: `5e008e3`
- 원격 저장소: `https://github.com/SoexDobin/Direct11_Nier_Automata.git`
- 기존 작업 상태: `.vscode/settings.json` 삭제가 이미 존재함. 사용자 변경으로 취급하고 건드리지 않는다.

## External Resource Workspace

리소스 추출과 데이터 작업 전용 외부 경로:

`D:\NierRes`

- nier_cli 소스: `D:\NierRes\nier_cli-1.4.0`
- 원본 게임 데이터와 CPK: `D:\NierRes\data`
- 이 경로는 주 소스 저장소가 아니며 대용량 리소스·추출 결과를 유지하는 작업 공간이다.
- 원본 CPK를 수정하거나 덮어쓰지 않는다.
- 대용량 추출 결과를 주 Git 저장소나 Vault에 복사하지 않는다.
- 추가 도구를 내려받기 전에 D:에 있는 기존 도구를 우선 확인한다.

## NotebookLM Resource Reference

리소스의 출처·위치·포맷·제작법처럼 로컬 파일이나 사용자가 제공한 명시적 경로만으로 확인할 수 없는 정보를 찾거나 참고해야 할 때 다음 프로젝트 노트북을 NotebookLM MCP로 조회한다. 입력과 절차가 이미 명확한 일반 리소스 작업에는 매번 호출하지 않는다.

`https://notebook.google.com/notebook/88617913-333a-42b4-82bb-6eeb6bac60ea`

- NotebookLM MCP 로컬 ID: `direct11-nier-automata`
- 노트북 답변은 참고 문맥으로 사용하며 저장소 소스, 실제 추출 파일, 도구 출력, 빌드·런타임 증거로 교차 검증한다.
- MCP가 없거나 인증되지 않았으면 그 제한을 알리고, 노트북 문맥이 안전한 판단에 필수인 경우 작업을 중단해 사용자에게 연결 또는 로그인을 요청한다.

2026-08-13 확인 상태:

- CPK 24개, 합계 약 19.78 GiB
- D: 여유 공간 약 857.9 GiB
- `nier_cli-1.4.0`에는 Dart 소스가 있으나 D:에서 `nier_cli.exe` 및 `dart.exe`는 발견되지 않음
- 기존 7-Zip 실행 파일: `D:\NierRes\F-SERVO-1.4.13\assets\bins\7z.exe`
- CPK 일괄 해제 런처: `D:\NierRes\nier_cli-1.4.0\unpack_all_cpk.ps1`

## Codex Entry Flow

1. 주 프로젝트 저장소의 `AGENTS.md`를 읽고 Git 상태를 확인한다.
2. Vault 루트 `AGENTS.md`, `index.md`, `log.md`와 이 허브를 읽는다.
3. 리소스 작업이면 `D:\NierRes`를 외부 작업 공간으로 사용한다.
4. 원본, 도구, 추출 출력의 경계를 확인한 뒤 작업한다.
5. 추가 다운로드나 설치는 기존 도구로 수행할 수 없는 경우에만 사용자 승인 후 진행한다.
6. 재사용할 결정·오류·작업 요약만 Vault에 저장한다.
7. 리소스 관련 정보 탐색이나 외부 참고가 필요하면 NotebookLM MCP의 `direct11-nier-automata` 노트북을 조회하고 로컬 증거와 교차 검증한다.

## Build / Verify

아직 검증된 빌드 명령은 등록되지 않았다. 솔루션 파일은 다음 경로에 있다.

`C:\Users\a9018\Desktop\Direct11_Nier_Automata\NieRAutomata.sln`

## Current Status

주 프로젝트와 외부 리소스 작업 공간의 역할을 등록했다. 저장소 루트 `AGENTS.md`와 이 허브를 양방향 진입점으로 연결했으며, 이후 프로젝트 작업은 Git 상태와 Vault 문맥을 복원한 뒤 시작한다.

2026-08-24 NotebookLM MCP 인증 데이터를 초기화하고 재로그인했다. `get_health`의 `authenticated` 값은 `false`로 남았지만, `direct11-nier-automata` 노트북에 대한 실제 `ask_question` 질의가 Gemini 2.5 기반 답변과 세션 ID를 정상 반환했으므로 기능 접근은 검증됐다. 이후에는 건강 상태 플래그만 보고 재인증하지 말고 최소 질의 성공 여부로 접근성을 판단한다.

`D:\NierRes\data`의 CPK 24개를 각각 `D:\NierRes\cpk_unpacked\<CPK 이름>`으로 원본 보존 방식으로 해제했다. 입력 24개와 출력 폴더 24개의 대응, 빈 폴더 없음, 총 132,807개 파일과 56,761,421,898바이트 출력을 검증했다. 상세 결과와 선택적 하위 변환 경고는 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/dev-tasks/CPK_일괄_해제]]를 따른다.

2026-08-25 RTTR 구조를 코드 기준으로 추적하고 현재 작업 트리의 관련 파일 172개를 SHA-256으로 기준화했다. Engine 등록 생성물 68개 중 25개만 실제 컴파일되며, Client 등록·Editor 메타데이터·직렬화가 서로 다른 진실 공급원을 가진다. 상세 구조, 누락 목록, ID 경계와 개선 단계는 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/RTTR_리플렉션_구조와_한계_분석]]을 따른다.

RTTR 라이브러리 자체의 버전 함정, API 계약, `variant`·metadata·상속·DLL 등록 수명주기, 유지·포크·교체 판단 기준은 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/sources/RTTR_라이브러리_개편_참고]]에 분리했다. 개편 시 RTTR 직접 호출을 프로젝트 Registry/descriptor 뒤로 격리하는 것을 기본 방향으로 삼는다.

2026-08-26 목표 구조를 Object 중심 GUID EngineCore와 코드 고정 구조 기반 full-snapshot Prefab으로 재확정했다. Object는 실행 클래스 RuntimeTypeId, 영구 TypeGuid, Hierarchy GameObject용 ObjectGuid, 매 생성 시 새 InstanceGuid 필드를 가지되 읽기만 제공한다. ObjectManager는 RuntimeTypeId/TypeGuid를 ReflectionRegistry에서 받아 Object에 바인딩하고 모든 Object ID index를 관리하며, ObjectGuid/InstanceGuid 발급·복원까지 전담한다. 기존 ObjectID의 Hierarchy·논리 객체 추적 역할은 128비트 ObjectGuid로 명확히 하고 InstanceGuid는 저장하지 않는다. PropertyGuid와 PrefabGuid는 Object ID가 아니므로 각각 ReflectionRegistry와 PrefabRegistry가 관리한다.

GameObject, Component, Child 구조는 Client factory/container 코드가 미리 만든다. generated `.rttr.cpp`는 base/derived 관계와 typed `PrefabSave` metadata를 등록하고 EngineCore는 base → derived 순으로 effective property 목록을 합성한다. Prefab은 구조를 생성하거나 변경하지 않고 저장 시마다 상속 체인의 `PrefabSave=true` property 현재 값을 전체 snapshot으로 저장·적용한다. Editor는 전체 구조를 Hierarchy/Inspector에 표시하고 값 수정은 허용하지만 Prefab의 Child/Component 추가·삭제는 차단한다. Container와 Child는 같은 Hierarchy GameObject이며 `ObjectGuid → GameObject` map으로 찾는다. 같은 TypeGuid의 Child도 서로 다른 ObjectGuid로 구분하고 Component는 `Owner ObjectGuid + Component TypeGuid`로 찾는다. Editor는 현재처럼 Client 코드와 ClientApp을 직접 실행하고 Launcher는 순수 Client 실행 진입점으로 유지한다. RuntimeHandle/TypeIndex/PrefabLocalGuid와 Unity식 연결 기능은 도입하지 않는다. 전체 계획은 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/에디터_리플렉션_프리팹_개편_계획]]을 따른다.

## Known Risks

- CPK 원본과 추출 결과는 대용량이므로 Git과 Vault 관리 대상에서 제외한다.
- `nier_cli --autoExtractChildren`은 CPK 내부 DAT/PAK 등 자식 형식까지 연쇄 추출하므로 단순 CPK 해제보다 파일 수와 용량이 크게 늘어날 수 있다.
- 기존 출력 폴더에 재실행하면 파일이 덮어써질 수 있으므로 출력 경로 존재 여부를 먼저 확인한다.
- MRuby `.scp.bin` 디컴파일은 Python 기반 실행 환경이 없으면 실패하며, 일부 JIS 문자열과 WTA 파일은 `nier_cli 1.4.0` 파서 `RangeError`가 발생할 수 있다. 실패 원본은 보존하고 로그를 기준으로 별도 재처리한다.
- 주 저장소의 기존 `.vscode/settings.json` 삭제 상태를 복구하거나 변경하지 않는다.
- RTTR Engine 생성 파일은 자동으로 프로젝트에 편입되지 않으며 기존 출력은 재생성되지 않는다. 생성물 존재만으로 런타임 등록 완료로 판단하지 않는다.
- `rttr::type::get_id()`와 32비트 `std::hash` 기반 Object ID는 영속 식별자로 사용할 수 없다. 저장·네트워크·빌드 간 ID에는 명시적인 안정 ID 체계가 필요하다.
- `client_metadata.json`은 현재 소비 경로가 확인되지 않았고 실제 Client 등록 타입 33개가 `other`로 분류된다. Editor 리플렉션 경로를 통합하거나 제거하기 전까지 드리프트를 전제로 검증한다.
- Editor가 생성된 객체를 ObjectManager에 넣은 뒤 ObjectID를 바꾸는 현재 경로는 ObjectID index를 stale 상태로 만들 수 있다. 목표 구조에서는 ObjectManager가 외부 노출 전 ID와 index를 함께 확정하고 임의 setter/reindex 우회 경로를 제거한다.
- 기존 AssetBrowser의 prototype 목록은 영구 프리팹 데이터베이스가 아니며 현재 런타임 레벨의 로드 상태에 의존한다. 프리팹 저장소와 혼용하지 않는다.
- 현재 `ID_Helper`는 `CoCreateGuid()` 결과를 XOR해 32비트로 축소하므로 GUID 기반처럼 보여도 128비트 정체성과 충돌 저항성을 보존하지 않는다.
- TypeGuid를 배포 Client의 첫 실행마다 생성하면 머신별 ID가 달라진다. 최초 발급은 EngineCore reflection generator가 source-controlled 결과로 고정하고 Client/CI는 누락 시 실패해야 한다.
- 기존 데이터 폐기는 확정된 설계 방향이지만 실제 LevelData/SceneData 물리 삭제는 새 writer와 제작 경로 검증 후 정확한 대상을 확인하는 별도 작업으로 수행한다.

## Decisions

- 주 프로젝트는 `C:\Users\a9018\Desktop\Direct11_Nier_Automata`로 유지한다.
- `D:\NierRes`는 리소스 추출 도구와 게임 데이터 전용 외부 작업 공간으로 유지한다.
- 저장소 `AGENTS.md`는 코드 작업 진입점이고, 이 허브는 지속 문맥 진입점으로 유지한다.
- 모든 GameObject/Component의 object identity 필드는 Object에 두고 RuntimeTypeId, TypeGuid, ObjectGuid, InstanceGuid 역할을 분리한다.
- Object는 ID 읽기만 제공한다. ObjectManager가 모든 Object ID의 바인딩·등록·조회·해제를 담당하고 ObjectGuid/InstanceGuid 발급·복원까지 수행한다.
- RTTR `type_id`는 실행 중 TypeGuid 역조회에만 사용하고 저장하지 않는다.
- ObjectGuid는 Scene·Prefab에 저장되는 Hierarchy GameObject 노드와 외부 객체 참조를 추적한다.
- InstanceGuid는 모든 실제 GameObject/Component 생성 때마다 새로 발급하고 저장하지 않는다.
- TypeGuid 발급·manifest·generated `.rttr.cpp`·Registry 검증은 EngineCore만 제공하고 Client와 Editor는 EngineCore API로 설정한다.
- 전환 완료 후 32비트 ID, PrototypeTag, Level PrototypeManager와 분리된 Legacy reflection 생성 경로를 Runtime에서 제거한다.
- 기존 LevelData/SceneData는 변환하지 않고 폐기하며 새 GUID 스키마로 다시 제작한다. Legacy converter와 구 포맷 runtime reader를 최종 구조에 두지 않는다.
- Reflection manifest는 모듈별로 분리하되 schema, TypeGuid/PropertyGuid 발급과 전체 검증은 EngineCore가 제공한다.
- PropertyGuid는 직렬화되거나 Editor에 노출되는 프로퍼티에 자동 발급하며 manifest와 generated code에서 추적한다. PropertyGuid 자체는 ReflectionRegistry가 관리한다.
- generated `.rttr.cpp`는 상속 관계와 typed `MetadataKey::PrefabSave`를 등록하고 EngineCore는 base → derived effective property 목록을 결정적으로 합성한다.
- Prefab/Scene은 RTTR property 전부가 아니라 상속 체인의 `PrefabSave=true` property만 저장한다.
- 일회성 GameObject/Component도 InstanceGuid를 발급하되 Transient 정책으로 Scene 저장에서 제외한다.
- GameObject/Component/Child 구조는 Client factory/container 코드가 구성하고 Prefab은 저장할 때마다 상속 체인의 모든 `PrefabSave=true` PropertyGuid 값을 전체 snapshot으로 저장·적용한다.
- Prefab에서는 Child와 Component 추가·삭제를 허용하지 않고 모든 코드 구성 Object를 Hierarchy/Inspector에서 보고 값 수정할 수 있게 한다.
- Container와 Child는 모두 Hierarchy GameObject이며 같은 ObjectGuid 규칙을 사용한다.
- 같은 TypeGuid의 Child가 여러 개여도 고유 ObjectGuid key로 독립 추적한다.
- Component의 영속 위치는 `Owner ObjectGuid + Component TypeGuid`로 찾고 live Component는 InstanceGuid로 구분한다.
- Prefab 배치 후 원본 연결, Apply/Revert/Unpack, override, PrefabLocalGuid를 사용하지 않는다.
- Prefab 등록·수정 action은 Editor가 요청하고 Engine PrefabSystem이 구조 검증·전체 property snapshot·직렬화·저장을 처리한다.
- Editor의 Client 직접 링크와 ClientApp 실행 구조를 유지하며 별도 Client module interface는 우선 도입하지 않는다.
- RuntimeHandle과 TypeIndex는 초기 EngineCore에서 제외하고 프로파일 결과가 필요성을 증명할 때만 검토한다.

## Dev Tasks

- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/dev-tasks/CPK_일괄_해제]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/dev-tasks/SoExDobin_git_history_summary]] — 엔진·에디터·클라이언트·렌더링·내비게이션·보스 구현의 Git 근거 정리

## Design

- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/RTTR_리플렉션_구조와_한계_분석]] — RTTR 생성·등록·소비 구조, 172파일 SHA-256 기준선, ID·직렬화·Editor 한계와 단계별 개선안
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/에디터_리플렉션_프리팹_개편_계획]] — ObjectManager 단일 ID 권한, 상속 기반 `PrefabSave` property 합성, Hierarchy ObjectGuid mapping, 코드 고정 구조의 full-snapshot Prefab과 Legacy 제거 계획
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/sources/RTTR_라이브러리_개편_참고]] — RTTR 0.9.7 설치 실체, 공식 API 계약, DLL 수명주기와 유지·포크·교체 판단 기준

## Handoffs

등록된 문서 없음.
