---
type: design
date: 2026-08-28
status: active
owner: user
repo_path: C:\Users\a9018\Desktop\Direct11_Nier_Automata
aliases:
  - Model Animation Resource Pipeline Plan
  - ModelConverter Model Client Editor Pipeline
---

# Model·Animation 리소스 파이프라인 복구 계획

## Summary

이 작업은 Phase 5가 아니라, 리소스 유실로 중단된 Phase 4 `Pl0000` GUI 검증을 재개하기 위한 선행 복구 작업이다. 실행 순서는 다음으로 고정한다.

1. 기존 `ModelConverter → Model → Client/Editor` 경로를 확장해 `.model`과 클립별 `.anim`을 분리한다.
2. GUID로 식별하는 Animation Preset JSON을 Editor에서 제작하고 Client에서 Model Prototype에 적용한다.
3. 기존 `ClientSettingManager`가 `ProjectSetting/ModelSettings.xlsx`를 직접 읽어 여러 Model Prototype을 등록한다. 사용자가 CSV로 내보내는 단계는 두지 않는다.
4. `Pl0000` 생성·이동·전투·destroy에 필요한 최소 리소스만 복구한다.
5. Phase 4 GUI·process restart round-trip·rollback 검증을 완료한다.
6. Phase 5 기존 UI/Editor 실행·편집 흐름 리팩토링을 진행한다.
7. 마지막에 Client와 Editor를 실제 실행하여 최초 런타임 오류를 수집하고, 이번 변경 범위의 오류는 수정 후 재실행한다.

새 AssetDatabase, AnimationManager, Registry, Context, Service를 선행 도입하지 않는다. 기존 `Converter`, `Model`, `Animation`, `ResourceManager`, `Game`, `ClientSettingManager`, `ModelViewer` 함수와 소유권을 먼저 수정·확장한다.

## 2026-08-28 확장 범위와 기본 결정

사용자가 기준 스켈레탈 모델을 추출해 제공하는 것을 입력 계약으로 두고, 이후 작업을 애니메이션·텍스처 추출과 Expedition식 상수 버퍼 규약 도입까지 확장한다. 답변이 필요한 선택지는 다음 기본값으로 고정해 첫 수직 슬라이스를 진행한다.

- 첫 캐릭터는 `pl0000` 원본 리소스를 현재 `p10000` Model Prototype에 적용한다.
- 애니메이션은 전체 `.mot` 일괄 변환보다 Idle, Walk, Run, Jump, Attack, Evade, Hit, Dead 대표 8~10개를 먼저 통과시킨다.
- `.mot`를 Engine이나 ModelConverter에서 직접 파싱하지 않는다. 기존 NieR2Blender2NieR의 Blender Action import를 배치 자동화하고, 동일 스켈레톤의 FBX를 기존 ModelConverter 입력으로 사용한다.
- 첫 애니메이션은 in-place를 기본으로 하며 실제 이동은 기존 gameplay movement가 소유한다. `.anim`의 root motion 필드는 보존해 이후 정책 전환 여지를 유지한다.
- 상수 버퍼는 `Camera`, `Object`, `Material`, `Light` 네 그룹부터 도입하고 `Shader_VtxAnimMesh.hlsl` 하나로 검증한다.
- `g_BoneMatrices[512]`는 첫 수직 슬라이스에서 기존 Effects11 matrix array 경로를 유지한다. 공통 버퍼 규약과 스키닝 데이터 경로를 동시에 바꾸지 않는다.
- 새 ShaderManager나 상수 버퍼 Registry를 만들지 않고 기존 `Shader`, `Pipeline`, `Material`, `Light`와 각 렌더 객체의 책임을 확장한다.

## Resource and Shader Execution Plan

### Batch R1 — Resource Fixture and Contract

1. 사용자가 제공한 기준 모델의 파일 경로, bone 이름 집합, bind pose, 좌표축, 단위와 60 FPS를 fixture manifest에 기록한다.
2. `pl0000`의 대표 `.mot` 8~10개와 첫 머티리얼 하나를 선택한다.
3. 출력 이름은 runtime Animation 이름과 1:1 대응하도록 고정하고 원본 hash·MOT 이름을 추적 정보로 보존한다.
4. 원본 `D:\NierRes\data`와 `cpk_unpacked` 입력은 읽기 전용으로 유지하고 중간 결과는 `D:\NierRes\work` 아래에 둔다.

완료 조건:

- 기준 skeleton과 대상 MOT/texture 목록이 명시되어 있다.
- 재실행해도 같은 입력이 같은 출력 이름과 경로를 만든다.

### Batch R2 — Texture Extraction and Semantic Mapping

1. 이미 추출된 `pl0000.wta`와 `pl0000.wtp`를 기존 NieR2Blender2NieR WTA/WTP importer 입력으로 사용한다.
2. 전체 DDS를 저장소에 복사하지 않고 기준 모델 머티리얼이 참조하는 hash만 선별한다.
3. `texture hash → material → semantic slot` 매핑을 기록한다.
4. DDS dimension, format, mip count, alpha와 `sRGB/Linear` 사용 규약을 검사한다.
5. 첫 엔진 슬롯은 `BaseColor`, `Normal`, `ORM`, `Emissive`, `Opacity`로 고정하고, 원본 NieR packed channel이 확인되지 않은 값은 추측하지 않고 미해결로 기록한다.
6. 누락 슬롯에는 엔진 기본 SRV를 바인딩해 이전 draw의 texture가 잔존하지 않게 한다.

완료 조건:

- 첫 머티리얼에 사용되는 DDS와 슬롯 의미가 명시되어 있다.
- BaseColor는 sRGB, Normal/ORM은 Linear로 샘플링되는지 확인할 수 있다.
- 참조되지 않는 대량 추출 파일은 저장소와 Vault에 들어오지 않는다.

### Batch R3 — Animation Extraction and Conversion

1. Blender에 기준 armature를 한 번 로드하고 기존 MOT importer로 선택 clip을 Action으로 가져온다.
2. 누락 bone mapping과 비어 있는 channel을 실패 로그로 남긴다.
3. 동일 bone name, axis, unit, 60 FPS로 animation FBX를 clip별 출력한다.
4. ModelConverter에 기존 `ExportModel` 흐름을 재사용하는 `animation-only` CLI 모드를 추가해 불필요한 `.model` 출력을 생략한다.
5. FBX를 `.anim`으로 변환하고 magic/version, duration, tick rate, channel name, root motion, keyframe 범위를 검증한다.
6. 통과한 clip만 AnimationPreset JSON에 순서대로 등록한다.

완료 조건:

- 대표 clip 전부가 현재 Model bone name으로 remap되어 재생된다.
- skeleton 불일치와 중복 clip은 Model 상태를 변경하지 않고 실패한다.
- 첫/끝 pose, loop clip 경계와 in-place 이동이 확인된다.

### Batch S1 — Engine Constant Buffer Convention

1. Engine shader include 원본에 `CameraBuffer`, `ObjectBuffer`, `MaterialBuffer`, `LightBuffer`를 선언한다.
2. C++에는 `ConstantBuffer` enum, enum 순서와 일치하는 buffer name 배열, `alignas(16)` CPU mirror 구조체를 둔다.
3. 모든 mirror 구조체에 16-byte size `static_assert`를 둔다. HLSL matrix는 `row_major`, CPU matrix는 현재 DirectX `Matrix` 메모리 계약을 유지한다.
4. `Shader::Initialize_Prototype()`에서 존재하는 effect constant buffer handle만 이름으로 cache한다.
5. `Shader::Bind_CBufferData<T>()`가 기존 `Shader` 소유의 dynamic `ID3D11Buffer`를 `WRITE_DISCARD`로 갱신하고 effect buffer에 연결한다.
6. shader가 특정 buffer를 선언하지 않은 경우는 선택적 계약으로 보고 성공 처리하되, 잘못된 enum·size·null data는 실패시킨다.
7. 기존 개별 matrix/raw/SRV API는 미전환 shader 호환을 위해 유지한다.

완료 조건:

- C++/HLSL buffer 이름과 layout이 한 규약으로 연결된다.
- clone된 Shader는 effect handle을 공유하되 자신의 dynamic GPU buffer를 소유한다.
- size가 다른 같은 buffer type 재사용은 안전하게 재할당되거나 명확히 실패한다.

### Batch S2 — Animated Mesh Vertical Slice

1. `Shader_VtxAnimMesh.hlsl`이 공용 constant-buffer include를 사용하도록 변경한다.
2. Animated Model draw caller는 World를 `ObjectCB`, View/Proj/Inverse/CameraPosition을 `CameraCB`로 묶어 전달한다.
3. 첫 단계 MaterialCB는 tint, emissive, normal strength, opacity cutoff, AO/roughness/metalness scale과 texture-present mask를 제공한다.
4. LightCB는 기존 `LIGHT_DESC`를 GPU layout으로 변환해 전달한다.
5. Bone matrix와 SRV binding은 기존 경로를 유지해 상수 버퍼 전환의 회귀 범위를 제한한다.
6. 수직 슬라이스가 통과한 후 StaticMesh, Deferred, Terrain/Effect 순서로 이관한다. 기존 문자열 변수는 소비 shader가 이관된 뒤에만 제거한다.

완료 조건:

- AnimatedMesh 한 개가 새 Camera/Object/Material/Light 규약으로 렌더된다.
- 여러 객체를 연속 draw해도 이전 객체의 상수와 texture가 잔존하지 않는다.
- 기존 non-animated shader는 변경 없이 계속 동작한다.

### Batch V1 — Verification Gate

1. 변경된 Effects11 HLSL을 실제 로드 경로에서 compile한다.
2. solution-level Engine, Client, Editor `Debug|x64` target을 빌드한다.
3. D3D11 debug layer 또는 RenderDoc이 사용 가능하면 constant buffer size와 draw별 binding을 확인한다.
4. Client/Editor에서 기준 모델, 대표 animation, BaseColor/Normal 방향, alpha cutoff를 확인한다.
5. 실패 시 C++ compile, HLSL compile, resource mapping, skeleton mapping을 분리해 최초 실패 지점을 기록한다.

완료 조건:

- Engine/Client/Editor 빌드와 실제 AnimatedMesh shader compile이 성공한다.
- 대표 animation과 첫 material의 runtime 검증 결과가 기록되어 있다.
- 이후 shader migration과 전체 MOT batch 여부를 판단할 근거가 남는다.

## 2026-08-29 적용 현황

완료:

- Batch S1: 기존 `Shader`가 effect의 `MaterialBuffer`, `CameraBuffer`, `ObjectBuffer`, `LightBuffer` handle을 cache하고, 타입드 CPU mirror를 동적 D3D11 constant buffer로 갱신하는 규약을 적용했다.
- CPU/HLSL layout은 각각 64/272/64/96 bytes로 고정했으며 C++ `static_assert`와 FXC dump의 offset·size가 일치한다.
- Batch S2: `Shader_VtxAnimMesh.hlsl`을 공용 include로 전환하고 기존 bone matrix array는 유지했다. `Pipeline`, `Material`, World matrix binding은 새 buffer가 있는 shader에만 새 경로를 사용하고 기존 shader는 기존 Effects11 variable 경로를 유지한다.
- Batch R3 일부: `ModelConverter.exe --animation-only <asset...>`를 추가했다. animation channel이 있으나 skinned mesh가 없는 입력도 읽고 `<Stem> Animation/*.anim`만 출력한다.
- 임시 GLTF fixture 검증에서 `Move.anim` 한 개와 `NANM` magic을 확인했고 `.model`·`.json`이 생성되지 않음을 확인했다.
- solution-level Engine, Client, Editor, ModelConverter `Debug|x64` 빌드와 실제 `Shader_VtxAnimMesh.hlsl` FXC compile이 성공했다.

남은 입력·검증:

- `Client/bin/resources/Models/p10000/`에는 현재 `NMDL` version 2, skeletal=true, bone 230, mesh 30, material 12, animation 0인 `p10000.model`과 DDS 6개가 있다. 이는 runtime 목적지 fixture로 보존하되 source armature와 texture semantic 계약이 확인된 것으로 간주하지 않는다.
- Batch R1/R2와 R3 실제 clip 변환은 사용자가 제공할 `pl0000` 기준 skeletal FBX/GLTF, 선택 MOT 목록, 추출 texture/material source를 입력으로 시작한다.
- Blender MOT import·bone mapping·in-place clip 정리, BaseColor/Normal/ORM/Emissive/Opacity semantic 확정과 실제 파일 배치는 아직 수행하지 않았다.
- Client/Editor 실제 실행, 다중 AnimatedMesh draw 간 상태 잔존, texture 방향·alpha cutoff, 대표 animation 재생은 실제 리소스가 들어온 뒤 V1 runtime gate에서 확인한다.
- `LightCB` CPU/HLSL 규약은 선언됐지만 첫 AnimatedMesh shader가 조명을 소비하지 않으므로 실제 light upload는 조명 pass를 이 규약으로 이관할 때 연결한다.

## Verified Current State

- Git 이력에는 ModelConverter 생성, 정적 메시 바이너리, Animation FBX 입출력, Engine Model 파싱, animation 함수 변경과 vcpkg 통합이 순차적으로 존재한다.
- 현재 `MODEL_HEADER`는 `NMDL`, version 1이며 Bone, Mesh, Material, Animation을 한 `.model` 파일에 저장한다.
- `Model::Initialize_Prototype()`은 `.model`에서 모든 Animation을 즉시 읽는다.
- 기존 작업 트리의 `ClientSettingManager`에 있던 `Client/bin/Resources/ModelSettings.csv` 임시 동기화 경로는 제거했다. 모델 설정의 authoring 원본은 `ProjectSetting/ModelSettings.xlsx` 하나이며, CSV 파일은 읽거나 생성하지 않는다.
- `ModelViewer`는 로드된 Model과 내장 Animation 목록을 선택할 수 있지만 Preset 제작·저장 기능은 없다.
- `Client/bin/resources/Models/p10000/`에는 skeletal `p10000.model`과 DDS 6개가 있으나 `.anim`과 source armature는 없다. 그 밖의 주요 모델 폴더는 대부분 비어 있어 전체 `ModelSettings.json` runtime 로드는 아직 완전한 fixture가 아니다.
- `D:\NierRes`에는 원본 CPK와 해제 데이터가 있으나 현재 조사에서 `.fbx`, `.model`, `.anim` 복구 원본은 확인되지 않았다. 원본 `D:\NierRes\data`는 수정하거나 덮어쓰지 않는다.

## Data Contract

### Model

- 새 Converter 출력은 `<ModelName>.model`에 Bone, Mesh, Material만 기록한다.
- 기존 `.model` version 1의 내장 Animation은 읽기 호환을 유지한다.
- 새 포맷은 명시적 version과 header 검증을 사용하고, 잘못되거나 잘린 파일을 부분 적용하지 않는다.

### Animation

- 모델과 같은 디렉터리에 `<ModelName> Animation/` 폴더를 만든다.
- FBX Animation이 10개면 해당 폴더에 10개의 `.anim` 파일을 생성한다.
- `.anim`은 한 클립의 이름, duration, tick rate, root motion, Channel name과 keyframe을 저장한다.
- 다른 Model에 적용할 때 Converter의 원본 bone index를 신뢰하지 않고 Channel name을 대상 Model bone name으로 다시 매핑한다.
- 필요한 Channel bone이 없거나 중복되면 클립 전체 로드를 실패시키고 Model 상태를 변경하지 않는다.
- 동일 이름 클립의 중복 정책은 오류로 처리한다.

### Animation Preset

- Preset은 `ProjectSetting/AnimationPreset/`에 JSON으로 저장한다.
- 문서 identity는 `AssetGuid`이며 파일명이나 경로를 identity로 사용하지 않는다.
- Preset은 표시 이름과 순서가 있는 `.anim` 경로 목록을 가진다.
- 범용 AssetRegistry나 새 Manager를 만들지 않고 기존 Client 설정 로딩과 Model API가 GUID→Preset 문서 경로를 전달한다.
- Editor의 기존 `ModelViewer`를 확장해 Model 선택, `.anim` 추가·제거·순서 변경, 저장·불러오기, preview를 제공한다.

### Client Model Table

- authoring 원본은 `ProjectSetting/ModelSettings.xlsx`이며 사용자가 CSV로 내보내지 않는다.
- Workbook의 `Models` worksheet를 기존 Client 영역의 `ClientSettingManager`가 직접 읽는다. 새 Excel Manager나 AssetDatabase를 추가하지 않는다.
- Header는 `Level`, `Tag`, `Path`, `PosX`, `PosY`, `PosZ`, `RotX`, `RotY`, `RotZ`, `ScaleX`, `ScaleY`, `ScaleZ`, `AnimationPresetGuid`를 사용한다. 열은 header 이름으로 찾으므로 순서를 바꿀 수 있지만, 필수 header 누락과 중복은 오류다.
- 한 data row는 Model Prototype 하나다. `Level`, `Tag`, `Path`는 필수이고 Transform 빈 셀은 기존 기본값, `AnimationPresetGuid`는 선택값으로 처리한다.
- Excel이 존재하면 모든 row를 먼저 검증한 뒤 `ProjectSetting/ModelSettings.json`을 임시 파일에서 원자적으로 교체한다. 실패 시 기존 JSON을 보존한다.
- authoring Excel이 없는 배포 환경에서는 기존 JSON cache가 있을 때만 경고 후 사용한다. Excel과 JSON이 모두 없으면 실패한다.
- XLSX→JSON 동기화와 JSON→Engine 등록의 실패 worksheet·cell/row·경로·tag·Preset GUID를 로그에 명시한다.

## Execution Batches

### Batch M1 — Format and Converter

1. `Engine_ModelStruct.h`에 최소 Animation file header와 version 상수를 추가한다.
2. `Converter::WriteModelFile()`은 새 출력에서 Animation payload를 제외한다.
3. 기존 `ExportModel()`을 확장해 model과 animation 파일을 한 변환 작업으로 출력한다.
4. 파일명에 사용할 수 없는 clip 문자, 빈 clip 이름, 중복 이름을 결정적으로 정리·검증한다.
5. 변환 결과에 model 1개와 animation N개가 생성됐는지 출력 로그로 확인한다.

완료 조건:

- Animation 0개 FBX는 `.model`만 생성한다.
- Animation N개 FBX는 `.model` 1개와 `.anim` N개를 생성한다.
- 기존 v1 `.model`은 Engine에서 계속 읽을 수 있다.

### Batch M2 — Engine Model and Animation Loading

1. 기존 `Model::Initialize_Prototype()`의 v1 내장 Animation 경로를 유지한다.
2. 기존 `Model`에 `.anim` 한 개 및 목록을 추가하는 API를 확장한다.
3. Channel name을 현재 Model bone index로 remap한 뒤 기존 `Animation::Create()`를 사용한다.
4. 추가 성공 후 `m_Animations`, `m_AnimationNames`, `m_NumAnimation`을 한 번에 갱신한다.
5. `ResourceManager`와 `Game`은 기존 Model Prototype을 조회해 Preset Animation을 적용하는 최소 façade만 제공한다.

완료 조건:

- 같은 Skeleton의 외부 `.anim`을 로드·재생할 수 있다.
- 호환되지 않는 Skeleton은 명확한 오류와 함께 기존 Model을 보존한다.
- Clone된 Model도 Prototype에 적용된 Animation 목록을 동일하게 가진다.

### Batch M3 — Client Excel and Preset Application

1. 임시 `Sync_ModelJson_FromCSV()`를 `Sync_ModelJson_FromExcel()`로 교체하고 `ProjectSetting/ModelSettings.xlsx`의 `Models` worksheet를 직접 읽는다.
2. XLSX의 workbook/relationship/shared-string/worksheet 구조를 검증하고 header 이름으로 열을 매핑한다.
3. 빈 셀과 잘못된 float/GUID, 필수 header·cell 누락, 중복 header를 worksheet와 cell/row 정보와 함께 검증한다.
4. 모든 row가 성공한 경우에만 기존 `ModelSettings.json`을 원자적으로 교체한다.
5. 기존 `Load_Model_FromJson()`이 Model Prototype 등록 후 Preset JSON을 읽어 `.anim` 목록을 적용한다.
6. 여러 Model row와 동일 Preset 재사용을 검증한다.

완료 조건:

- `ModelSettings.xlsx`만 수정해 복수 Model Prototype과 Animation Preset을 등록할 수 있고 CSV export가 필요 없다.
- 잘못된 한 row 때문에 기존 `ModelSettings.json`이 부분 갱신되거나 이전에 성공한 Model 설정이 조용히 바뀌지 않는다.
- 실패 원인은 worksheet, cell/row, level, tag, path, Preset GUID 중 필요한 정보를 포함한다.

### Batch M4 — Existing Editor Extension

1. 기존 `ModelViewer`에 Preset name, AssetGuid, clip path 목록 편집을 추가한다.
2. 기존 Model/Animation 선택과 preview 동작을 재사용한다.
3. 저장 경로는 `ProjectSetting/AnimationPreset/`로 고정하고 JSON schema를 검증한다.
4. 새 Editor manager나 별도 asset database를 추가하지 않는다.

완료 조건:

- Editor에서 Preset 생성→저장→재로드→preview가 가능하다.
- Preset GUID는 재저장해도 유지된다.
- 다른 Model이 같은 Preset을 참조할 수 있고 skeleton 불일치가 표시된다.

### Batch M5 — Pl0000 Minimum Resource Recovery

1. `Pl0000`과 8개 CodeDefined Child가 요구하는 model tag, animation name, texture와 shader를 코드에서 역추적한다.
2. 복구 가능한 원본과 변환 입력의 위치를 확인한다. 원본 CPK는 읽기 전용으로 유지한다.
3. 전체 게임 데이터가 아니라 생성·이동·전투·destroy에 필요한 최소 세트만 `Client/bin/Resources`에 배치한다.
4. ModelSettings XLSX/JSON과 Animation Preset을 최소 세트에 맞춘다.

완료 조건:

- `Pl0000` Root와 8개 Child가 생성된다.
- 기본 이동·전투 Animation과 필요한 Material/Texture가 로드된다.
- 누락 리소스가 있다면 정확한 tag와 경로가 로그에 출력된다.

### Batch M6 — Phase 4 Completion Gate

1. 동일 Root 재로드 전후 ObjectGuid와 RuntimeObjectId 역할을 비교한다.
2. process restart `save → load → save` round-trip을 수행한다.
3. schema 불일치 fixture의 load 실패와 기존 Scene rollback을 확인한다.
4. CodeDefined Child의 reparent·reorder·delete 차단을 확인한다.
5. 부모·자식 Active 조합과 frame당 Update/Render 호출 횟수를 확인한다.
6. `Pl0000` 생성·이동·전투·destroy를 확인한다.

### Batch M7 — Phase 5

- Phase 4 검증이 모두 통과한 뒤 기존 Phase 5 PLAN의 Batch 5A와 5B를 순서대로 수행한다.
- Phase 5 범위와 완료 조건은 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/에디터_리플렉션_프리팹_개편_계획]]을 따른다.

### Batch M8 — Build and Runtime Error Gate

1. solution-level target으로 Engine, Client, Editor `Debug|x64`를 빌드한다.
2. Client를 실제 실행해 시작부터 Level 진입 또는 최초 오류까지 로그·메시지·종료 코드를 수집한다.
3. Editor를 실제 실행해 ProjectSetting 동기화, Model 등록, Preset 로드와 viewport preview 또는 최초 오류까지 확인한다.
4. 이번 파이프라인 변경에서 발생한 런타임 오류는 수정하고 같은 실행을 반복한다.
5. 외부 원본 부재나 수동 GUI 조작처럼 자동으로 해소할 수 없는 차단은 정확한 경로와 재현 단계로 기록한다.

완료 조건:

- 빌드 성공만으로 완료 처리하지 않는다.
- Client와 Editor의 실제 실행 결과, 최초 런타임 오류 또는 정상 도달 지점이 기록되어 있다.
- 수정한 오류는 재실행 결과까지 남긴다.

## Implementation Status — 2026-08-28

- Batch M3의 XLSX 직접 입력 소스를 적용했다. `ClientSettingManager`가 minizip과 pugixml로 `Models` worksheet를 읽고, 모든 행 검증 성공 후에만 `ModelSettings.json`을 원자적으로 교체한다.
- `Sync_ModelJson_FromCSV()` 호출은 `Sync_ModelJson_FromExcel()`로 교체했고, Excel 동기화 실패를 Loader가 조용히 건너뛰지 않도록 실패 전파를 연결했다.
- 기존 JSON cache가 있고 Excel이 없는 배포 환경은 경고 후 JSON을 사용하며, 둘 다 없으면 로딩을 실패시킨다.
- Solution-level Client와 Editor `Debug|x64` 빌드는 성공했다.
- 실제 `ProjectSetting/ModelSettings.xlsx`가 저장소에 없어 workbook 파싱, JSON 교체, 복수 Model Prototype 등록의 런타임 검증은 남아 있다. CSV fixture로 대체 검증하지 않는다.

## Resource Import Status — 2026-08-30 WD Shared Textures

- NieR2Blender2NieR drag-and-drop DTT import는 같은 폴더의 DAT만 찾던 기존 경로를 보완해 `cpk_unpacked/data*` 형제 폴더에서 동일 group과 stem의 분할 DAT를 찾는다. `data004/wd1/g11021.dtt`는 `data003/wd1/g11021.dat`을 companion으로 사용한다.
- `wd*` 월드 DTT를 import할 때 전역 `data*`를 섞지 않고 해당 DTT와 companion DAT가 속한 data family 안에서만 완전한 `wda` DAT/DTT basename 쌍을 찾는다. 현재 구조에서 `data003/004`의 `wd1/2`는 `ga0001/2`, `data013/014`의 `wd1~5`는 `ga0000`, DAT/DTT가 함께 있는 `data100/wd5`는 추가 WDA 없이 처리한다. 이후 다른 data family에 WDA 쌍이 생기면 같은 폴더 규칙으로 자동 인식한다.
- texture resolve 우선순위는 현재 DTT의 local `textures` → 자동 발견한 `wda/nier2blender_extracted/<archive>.dtt/textures` 및 기존 `*.wtp_extracted` → 사용자가 등록한 texture preference directory다.
- 개발본 `D:\NierRes\tools\NieR2Blender2NieR_DragDrop`과 Blender 4.5 설치본의 `datImportOperator.py`, `utils/util.py`에 같은 patch를 적용했다. 각 원본은 같은 위치의 `.bak_20260830_shared_wda`로 보존했다.
- Blender 4.5.13 background 검증에서 `data004/wd1`, `data004/wd2`, `data014/wd1~5`, `data100/wd5` 대표 DTT 모두 companion DAT와 family-scoped WDA 집합이 정확히 선택됨을 확인했다. 기존 `g11021` 검증에서는 local texture 128개, shared texture 324개, local에 없는 shared hash 324개의 WDA fallback resolve를 확인했다. `g11021` WMB가 직접 참조한 131개 hash는 모두 local WTA에도 존재했으므로 이 fixture에서 실제 shared-only material reference는 0개였고, 공유 검색 경로는 local에 없는 WDA hash 집합으로 별도 검증했다.
- 원본 CPK `D:\NierRes\data`는 변경하지 않았고 생성물은 `cpk_unpacked/.../nier2blender_extracted` cache에만 기록했다.

## Build Commands

- Engine: `MSBuild.exe NieRAutomata.sln /t:Engine /p:Configuration=Debug /p:Platform=x64 /m /v:minimal`
- Client: `MSBuild.exe NieRAutomata.sln /t:Client /p:Configuration=Debug /p:Platform=x64 /m /v:minimal`
- Editor: `MSBuild.exe NieRAutomata.sln /t:Editor /p:Configuration=Debug /p:Platform=x64 /m /v:minimal`

## Risks

- 현재 저장소와 `D:\NierRes`에서 변환 입력용 `Pl0000` FBX가 확인되지 않았다. 원본 WMB/WTA/WTP에서 FBX·Texture로 만드는 별도 복구 단계가 필요할 수 있다.
- 기존 `.model`은 struct raw write를 사용하므로 ABI와 padding에 민감하다. 이번 범위에서는 기존 v1 호환을 유지하되 새 `.anim`에는 magic과 version, read 검증을 적용한다.
- 외부 Animation 호환성은 모델 이름이 아니라 Channel bone name 집합으로 판단해야 한다.
- Animation Preset은 AssetGuid를 사용하지만 범용 AssetDatabase 도입 근거로 확대하지 않는다.
- Git 작업 트리에 EngineCore·Client·Editor의 사용자 변경이 다수 존재하므로 관련 파일의 기존 변경을 보존하며 작은 patch로 진행한다.

## Links

- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/에디터_리플렉션_프리팹_개편_계획]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/rules/기존_구조_우선_설계_규칙]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/dev-tasks/SoExDobin_git_history_summary]]
