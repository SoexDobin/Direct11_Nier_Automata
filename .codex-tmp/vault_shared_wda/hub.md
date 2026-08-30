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
- 마지막 확인 브랜치: `Refactor/EngineCore`
- 마지막 확인 커밋: `7be8c8b`
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

2026-08-28 Debug|x64에서 다음 명령을 검증했다.

- Engine: `MSBuild.exe NieRAutomata.sln /t:Engine /p:Configuration=Debug /p:Platform=x64 /m /v:minimal`
- Client: `MSBuild.exe NieRAutomata.sln /t:Client /p:Configuration=Debug /p:Platform=x64 /m /v:minimal`
- Editor: `MSBuild.exe NieRAutomata.sln /t:Editor /p:Configuration=Debug /p:Platform=x64 /m /v:minimal`

Engine build의 post-build가 Engine public header와 lib/DLL/PDB를 EngineSDK, Client, Editor로 복사한다. Engine과 Client project를 직접 빌드하지 말고 solution-level target을 사용한다. Client vcxproj 직접 빌드는 `$(SolutionDir)` 누락으로 Unity 입력 경로가 잘못 계산된다.

## Current Status

주 프로젝트와 외부 리소스 작업 공간의 역할을 등록했다. 저장소 루트 `AGENTS.md`와 이 허브를 양방향 진입점으로 연결했으며, 이후 프로젝트 작업은 Git 상태와 Vault 문맥을 복원한 뒤 시작한다.

2026-08-28 ModelConverter→Model→Client/Editor 복구를 Phase 4 완료 전 선행 작업으로 확정했다. 새 출력은 `.model`과 `<ModelName> Animation/*.anim`을 분리하고, 기존 Model·ResourceManager·Game·ClientSettingManager·ModelViewer를 확장해 GUID Animation Preset과 `ProjectSetting/ModelSettings.xlsx` 직접 입력 기반 복수 Model Prototype 등록을 연결한다. 사용자가 CSV로 내보내는 단계는 두지 않는다. `Pl0000` 최소 리소스 복구 뒤 Phase 4 GUI를 완료하고 Phase 5로 진행하며, 최종 게이트에는 Client·Editor 실제 실행과 최초 런타임 오류 수집·수정·재실행을 포함한다. 상세 실행 계획은 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/모델_애니메이션_리소스_파이프라인_계획]]을 따른다.

2026-08-29 리소스 복구 PLAN의 셰이더·변환기 기반을 먼저 적용했다. 기존 `Shader`·`Pipeline`·`Material`에 Camera/Object/Material/Light typed constant-buffer 규약을 추가하고 AnimatedMesh 셰이더 한 개를 수직 적용했으며, CPU/HLSL 크기 64/272/64/96 bytes와 FXC layout을 대조했다. ModelConverter에는 skinned mesh가 없는 animation input도 처리하는 `--animation-only` 모드를 추가해 `.anim`만 생성하도록 했다. Engine·Client·Editor·ModelConverter solution-level `Debug|x64`와 AnimatedMesh FXC compile, `NANM` fixture 출력 계약은 통과했다. 현재 runtime 폴더의 `p10000.model`은 version 2 skeletal, bone 230, mesh 30, material 12, animation 0이며 DDS 6개와 함께 존재하지만 `.anim`과 source armature는 없다. 실제 texture semantic·MOT 추출과 Client/Editor runtime gate는 기준 skeletal FBX/GLTF와 대상 clip 입력 후 진행한다.

2026-08-30 NieR2Blender2NieR DTT import에 분할 archive와 월드 공유 텍스처 fallback을 적용했다. `data004/wd1/g11021.dtt` import는 `data003/wd1/g11021.dat`을 자동 companion으로 사용한다. WDA는 모든 `data*`를 전역으로 섞지 않고 DTT와 companion DAT가 속한 family만 연결한다. 따라서 `data003/004`의 `wd1/2`는 `ga0001/2`, `data013/014`의 `wd1~5`는 `ga0000`, `data100/wd5`는 추가 WDA 없이 처리하며, 다른 data family도 같은 폴더 규칙으로 자동 인식한다. Blender 4.5.13 background에서 각 그룹 대표 DTT의 companion과 WDA 범위, `g11021`의 local 128개와 shared 324개 texture 및 local에 없는 WDA hash resolve를 확인했다. 개발본과 설치본은 각각 백업 후 동일 patch를 적용했으며 원본 CPK는 변경하지 않았다. 상세 상태는 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/모델_애니메이션_리소스_파이프라인_계획]]에 기록했다.

2026-08-30 `CityOfRuinEntry.fbx` 916개 메시 이름을 전수 분석하고 raw `g11021.col`, `Layout.lay`, `materials.json`과 대조했다. `HexGround` 401개는 raw COL batch와 정확히 대응하고 FBX 왕복 중 collision triangle 34개가 탈락했다. 나머지는 WMB visual 515개이며 명시적 internal LOD1 3개를 제외하면 문서상 g11021 object 512개와 일치한다. raw LAY의 model 9종, asset 26개, 추가 instance 372개는 FBX에 남아 있지 않다. material 50개는 sidecar와 1:1 일치하고 131 texture hash 중 128개가 local/WDA cache에서 resolve된다. 따라서 단일 FBX 직접 변환 대신 visual, raw COL, raw LAY를 분리하는 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/CityOfRuinEntry_월드_임포트_계획]]을 작성했고 전체 이름 근거는 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/sources/CityOfRuinEntry_FBX_메시_인벤토리]]에 보존했다.

2026-08-29 머티리얼 셰이더 그래프는 전체 `materials.json`과 참조 DDS가 확보되기 전까지 잠정 방향만 유지한다. `materials.json`은 import 힌트로 보존하고, Roughness·Metallic·AO 같은 공통 표면 출력과 복수 TextureBinding 입력을 분리한다. Engine은 공통 MaterialCB·표면 규약·노드 함수를 제공하고 Client는 Cloth·Weapon·Skin·Hair·Eye·Map 셰이더를 조합하며, Editor는 graph 검증·저장·셰이더 생성을 담당한다. CB layout, mask 채널, semantic, node와 Material asset schema는 전체 리소스 통계와 채널 검증 뒤 확정한다. 상세 초안은 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/머티리얼_셰이더_그래프_잠정_방향]]을 따른다.

2026-08-28 Batch M3의 XLSX 직접 입력 소스를 적용했다. `ClientSettingManager`가 `Models` worksheet를 직접 파싱해 전체 행 검증 후 `ModelSettings.json`을 원자적으로 교체하고, Loader는 동기화 실패를 전파한다. Client와 Editor solution-level `Debug|x64` 빌드는 성공했다. 실제 `ProjectSetting/ModelSettings.xlsx`가 없어 workbook 기반 런타임 검증은 남아 있으며 CSV fixture로 대체하지 않는다.

2026-08-27 Phase 1 Registry cutover를 적용했다. TypeGuid/PropertyGuid/hash bridge를 제거하고 RTTR RegisteredName 기반 Registry, RuntimeTypeId 기본 Prototype lookup, Engine 소유 reflected Prototype 자동 등록, type/property registered name 직렬화, ObjectManager 등록 전 중복 검사와 Scene load rollback을 연결했다. Engine과 Client Debug|x64 빌드는 성공했으며 GUI Scene round-trip은 남아 있다. 상세 구현과 다음 Phase 2 진입 조건은 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/에디터_리플렉션_프리팹_개편_계획]]을 따른다.

2026-08-27 Phase 2 Unified ChildGraph를 적용했다. 기존 `GameObject` Parent/OrderedChildren에 ObjectGuid·StableChildKey 파생 lookup과 cycle/duplicate/reparent/subtree destroy 검증을 통합하고, `ContainerObject::m_PartObjects`를 제거해 모든 Part 소유·순서·조회를 공통 관계로 전환했다. Part 생성은 RTTR RegisteredName 기본 Prototype 경로를 사용하며, Component attach/lifecycle/Post_Load 우회와 Scene `parentObjectID` fallback도 제거했다. Engine과 Client Debug|x64 빌드 및 Pl0000 8개 고유 StableChildKey 정적 검증은 성공했고 GUI 런타임 회귀 검증은 남아 있다.

2026-08-28 Child core 리팩토링으로 `m_Children`만 Child 저장소로 남기고 두 파생 map을 선형 `Find_Child`로 치환했다. 상태 없는 `ContainerObject` 계층과 `GAMEOBJECTTYPE::CONTAINER`를 제거해 `Entity`를 `GameObject`에 직접 연결했으며, Part 생성/검색/순회를 기존 `Add_Child` 확장·`Find_Child`·`Get_Children`으로 통합했다. `PartObject::Get_Owner()`는 공통 Parent를 반환하고 별도 Owner/raw parent matrix는 제거했으며 bone/socket 결합 계산은 유지했다. Engine·Client·Editor Debug|x64 빌드는 성공했다. Part full snapshot은 결정적 Child GUID와 StableChildKey 기반 기존 Child 매칭을 함께 구현할 다음 단계까지 기존 Serializer 제외를 유지한다.

2026-08-28 Phase 4 실행 계획을 현재 호출 순서에 맞춰 확정했다. 새 Child 저장소·service·GUID Clone·friend 없이 기존 `Game::Instantiate_Internal`, `GameObject::Initialize`/`Add_Child`, `PrefabManager`, `LevelSerializer`, Editor `Hierarchy`를 확장한다. 저장 Root GUID는 PrefabManager의 동기식 1회용 상태를 통해 base Initialize에서 먼저 확정하고, Child GUID는 `Parent GUID + StableChildKey + RegisteredName`으로 파생한다. Serializer는 Root만 생성한 뒤 저장 Child와 factory Child를 StableChildKey로 매칭하고 수·순서·typeName·파생 GUID·Component 구조를 직접 비교한다. 별도 StructureVersion/fingerprint 대신 기존 schemaVersion만 갱신하며, Part full snapshot과 기존 Editor action의 CodeDefined 구조 잠금을 같은 단계에서 검증한다.

2026-08-28 Editor 제작 project asset의 저장 루트를 `ProjectSetting/`으로 확정했다. Scene hierarchy는 `ProjectSetting/Scene/LevelData_<index>.json`, Prefab은 `ProjectSetting/Prefab/<PrefabName>.json`, Editor 전용 프로젝트 설정은 `ProjectSetting/Editor/`를 사용한다. 이후 Material/UI/Animation 등은 실제 writer/reader가 생길 때만 종류별 폴더를 추가한다. 기존 Editor `PathManager`, Client `ClientSettingManager`, Engine `PrefabManager`와 경로 비종속 `LevelSerializer`를 확장하며 별도 AssetDatabase·manifest·ProjectPathManager는 만들지 않는다. 루트의 기존 `LevelData_*.json`과 `SceneData.json`은 새 경로 검증 전 보존하고 자동 fallback/migration 없이 추후 승인된 정리 대상으로 둔다.

2026-08-28 Phase 4 소스를 적용했다. PrefabManager의 1회용 pending GUID를 기존 Game/GameObject 초기화 흐름에 연결해 Root GUID를 factory Child 생성 전에 확정하고, 기존 Add_Child가 Parent GUID·StableChildKey·RegisteredName으로 결정적 Child GUID를 생성한다. LevelSerializer schemaVersion 2는 Root만 생성한 뒤 기존 factory Child/Component를 직접 매칭하고 Part full snapshot을 적용하며, Editor Hierarchy는 CodeDefined subtree 구조 변경을 차단한다. Editor와 Client Level 경로는 `ProjectSetting/Scene/LevelData_<index>.json`으로 통일했다. Engine/Client/Editor solution target Debug|x64 빌드와 정적 검증은 성공했으며, GUI Pl0000 round-trip과 실패 fixture는 남아 있어 Phase 4 전체 완료로는 표시하지 않는다.

2026-08-28 현재 런타임 검증용 리소스 데이터 유실로 Phase 4 Pl0000 GUI 검증을 보류했다. 먼저 Model 구조와 리소스 파싱 경로를 별도 PLAN으로 조사·복구하고 Pl0000 실행에 필요한 최소 리소스를 되살린 뒤, Phase 4 GUI·재시작 round-trip·rollback·Hierarchy 잠금을 통과해야 Phase 5에 착수한다. Phase 5는 기존 실행 경로 정리와 UI authoring을 5A/5B로 분리한다. 5A는 Expedition_158 MainEditor 비교, ObjectManager 단일 frame owner, Level 수동 UI 호출 제거, subtree delete/incremental Refresh를 다루고, 5B는 기존 UIObject/GameObject/LevelSerializer/Editor owner를 확장해 UI Root+3단계 Child, 편집 action, 최소 Undo/Redo, 저장·복원, 제한된 ObjectGuid binding을 검증한다. 범용 reference와 Prefab 저장소는 Phase 6에 유지한다.

2026-08-24 NotebookLM MCP 인증 데이터를 초기화하고 재로그인했다. `get_health`의 `authenticated` 값은 `false`로 남았지만, `direct11-nier-automata` 노트북에 대한 실제 `ask_question` 질의가 Gemini 2.5 기반 답변과 세션 ID를 정상 반환했으므로 기능 접근은 검증됐다. 이후에는 건강 상태 플래그만 보고 재인증하지 말고 최소 질의 성공 여부로 접근성을 판단한다.

`D:\NierRes\data`의 CPK 24개를 각각 `D:\NierRes\cpk_unpacked\<CPK 이름>`으로 원본 보존 방식으로 해제했다. 입력 24개와 출력 폴더 24개의 대응, 빈 폴더 없음, 총 132,807개 파일과 56,761,421,898바이트 출력을 검증했다. 상세 결과와 선택적 하위 변환 경고는 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/dev-tasks/CPK_일괄_해제]]를 따른다.

2026-08-25 RTTR 구조를 코드 기준으로 추적하고 현재 작업 트리의 관련 파일 172개를 SHA-256으로 기준화했다. Engine 등록 생성물 68개 중 25개만 실제 컴파일되며, Client 등록·Editor 메타데이터·직렬화가 서로 다른 진실 공급원을 가진다. 상세 구조, 누락 목록, ID 경계와 개선 단계는 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/RTTR_리플렉션_구조와_한계_분석]]을 따른다.

RTTR 라이브러리 자체의 버전 함정, API 계약, `variant`·metadata·상속·DLL 등록 수명주기, 유지·포크·교체 판단 기준은 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/sources/RTTR_라이브러리_개편_참고]]에 분리했다. 개편 시 RTTR 직접 호출을 기존 TypeCatalog를 치환한 얇은 Registry 뒤로 격리하고 별도 descriptor 계층은 필요성이 확인되기 전까지 만들지 않는다.

2026-08-27 목표 구조는 ObjectGuid 기반 공통 Parent/Children 관계(`ChildGraph`는 문서상 모델명)와 두 가지 제작 정책을 유지한다. Object는 현재 실행의 RuntimeTypeId와 Engine 내부 `uint64 RuntimeObjectId`를 가지며 ObjectGuid는 저장되는 GameObject만 소유한다. RTTR RegisteredName은 영속 타입과 기본 Prototype 결정자이며 숫자 RuntimeTypeId는 저장하지 않는다. Component에는 Invalid ObjectGuid를 두지 않고 실행 중 RuntimeTypeId로 중복을 검사하며 `Owner ObjectGuid + Component RTTR RegisteredName`으로 영속 위치를 식별한다. Property는 RTTR registered property name, 저장 설정 asset은 PrefabGuid로 식별한다.

월드 Container는 `CodeDefined` 구조로 Client factory가 자식과 Component를 만들고 Editor는 구조를 잠근 채 reflection 값만 편집한다. `Pl0000`의 기존 `m_PartObjects` ownership map은 제거하고 `partTag`를 `StableChildKey`로 승격해 기존 GameObject Parent/Children 관계가 소유·순서·lookup을 담당한다. UI는 `EditorDefined` UIObject 노드를 추가·삭제·재부모화·순서 변경할 수 있게 하되 각 UIObject 내부 Component 구조는 코드 고정으로 유지한다. `HpBarWorldUI` 같은 target 의존 UI도 별도 RuntimeBound 계층으로 분리하지 않고 Hierarchy drag-and-drop으로 설정하는 GUID `ObjectRef` property를 사용한다. UI Update/Render/Input은 Level 직접 호출과 ObjectManager 경로의 중복을 분석해 기존 owner 하나로 통합하고, 새 scheduler class는 기존 경계로 해결할 수 없을 때만 검토한다. Prefab은 Unity식 원본 연결 없이 배치 시 복사되며, 월드 CodeDefined 객체는 값 snapshot, EditorDefined UI는 구조와 값 snapshot을 저장한다.

2026-08-27 Phase 2 실행 소유권은 기존 ObjectManager frame 함수의 Root-only 진입과 `Get_Children()` 부모 우선 재귀로 확정했다. 재귀 중에는 각 객체의 기존 virtual Update/Render 계열을 호출하고 별도 GameObject `_Hierarchy` 함수나 `friend ObjectManager`를 추가하지 않는다. Child는 ObjectManager에 GUID lookup·삭제 대상으로 계속 등록되지만 독립 frame 진입점으로 호출되지 않는다. 별도 계층 Active 변수 없이 기존 객체별 `m_IsActive`를 사용하므로 inactive Parent는 전체 subtree 실행을 막고, inactive Child는 Parent가 active여도 자신의 subtree만 실행을 막는다.

2026-08-27 Phase 3 실행 계획은 새 reflection/serializer 계층을 추가하지 않고 기존 Registry와 `00. System\03. Serializer` 필터의 LevelSerializer를 공통 Scene/Prefab codec으로 마감하는 것으로 좁혔다. RTTR 생성 대상과 프로젝트 편입을 일치시키고, `schemaVersion: 1`을 World 변경 전에 선검증하며, `typeName` 하나로 기본 Prototype을 선택하고 저장 ObjectGuid를 등록 전에 확정한다. 최소 PrefabManager는 PrefabGuid·경로·문서 lookup과 LevelSerializer 호출만 소유하고, 실제 Object 생성은 기존 Game/Registry/PrototypeManager 경로를 사용한다. StructureVersion/fingerprint와 Pl0000 결정적 Child GUID는 Phase 4, Prefab 배치/remap/Editor action은 Phase 6으로 남긴다.

2026-08-27 Phase 3 Registry-backed serialization을 적용했다. `LevelSerializer`는 `schemaVersion: 1`, ObjectGuid-keyed `objects`, `roots`, ordered `children`, registered type/property name만 저장하고 로드 전 schema·GUID·계층·type/property·기본 Prototype을 검사한다. ObjectGuid는 기존 Clone 단계에서 등록 전에 확정하며 등록 후 restore/re-key와 friend를 제거했다. RTTR generator의 무표시 header 생성을 차단하고 실제 대상 중 빌드 가능한 34개를 편입했으며, stale `ResourceManager`/`StateMachine` 생성물은 편입하지 않았다. Game-owned 최소 PrefabManager는 PrefabGuid/path lookup과 기존 LevelSerializer 위임만 담당한다. Engine/Client Debug|x64 빌드와 금지 경로 정적 검증은 성공했고 GUI round-trip/실패 fixture 실행은 남아 있다.

Editor Update/Render의 최종 구조는 Expedition33 프로젝트에서 Editor 자체 화면과 Scene/Viewport의 호출 순서, pause/step, render target, mutation safe point를 실제 코드로 분석한 뒤 다시 확정한다. 비교 전에는 새 coordinator/scheduler를 최종 호출 계약으로 간주하지 않으며, 분석 결과를 이 프로젝트의 기존 Game/manager/ObjectManager/Renderer 호출 순서에 우선 반영한다. 새 타입은 기존 경계로 해결되지 않는 문제가 확인될 때만 검토한다.

2026-08-26 Expedition33(`Expedition_158`)의 Base, Component, GameObject, GameInstance, Object_Manager, MainEditorApp, EditorInstance, Scene_View를 1차 대조했다. 작은 Base, Editor와 runtime delta 분리, Play/Pause/Frame Step, Game Draw 후 viewport capture와 Editor UI 합성, CommandHistory/pending mutation 개념은 참고할 가치가 있다. GameInstance의 큰 service 집합은 Singleton 폐기나 새 Context 도입 근거로 사용하지 않는다. Update/Render orchestration과 safe point만 참고하고, 이 프로젝트는 `Game` Singleton과 `GAME_INSTANCE`, 기존 manager를 유지한 채 불필요한 forwarding·중복 ownership·직접 mutation·prototype 우회 경로만 정리한다.

현재 `Game`/`GAME_INSTANCE`는 엔진의 공용 Singleton 진입점으로 유지한다. 개편은 이를 여러 수명 객체로 분해하거나 새 단일 Context로 감싸지 않는다. Object의 32비트 ID·등록 후 ID 변경, Container별 중복 ownership, Editor의 직접 ObjectManager mutation과 전체 clear, ID·생성 계약을 우회하는 prototype/참조 복원 경로처럼 확인된 문제만 기존 Game/ObjectManager/GameObject/Serializer 경계를 최적화해 제거한다.

2026-08-27 `Refactor/EngineCore`에 ID 개편의 첫 수직 단위를 적용했다. Game Singleton과 기존 manager 구조는 유지했고, Guid strong type, RuntimeObjectId, ObjectGuid live registry, ObjectGuid 기반 템플릿 검색·삭제·Component 추가, Scene 2.1 GUID 저장·복원, Owner별 Component 중복 차단을 추가했다. 이때 들어간 TypeGuid/hash/PropertyGuid와 TypeCatalog TypeGuid map은 RTTR RegisteredName 결정으로 폐기할 전환 bridge다. 다음 단계는 기존 TypeCatalog를 `00. System\05. GUID`의 Registry로 치환하고, Engine 정적 RTTR 등록과 `Client::Register_Client_Reflection()` 뒤 전체 타입을 검증해 기존 PrototypeManager에 기본 Prototype을 자동 등록하며, serializer를 RTTR type/property registered name으로 정리하는 것이다. 별도 Context·ComponentService·PrototypeKey/VariantKey·선행 atomic transaction은 계획에서 제외했다.

## Known Risks

- CPK 원본과 추출 결과는 대용량이므로 Git과 Vault 관리 대상에서 제외한다.
- `nier_cli --autoExtractChildren`은 CPK 내부 DAT/PAK 등 자식 형식까지 연쇄 추출하므로 단순 CPK 해제보다 파일 수와 용량이 크게 늘어날 수 있다.
- 기존 출력 폴더에 재실행하면 파일이 덮어써질 수 있으므로 출력 경로 존재 여부를 먼저 확인한다.
- MRuby `.scp.bin` 디컴파일은 Python 기반 실행 환경이 없으면 실패하며, 일부 JIS 문자열과 WTA 파일은 `nier_cli 1.4.0` 파서 `RangeError`가 발생할 수 있다. 실패 원본은 보존하고 로그를 기준으로 별도 재처리한다.
- 주 저장소의 기존 `.vscode/settings.json` 삭제 상태를 복구하거나 변경하지 않는다.
- RTTR Engine 생성 파일은 자동으로 프로젝트에 편입되지 않으며 기존 출력은 재생성되지 않는다. 생성물 존재만으로 런타임 등록 완료로 판단하지 않는다.
- `rttr::type::get_id()`와 32비트 `std::hash` 기반 Object ID는 영속 식별자로 사용할 수 없다. 저장·네트워크·빌드 간 ID에는 명시적인 안정 ID 체계가 필요하다.
- `client_metadata.json`은 현재 소비 경로가 확인되지 않았고 실제 Client 등록 타입 33개가 `other`로 분류된다. Editor 리플렉션 경로를 통합하거나 제거하기 전까지 드리프트를 전제로 검증한다.
- Editor가 생성된 객체를 ObjectManager에 넣은 뒤 ObjectID를 바꾸는 경로가 있어 ObjectID 인덱스가 stale 상태가 될 수 있다. ID는 삽입 전에 확정하거나 단일 reindex API를 거쳐야 한다.
- `Pl0000`의 Part는 현재 `unordered_map`과 별도 Owner·부모 행렬 포인터로 관리된다. 개편 시 이 ownership 저장소를 보존하지 않고 공통 ChildGraph 규약으로 교체해야 한다.
- 현재 Screen UI의 Level별 수동 Update/Render와 ObjectManager 실행 경로는 중복 여부를 확인하고 기존 owner 하나로 통합해야 한다. 새 scheduler class는 선행 조건이 아니다.
- GUID ObjectRef drag-and-drop binding은 저장뿐 아니라 타입 검증, 삭제/remap, unresolved 표시, load 후 resolve cache Refresh까지 한 계약으로 구현해야 한다.
- 현재 Editor의 `Delete_Selected()` 전체 clear 방식은 기존 ObjectManager/GameObject 정리 순서를 사용하는 ObjectGuid subtree 삭제와 영향 범위 incremental Refresh로 교체해야 한다.
- 기존 AssetBrowser의 prototype 목록은 영구 프리팹 데이터베이스가 아니며 현재 런타임 레벨의 로드 상태에 의존한다. 프리팹 저장소와 혼용하지 않는다.
- 현재 `ID_Helper`는 `CoCreateGuid()` 결과를 XOR해 32비트로 축소하므로 GUID 기반처럼 보여도 128비트 정체성과 충돌 저항성을 보존하지 않는다.
- RTTR numeric type id는 실행마다 달라질 수 있으므로 저장하면 안 된다. 저장된 RTTR RegisteredName은 불변 schema key로 취급하고 C++ class rename이나 generator 재실행이 등록 문자열을 조용히 바꾸지 않도록 해야 한다.
- 2026-08-27 Engine·Client C++/header 전수 조사에서 114개 선언과 112개 고유 RegisteredName이 확인됐으며 `StateMachine`, `Terrain`이 중복 등록되어 있다. Registry finalize와 Prototype 자동 등록 전에 정리해야 한다.
- 기존 데이터 폐기는 확정된 설계 방향이지만 실제 LevelData/SceneData 물리 삭제는 새 writer와 제작 경로 검증 후 정확한 대상을 확인하는 별도 작업으로 수행한다.
- `bin` 전체를 ignore하면 런타임 리소스와 HLSL/HLSLI 원본까지 숨겨져 프로젝트가 복구 불가능해질 수 있다. `bin`은 추적 가능 상태로 유지하고 재생성 가능한 빌드 산출물만 이름·확장자로 제외한다. 상세 규칙은 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/errors/bin_리소스와_빌드_산출물_분리]]를 따른다.

## Decisions

- 주 프로젝트는 `C:\Users\a9018\Desktop\Direct11_Nier_Automata`로 유지한다.
- `D:\NierRes`는 리소스 추출 도구와 게임 데이터 전용 외부 작업 공간으로 유지한다.
- 저장소 `AGENTS.md`는 코드 작업 진입점이고, 이 허브는 지속 문맥 진입점으로 유지한다.
- Object는 RuntimeTypeId와 RuntimeObjectId를 가지며 ObjectGuid는 저장되는 GameObject에만 둔다. RTTR RegisteredName은 영속 타입/default Prototype schema key이고 RTTR numeric type id와 RuntimeObjectId는 저장하지 않는다.
- ObjectGuid는 Scene·Prefab의 Hierarchy GameObject 노드와 외부 객체 참조를 추적한다. Component는 실행 중 RuntimeTypeId로 중복을 검사하고 `Owner ObjectGuid + Component RTTR RegisteredName`으로 영속 위치를 식별한다.
- 기존 TypeCatalog는 병렬 유지하지 않고 EngineCore의 얇은 Registry로 치환한다. Registry는 RTTR type/property/method/metadata를 감싸되 Prototype, live Object, Asset을 소유하지 않는다.
- Engine 정적 등록과 `Client::Register_Client_Reflection()`이 끝난 뒤 Registry를 refresh/finalize한다. RegisteredName 중복·누락·factory·상속 관계를 검증하고 concrete Engine·Client Object의 기본 Prototype을 기존 PrototypeManager에 자동 등록한다.
- RTTR RegisteredName과 property registered name은 불변 저장 schema key다. C++ rename과 generator 재실행은 기존 등록 문자열을 보존하고 이름 변경은 명시적 migration 또는 데이터 재제작으로 처리한다. TypeGuid/PropertyGuid manifest는 사용하지 않는다.
- 모든 월드와 UI GameObject는 공통 Parent/Children `ChildGraph`와 World-scoped ObjectGuid Registry를 사용한다. 기존 ObjectManager frame 함수는 Root만 진입점으로 잡고 `Get_Children()`을 재귀 순회해 각 객체의 기존 virtual frame 함수를 호출한다.
- 월드 Container는 `CodeDefined`다. factory가 Child와 Component 구조를 만들고 Editor는 구조 변경을 막으며 값만 편집한다.
- CodeDefined Child는 기존 `partTag` 같은 `StableChildKey`와 Parent ObjectGuid, Child RTTR RegisteredName으로 결정적 ObjectGuid를 만든다. 별도 ChildSlotGuid는 두지 않는다.
- 일반 UIObject 노드는 `EditorDefined`다. Editor에서 Child 추가·삭제·재부모화·순서 변경을 허용하지만 UIObject 내부 Component 구조는 코드 고정으로 유지한다.
- `WorldUIObject` target은 Hierarchy drag-and-drop 가능한 GUID ObjectRef로 binding한다. Binding 유무로 UI를 별도 RuntimeBound 제작 계층으로 분리하지 않는다.
- 월드 CodeDefined Prefab은 전체 property 값 snapshot, EditorDefined UI Prefab은 ordered child 구조와 전체 property 값 snapshot을 저장한다.
- Prefab 배치 후 원본 연결, Apply/Revert/Unpack, override, PrefabLocalGuid를 사용하지 않는다.
- ObjectGuid/RuntimeObjectId는 ObjectManager 등록 전에 확정하고 기존 Instantiate/ObjectManager의 발급·검증·등록·실패 정리 순서를 우선 최적화한다. 별도 batch rollback은 이 방식으로 해결되지 않는 부분 상태 문제가 재현될 때만 검토한다.
- Editor mutation은 ObjectGuid command queue와 dirty scope 기반 incremental Refresh로 처리하고 전체 Level clear를 사용하지 않는다.
- UI Update/Render/Input은 현재 호출 그래프를 분석해 기존 owner 하나로 통합한다. 전환 후 Level별 수동 중복 호출과 EngineCore의 Editor 우회 mutation·중복 관리 코드만 제거한다.
- Editor Update/Render 최종안은 Expedition33의 Editor Screen과 Scene/Viewport 두 실행 문맥을 분석하고 프로젝트 대응표를 작성한 뒤 확정한다.
- Expedition33 1차 비교 결과, Editor/runtime delta·Play/Pause/Step·viewport capture 순서는 참고한다. Singleton 자체는 배제하지 않고 base Device/Context 보관, prototype clone, 불명확한 snapshot restore처럼 검증되지 않은 내부 구조만 채택하지 않는다.
- `Game` Singleton과 `GAME_INSTANCE`는 유지한다. `WorldContext`나 단일 `Context` class를 선행 도입하지 않고 기존 manager가 상태와 수명을 유지한다. 모든 전역 호출을 옮기지 않고 불필요한 forwarding과 급조된 우회 경로만 제거한다.
- RTTR RegisteredName이 클래스와 기본 Prototype/Factory를 결정하고 Registry가 현재 RuntimeTypeId로 resolve한다. Model/Texture/Navigation 파일은 AssetGuid, 저장된 설정은 PrefabGuid로 구분하며 PrototypeKey/VariantKey는 현재 계획에 두지 않는다.
- 전환 완료 후 32비트 ID와 분리된 Legacy reflection 경로를 Runtime에서 제거한다. 기존 LevelData/SceneData는 변환하지 않고 새 GUID 스키마로 다시 제작한다.
- Editor의 Client 직접 링크와 ClientApp 실행 구조를 유지하며 별도 Client module interface는 우선 도입하지 않는다.
- Engine 내부 live lookup에는 `uint64 RuntimeObjectId`를 사용하고 별도 dense TypeIndex는 프로파일 결과가 필요성을 증명할 때만 검토한다.
- Registry는 등록 완료 뒤 `RegisteredName ↔ rttr::type ↔ current RuntimeTypeId` resolve를 검증한다. PrototypeManager는 유지하고 기본 Prototype lookup을 RuntimeTypeId로 전환하며 기존 32비트 ObjectID/tag 우회 index만 단계적으로 제거한다.
- Registry, LevelSerializer, PrefabManager 같은 Engine System/Manager는 `Game` Singleton이 각각 한 인스턴스만 소유한다. 다른 manager나 객체는 이를 직접 소유하거나 포인터로 보관하지 않고 `Game` façade를 사용한다. PrototypeManager는 Registry 소유를 내려놓고 Prototype 저장만 담당한다.
- GUID 기반 Clone API는 GameObject에 두지 않는다. 저장 ObjectGuid 복원은 PrefabManager의 동기식 1회용 생성 상태를 기존 `GameObject::Initialize()`가 소비해 Child factory 실행과 ObjectManager 등록 전에 완료한다.
- 같은 GameObject Owner에는 동일 Component RuntimeTypeId를 하나만 허용한다. factory, `Add_Component<T>()`, Scene/Prefab load, duplicate 모두 기존 `GameObject::Add_Component()` 검증을 통과하고 중복이면 상태 변경 없이 실패한다.
- Client와 Editor의 외부 사용 방식은 `GAME_INSTANCE`와 템플릿 API를 유지한다. ID 발급·복원·등록·중복 검증은 Engine 내부에서 수행하고 생성·검색·삭제·Component 추가 API만 점진적으로 ID Registry 기반으로 전환한다.
- 설계는 기존 구조 최적화를 우선한다. 계획서에 없는 상황은 현 코드를 분석해 가장 작은 변경으로 해결하고, 미래 가능성만으로 새 class/service/context/key/registry/transaction을 추가하지 않는다. 확대가 불가피하면 현재 문제, 기존 owner로 해결 불가한 이유, 실제 호출자·수명, 대체할 경로, 검증 방법과 순증 복잡도를 먼저 기록한다.
- 이 프로젝트의 주 목표는 기존 타입과 함수의 책임을 수정·확장하는 리팩토링이다. 변수·함수 추가를 일률적으로 금지하지 않으며, 실제 caller나 불변식이 요구할 때 기존 owner 안에 최소 범위로 추가하고 대체되는 우회·중복 경로를 함께 제거한다.
- Scene hierarchy, Prefab과 이후 Editor 제작 project asset은 `ProjectSetting/<AssetType>/`에 저장한다. Scene은 `Scene/`, Prefab은 `Prefab/`, Editor 전용 설정은 `Editor/`를 사용한다. LevelSerializer는 경로를 소유하지 않고 기존 Editor/Client path owner가 완성 경로를 넘기며, PrefabGuid는 파일 경로가 아닌 문서 identity다.

## Dev Tasks

- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/dev-tasks/CPK_일괄_해제]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/dev-tasks/SoExDobin_git_history_summary]] — 엔진·에디터·클라이언트·렌더링·내비게이션·보스 구현의 Git 근거 정리

## Errors

- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/errors/bin_리소스와_빌드_산출물_분리]] — `bin` 전체 ignore로 리소스·셰이더 원본을 숨기지 않고 재생성 가능한 산출물만 제외하는 규칙

## Rules

- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/rules/기존_구조_우선_설계_규칙]] — 기존 구조 최적화 우선, 근거 없는 상위 구조·키·서비스·transaction 확장 금지

## Design

- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/RTTR_리플렉션_구조와_한계_분석]] — RTTR 생성·등록·소비 구조, 172파일 SHA-256 기준선, ID·직렬화·Editor 한계와 단계별 개선안
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/에디터_리플렉션_프리팹_개편_계획]] — RTTR RegisteredName Registry와 Engine·Client Object 기본 Prototype 자동 등록, type/property 이름 직렬화, ObjectGuid ChildGraph 및 기존 manager 경계 최적화 계획
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/모델_애니메이션_리소스_파이프라인_계획]] — `.model`·클립별 `.anim`, GUID Animation Preset, `ModelSettings.xlsx` 직접 입력 Model Prototype, Pl0000 최소 복구, Phase 4·5 및 실제 런타임 오류 게이트
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/CityOfRuinEntry_월드_임포트_계획]] — g11021 visual 515, raw COL 401, raw LAY 398 placements를 분리하고 material, spatial chunk, Navigation, instance, wd2 LOD를 단계적으로 연결하는 계획
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/머티리얼_셰이더_그래프_잠정_방향]] — 전체 `materials.json` 분석 전 유지할 MaterialCB·복수 texture binding·Engine node·Client shader·Editor graph의 잠정 책임과 최종 확정 게이트
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/sources/CityOfRuinEntry_FBX_메시_인벤토리]] — FBX 916개 전체 메시 이름, base group 15개, COL/LAY/material 원본 대조와 행 단위 vertex·triangle·material·UV 근거
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/sources/RTTR_라이브러리_개편_참고]] — RTTR 0.9.7 설치 실체, 공식 API 계약, DLL 수명주기와 유지·포크·교체 판단 기준

## Handoffs

등록된 문서 없음.
