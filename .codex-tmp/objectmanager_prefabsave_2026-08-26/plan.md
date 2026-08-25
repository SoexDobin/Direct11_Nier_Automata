---
type: design
date: 2026-08-26
status: active
owner: user
repo_path: C:\Users\a9018\Desktop\Direct11_Nier_Automata
aliases:
  - Object GUID EngineCore Reflection Prefab Plan
  - GUID 엔진 코어 개편
  - Hierarchy ObjectGuid Full Snapshot Prefab
---

# Hierarchy ObjectGuid 기반 EngineCore·전체 값 Prefab 개편 계획

## Summary

목표는 모든 GameObject와 Component에 필요한 타입·논리 객체·실행 인스턴스 ID 필드를 `Object` 단계에서 통일하고, `ObjectManager`가 모든 Object ID의 Object 바인딩·등록·조회·해제를 관리하며 ObjectGuid/InstanceGuid의 발급·복원까지 전담하고, EngineCore가 Reflection·Prefab 값 설정·직렬화를 제공하는 것이다.

Prefab은 Unity식 구조 제작·연결 시스템이 아니다. GameObject, Child GameObject, Component 구조는 Client 코드의 factory와 container가 미리 구성한다. Editor는 이 구조를 Hierarchy와 Inspector에 전부 표시하지만 Prefab 안에서 Child/Component를 추가하거나 삭제하지 못한다. 목적은 reflection property 값을 조정하고, 변경 여부와 관계없이 Prefab 기준의 전체 값을 저장하는 것이다.

Editor는 현재처럼 Client 코드와 `ClientApp` 전체를 함께 실행하면서 제작·설정·디버깅을 담당한다. Launcher는 순수 게임 진입점이고, Client는 Editor가 저장한 값 설정 데이터를 EngineCore로 파싱해 같은 코드 구조에 적용한다.

## Confirmed Runtime Flow

```text
Client source
  ├─ GameObject factory
  ├─ Container child 구성
  ├─ Component 구성
  └─ Reflection property 정의
        │
        ▼
EngineCore Reflection Generator
  ├─ Engine module manifest / generated .rttr.cpp
  └─ Client module manifest / generated .rttr.cpp

Editor.exe
  ├─ EngineCore
  ├─ Client code + ClientApp
  ├─ 코드가 만든 전체 Object 구조 표시
  ├─ 구조 추가/삭제 잠금
  ├─ property 값 조정
  └─ Prefab/Scene 설정 데이터 저장

Launcher.exe
  └─ Client.exe 실행

Client.exe
  ├─ 동일 factory/container로 Object 구조 생성
  ├─ Editor 저장 값 파싱·적용
  ├─ 매 Object에 새 InstanceGuid 발급
  └─ 게임 실행
```

- 별도 `IClientModule`은 현재 필요하지 않다.
- Editor의 Client 직접 링크와 ClientApp lifecycle을 유지한다.
- Client DLL hot reload나 다중 Client 프로젝트가 필요할 때만 별도 모듈 경계를 검토한다.

## Object Identity

### Object가 소유하는 ID

기존 ObjectID가 담당하려던 영구 객체 추적 역할을 `ObjectGuid`로 명확히 하고 128비트 GUID로 교체한다.

```cpp
class Object
{
    RuntimeTypeId m_RuntimeTypeId; // 현재 프로세스의 RTTR type_id
    TypeGuid      m_TypeGuid;      // 클래스의 영구 GUID
    ObjectGuid    m_ObjectGuid;    // 논리 객체·저장 참조 GUID
    InstanceGuid  m_InstanceGuid;  // 현재 생성 인스턴스 GUID
};
```

`Object`는 ID 저장 공간과 읽기 접근만 제공한다. ID를 직접 바꾸는 public setter는 두지 않으며, 모든 ID 쓰기 권한은 `ObjectManager`에만 둔다.

| ID | 의미 | 저장 | 규칙 |
|---|---|---|---|
| `RuntimeTypeId` | 현재 실행의 RTTR 클래스 ID | 저장 금지 | 프로세스마다 다시 구성 |
| `TypeGuid` | Engine/Client 클래스 ID | manifest/Prefab/Scene | 클래스마다 한 번 발급 |
| `ObjectGuid` | Hierarchy GameObject 노드와 객체 참조 ID | Prefab/Scene | 해당 논리 GameObject가 유지되는 동안 보존 |
| `InstanceGuid` | 현재 실행의 실제 Object 인스턴스 ID | 저장 금지 | 생성할 때마다 새 발급 |

Editor에서 Prefab을 배치하거나 새 Scene Object를 만들면 ObjectManager가 ObjectGuid와 InstanceGuid를 발급한다. Client가 저장 데이터를 읽을 때도 ObjectManager가 ObjectGuid를 복원하고 InstanceGuid만 새로 발급한다.

모든 GameObject와 Component가 Object를 상속하므로 ID 필드는 Object 단계에서 제공한다. 다만 ObjectGuid는 Hierarchy GameObject 노드에 사용한다. Component는 Hierarchy 노드가 아니므로 owner GameObject의 ObjectGuid와 자기 Component TypeGuid로 영속 위치를 식별하고, 실제 live Component 구분에는 InstanceGuid를 사용한다. Transient 객체도 InstanceGuid는 갖지만 SceneSerializer가 저장하지 않는다.

### Metadata·Asset ID

객체 인스턴스 필드가 아닌 metadata/asset ID는 해당 EngineCore Registry가 관리한다.

```text
ReflectionRegistry
  RuntimeTypeId → TypeGuid
  TypeGuid → TypeDescriptor
  PropertyGuid → PropertyDescriptor

PrefabRegistry
  PrefabGuid → PrefabSnapshot
```

- PropertyGuid를 Object 인스턴스마다 복사하지 않는다.
- Object는 TypeGuid로 TypeDescriptor와 PropertyGuid 목록을 조회한다.
- PrefabGuid는 설정 asset ID이며 배치된 Object의 identity가 아니다.
- ObjectManager가 관리하는 "모든 Object ID"는 Object에 귀속되는 RuntimeTypeId, TypeGuid, ObjectGuid, InstanceGuid를 뜻한다.
- PropertyGuid와 PrefabGuid는 Object ID가 아니므로 각각 ReflectionRegistry와 PrefabRegistry가 계속 관리한다.

## Reflection Registration

### 모듈별 Manifest

```text
Engine reflection manifest
  Engine TypeGuid / PropertyGuid

Client reflection manifest
  Client TypeGuid / PropertyGuid

Editor reflection manifest
  Editor 자체 reflected type이 필요할 때만 사용
```

manifest는 모듈이 소유하지만 GUID 발급, `.rttr.cpp` 생성, schema와 통합 audit 구현은 EngineCore 한 벌만 존재한다.

EngineCore generator:

1. 모듈별 reflection class/property 검색
2. 기존 TypeGuid/PropertyGuid 조회
3. 신규 class와 저장·Editor 노출 property에 GUID 자동 발급
4. base/derived 관계와 typed property metadata 수집
5. manifest 갱신
6. module별 `.rttr.cpp` 결정적 전체 생성
7. project file 포함 목록 검증
8. 중복 GUID, 잘못된 base/property/factory audit

개발자는 GUID 문자열을 직접 입력하지 않는다. Rename만 generator command로 기존 GUID를 유지한다.

### Runtime Mapping

```text
rttr::type::get<Em0010>().get_id()
  ↔ TypeGuid(Em0010)
  ↔ TypeDescriptor

rttr property MoveSpeed
  ↔ PropertyGuid(Em0010::MoveSpeed)
  ↔ PropertyDescriptor
```

- RTTR ID는 실행 중 클래스 조회에만 쓴다.
- TypeGuid/PropertyGuid만 저장한다.
- CI와 배포 Client는 누락 GUID를 자동 생성하지 않고 실패한다.

### 상속 기반 Prefab 저장 Property

generated `.rttr.cpp`는 각 타입의 base/derived 관계와 property metadata를 함께 등록한다. Prefab과 Scene의 저장 대상은 RTTR에 보이는 모든 property가 아니라, 상속 체인에서 `MetadataKey::PrefabSave = true`로 명시된 property 전체다.

```text
Object
  └─ GameObject
       └─ Character

EffectivePrefabProperties(Character)
  = Object의 PrefabSave property
  + GameObject의 PrefabSave property
  + Character의 PrefabSave property
```

- EngineCore는 `BaseTypeGuids`를 따라 base → derived 순으로 effective property 목록을 합성한다.
- 상속받은 property는 선언 타입의 PropertyGuid를 그대로 사용하며 derived 타입용 GUID를 중복 발급하지 않는다.
- 상속 계층에 같은 이름의 property가 있어도 이름을 저장 key로 쓰지 않는다. 전역적으로 유일한 PropertyGuid를 key로 사용하고 DeclaringTypeGuid는 descriptor audit에 보존한다.
- RTTR의 열거 순서에는 의존하지 않고 manifest의 base 순서와 generated descriptor 순서로 결정적으로 정렬한다.
- `PrefabSave`가 없거나 false인 property는 Editor에 표시되더라도 Prefab/Scene에 저장하지 않는다.
- 포인터, 런타임 캐시, 타이머, 임시 상태는 기본적으로 제외하고 정말 저장할 값에만 `PrefabSave=true`를 선언한다.
- 문자열 metadata key를 직접 쓰지 않고 typed `MetadataKey::PrefabSave`를 generated `.rttr.cpp`와 descriptor가 공유한다.

### TypeDescriptor

```text
TypeGuid
RuntimeTypeId
CanonicalName / Aliases
ModuleOwner
BaseTypeGuids
ObjectKind                GameObject | Component | PlainData
Abstract
EditorSpawnable
PrefabValueEditable
Factory(ObjectCreateContext)
Properties[]              PropertyGuid 포함
  DeclaringTypeGuid
  PrefabSave
EffectivePrefabProperties[]  base → derived 결정적 합성 결과
StructureVersion
```

`StructureVersion`은 코드의 child/component 구성 계약이 바뀌었는지 Prefab/Scene 데이터가 검출하는 버전이다.

## Code-Defined Object Structure

### 구조 생성 책임

Object factory와 Client container가 구조를 만든다.

```text
Factory(TypeGuid)
  → Root GameObject
  → 고정 Component 구성
  → Container의 고정 Child 구성
  → Child별 고정 Component 구성
```

- Prefab 파일이 Component나 Child를 생성하지 않는다.
- 같은 GameObject에 같은 Component TypeGuid를 중복 부착하지 않는다.
- Component는 TypeGuid로 안정적으로 찾는다.
- Container와 Child는 모두 Hierarchy의 GameObject 노드이며 같은 ObjectGuid 규칙을 사용한다.

### Hierarchy ObjectGuid Key-Value Mapping

같은 TypeGuid의 Child가 여러 개여도 각 Hierarchy 노드는 서로 다른 ObjectGuid를 가지므로 충돌하지 않는다.

```text
ObjectRegistry
  ObjectGuid A → Character root
  ObjectGuid B → Body child
  ObjectGuid C → Left Effect child
  ObjectGuid D → Right Effect child

Hierarchy
  Parent ObjectGuid A
    Children [B, C, D]
```

- ObjectGuid가 실제 key이며 GameObject pointer/reference가 value다.
- Container는 child를 보유하는 GameObject일 뿐 별도 ID 종류가 아니다.
- 같은 Child TypeGuid와 같은 표시 이름을 허용해도 ObjectGuid가 다르면 서로 다른 노드다.
- 같은 ObjectGuid 중복 등록은 즉시 실패시킨다.
- 코드가 만드는 child 등록 순서는 결정적으로 유지한다.
- Prefab/Scene snapshot은 이 순서와 ObjectGuid를 함께 저장한다.
- 코드 구조의 추가·삭제·순서가 바뀌면 StructureVersion을 올리고 Prefab/Scene 데이터를 다시 제작한다.

### Hierarchy 표시와 편집

코드로 생성된 모든 Child와 Component는 ObjectManager를 통해 등록하고 Editor Hierarchy/Inspector에 표시한다.

- Root, Child, Component 선택 가능
- reflection property 값 수정 가능
- Transform 수정 가능
- 객체 참조 property 편집 가능
- Prefab 편집 화면에서는 Child 추가/삭제 불가
- Prefab 편집 화면에서는 Component 추가/삭제 불가
- Container가 소유한 parent-child 구조 변경 불가

즉 구조는 read-only이고 값은 editable이다.

## Engine Prefab System

### Prefab 데이터 모델

Prefab은 Object graph의 구조를 새로 정의하는 파일이 아니라, 코드가 만든 고정 구조의 모든 편집 대상 값을 보존하는 전체 snapshot이다.

```text
PrefabSnapshot
  PrefabGuid
  RootTypeGuid
  StructureVersion
  RootObjectGuid
  HierarchyObjects
    ObjectGuid →
      TypeGuid
      Properties
        상속 체인의 PrefabSave=true PropertyGuid → Value
      Components
        Component TypeGuid →
          상속 체인의 PrefabSave=true PropertyGuid → Value
      Children
        순서가 보존된 Child ObjectGuid 목록
```

PrefabSnapshot에는 Child 추가/삭제 명령, PrefabLocalGuid, instance override가 없다. 같은 TypeGuid의 Child가 여러 개여도 각 노드의 ObjectGuid가 key이므로 충돌하지 않는다. Component는 같은 GameObject 안에서 TypeGuid가 유일하다는 규칙에 따라 `Owner ObjectGuid + Component TypeGuid`로 찾는다.

저장 규칙:

- Prefab을 저장할 때 변경된 property만 추출하지 않는다.
- 변경 여부와 관계없이 상속 체인의 `PrefabSave=true` property 현재 값을 매번 전부 기록한다.
- `PrefabSave=false` 또는 metadata가 없는 property는 snapshot에 넣지 않는다.
- 기존 Prefab은 저장된 전체 값이 기준이며, 이후 코드 기본값이 바뀌어도 기존 Prefab 값이 묵시적으로 바뀌지 않는다.
- 새 Prefab은 현재 코드 기본값으로 구조를 만든 뒤 그 전체 값을 최초 snapshot으로 저장한다.
- 저장 중 누락된 ObjectGuid, 중복 ObjectGuid, 알 수 없는 TypeGuid/PropertyGuid, StructureVersion 불일치는 실패시킨다.

### Editor Prefab 편집

1. PrefabGuid 선택 또는 새 Prefab 생성
2. RootTypeGuid factory를 EditorPreview 모드로 실행
3. Client 코드가 Root/Component/Child 전체 구조 생성
4. Engine이 결정적 child 등록 순서로 Prefab의 Hierarchy ObjectGuid를 복원
5. Engine이 기존 Prefab의 전체 property 값을 적용
6. Editor Hierarchy/Inspector에 전체 구조 표시
7. 구조 변경 UI 비활성화
8. 사용자가 property 값만 조정
9. Engine PrefabSystem이 상속 체인의 모든 `PrefabSave=true` property 현재 값을 검증해 전체 snapshot으로 저장

### Drag-and-Drop

```text
PrefabGuid drag
  → RootTypeGuid factory 실행
  → 코드가 Component/Child 구조 생성
  → Prefab 전체 snapshot 적용
  → ObjectManager가 모든 Hierarchy GameObject에 새 ObjectGuid 발급
  → ObjectManager가 Prefab ObjectGuid → 배치 ObjectGuid remap 구성
  → ObjectManager가 모든 실제 GameObject/Component에 새 InstanceGuid 발급
  → snapshot 내부 ObjectGuid 참조 remap
  → Hierarchy 등록
```

배치 후:

- 원본 Prefab과 연결하지 않는다.
- Apply/Revert/Unpack을 제공하지 않는다.
- Prefab 수정이 기존 배치 객체에 자동 반영되지 않는다.
- 배치 객체의 값은 Inspector와 Client 코드에서 추가 조정할 수 있다.
- Scene 저장 시 배치 객체의 현재 완성 값을 저장한다.

### Engine과 Editor 책임

Engine PrefabSystem:

- PrefabGuid/PrefabRegistry
- RootTypeGuid/StructureVersion 검증
- factory 실행
- 코드 구조 탐색
- ObjectManager에 Hierarchy ObjectGuid 복원·신규 발급·참조 remap 요청
- 상속 체인의 모든 `PrefabSave=true` PropertyGuid 전체 snapshot 저장·적용
- serialize/deserialize와 atomic save

Editor:

- Prefab Browser와 drag-and-drop
- 코드 구조 Hierarchy 표시
- property 값 조정 UI
- 구조 변경 명령 비활성화
- Engine PrefabSystem에 typed request 전달

Editor가 Prefab JSON이나 Registry map을 직접 수정하지 않는다.

## Scene Data and Object References

### Scene 저장

Scene은 배치된 Hierarchy 전체를 ObjectGuid key-value map으로 저장하며, 각 GameObject와 Component 상속 체인의 모든 `PrefabSave=true` 현재 값을 보존한다.

```json
{
  "schemaVersion": 1,
  "roots": ["<root-object-guid>"],
  "objects": {
    "<root-object-guid>": {
      "typeGuid": "<root-type-guid>",
      "structureVersion": 1,
      "properties": {},
      "components": {
        "<component-type-guid>": {
          "properties": {}
        }
      },
      "children": ["<body-object-guid>"]
    },
    "<body-object-guid>": {
      "typeGuid": "<body-type-guid>",
      "properties": {},
      "components": {},
      "children": []
    }
  }
}
```

- RuntimeTypeId와 InstanceGuid는 저장하지 않는다.
- 구조는 Client code와 StructureVersion이 정의한다.
- Scene은 각 Hierarchy GameObject의 ObjectGuid와 상속 체인의 모든 `PrefabSave=true` 현재 property 값을 저장한다.
- Parent-child는 parent의 순서가 보존된 Child ObjectGuid 목록으로 복원한다.
- Component 값은 `Owner ObjectGuid + Component TypeGuid`로 저장·복원한다.
- Camera Target·Script reference 등 계층 밖 참조는 target ObjectGuid를 저장한다.

### Client 로드

1. Engine/Client reflection module 등록과 Registry finalize
2. RootTypeGuid factory 실행
3. Client 코드가 전체 Component/Child 구조 생성
4. ObjectManager가 결정적 child 등록 순서와 StructureVersion을 검증해 Scene ObjectGuid 복원
5. ObjectManager가 모든 GameObject/Component에 새 InstanceGuid 발급
6. 상속 체인의 모든 `PrefabSave=true` PropertyGuid 기반 Scene 전체 값 적용
7. ObjectGuid resolve map으로 Camera Target·Script reference 연결
8. Client Begin/gameplay lifecycle 시작

Editor와 Client는 ObjectGuid와 설정값을 공유한다. RuntimeTypeId와 InstanceGuid는 각 실행에서 새로 구성된다.

## ObjectManager와 Object Registry

ObjectManager는 Object에 귀속되는 모든 ID의 단일 변경 권한자다. ReflectionRegistry가 TypeGuid/RuntimeTypeId catalog의 원본을 제공하면 ObjectManager가 생성되는 Object에 이를 바인딩하고 live index를 관리한다.

```text
ObjectManager
InstanceGuid → Object             모든 live Object
ObjectGuid   → GameObject         Hierarchy 노드·저장 참조
RuntimeTypeId → Object list       실행 중 클래스 조회
TypeGuid      → Object list       영구 클래스 기준 조회
Layer         → GameObject list
```

Component는 InstanceGuid로 live registry에 등록하고, 저장 데이터에서는 owner GameObject의 ObjectGuid 아래에서 Component TypeGuid로 찾는다.

Engine API:

```text
Create(TypeGuid, ObjectCreateContext)
Destroy(InstanceGuid)
ResolveInstance(InstanceGuid)
ResolveObject(ObjectGuid)
FindByRuntimeType(RuntimeTypeId)
```

규칙:

- factory는 Object를 만들지만 ID를 직접 발급하거나 수정하지 않는다.
- ObjectManager는 ReflectionRegistry를 조회해 RuntimeTypeId와 TypeGuid를 검증·바인딩한다.
- 신규 GameObject의 ObjectGuid, 모든 신규 Object의 InstanceGuid는 ObjectManager만 발급한다.
- Prefab/Scene 로드의 ObjectGuid 복원도 ObjectManager만 수행하고 중복이면 즉시 실패한다.
- ID와 모든 index는 World에 노출되기 전에 한 번의 등록 절차로 확정한다.
- World 삽입 후 임의 ID setter와 우회 reindex 경로를 제거한다.
- 모든 Child/Component도 ObjectManager에 정식 등록한다.
- 삭제 시 ObjectManager가 InstanceGuid/ObjectGuid/Type/Layer index를 한 번에 해제한 뒤 객체 수명을 종료한다.
- Editor 선택은 현재 InstanceGuid, 저장 추적과 외부 참조는 ObjectGuid를 사용한다.
- Transient는 InstanceGuid로 관리하지만 Scene에 저장하지 않는다.
- PropertyGuid와 PrefabGuid는 Object ID가 아니므로 ObjectManager index에 넣지 않는다.

### RuntimeHandle을 도입하지 않음

- 클래스 분류는 RuntimeTypeId가 담당한다.
- live 객체 식별은 InstanceGuid가 담당한다.
- 저장 객체 추적은 ObjectGuid가 담당한다.
- Update/Render는 이미 얻은 Shared/Weak pointer와 Layer 목록을 순회한다.
- RuntimeHandle/TypeIndex는 초기 EngineCore에 넣지 않는다.
- 실제 프로파일에서 필요성이 증명될 때만 최적화 단계로 추가한다.

## Editor Integration

초기 개편에서 현재 Editor의 Client 직접 링크와 ClientApp 실행 구조를 유지한다.

```text
Editor.exe
  EngineCore
  Client code/library
  ClientApp lifecycle
  EditorContext
  Hierarchy/Inspector/Prefab Browser
```

- 실제 Client factory/container 코드를 실행해 동일 구조를 본다.
- Client Play와 게임 상태 디버깅을 유지한다.
- Reflection 등록만 읽는 제한된 도구로 축소하지 않는다.
- 별도 Client module interface는 지금 만들지 않는다.

Editor 서비스:

- `SelectionService`: InstanceGuid/ObjectGuid 선택
- `HierarchyService`: 코드 구조 표시와 구조 lock
- `ReflectionInspector`: PropertyGuid 기반 값 편집
- `ObjectPlacementService`: TypeGuid/PrefabGuid 생성
- `PrefabAuthoringService`: Engine PrefabSystem request 전달
- `CommandStack`: property edit, root placement/delete, scene hierarchy edit
- `SceneDocument`: ObjectGuid와 현재 값 저장

## Legacy Removal

제거 대상:

- 32비트 `ID_DESC::m_typeID/m_objectID/m_instanceID`
- GUID → uint32 `Hash_GUID`
- `Create_FixedObjectID`, `Set_ObjectID`
- ObjectID/InstanceID 기반 Manager API
- Level별 PrototypeManager/PrototypeTag
- 문자열 `Create/CreatePrototype` invoke
- 분리된 Engine/Client/Editor reflection 생성 경로
- `client_metadata.json`
- 숫자 parentObjectID/TargetID 구 serializer
- 기존 LevelData/SceneData와 구 reader
- RuntimeHandle/TypeIndex/PrefabLocalGuid 계획
- Unity식 연결 Prefab 기능
- Prefab의 Child/Component 구조 추가·삭제 기능

Cutover:

1. Legacy ID/Prototype/serializer inventory
2. Object RuntimeTypeId/TypeGuid/ObjectGuid/InstanceGuid
3. module manifest와 Reflection generator
4. ObjectRegistry와 code structure 등록
5. full-snapshot PrefabSystem
6. 새 Scene writer/reader
7. Editor/Client call site 전환
8. 새 Prefab/Scene 재제작
9. Launcher→Client 실행 검증
10. 사용자 승인 후 Legacy 코드와 데이터 제거

변환기와 dual runtime reader는 만들지 않는다. 계획 단계에서는 기존 데이터를 삭제하지 않는다.

## Implementation Phases

### Phase 0 — 기준선과 구조 Inventory

- 기존 ID/PrototypeTag/serializer call site
- GameObject별 factory/component/container child 구조
- 폐기할 Scene/LevelData
- Editor/Client 실행·빌드 기준선

완료: 각 factory/container가 어떤 TypeGuid의 GameObject·Component를 만들고 어떤 순서의 Child ObjectGuid 구조를 구성하는지 inventory가 있다.

### Phase 1 — GuidCore와 Object Identity

- Guid128 strong type
- Object의 RuntimeTypeId/TypeGuid/ObjectGuid/InstanceGuid
- GUID 생성·parse·JSON·hash 테스트
- Transient 정책

완료: 실제 Object는 생성마다 새 InstanceGuid를 받고 저장 ObjectGuid는 재로드된다.

### Phase 2 — ReflectionCore

- 모듈별 TypeGuid/PropertyGuid manifest
- EngineCore generator와 generated `.rttr.cpp`
- generated `.rttr.cpp`의 base/derived 등록과 typed `PrefabSave` metadata
- ReflectionRegistry/TypeCatalog/PropertyDescriptor
- RTTR RuntimeTypeId ↔ TypeGuid mapping
- base → derived `EffectivePrefabProperties` 합성
- StructureVersion metadata

완료: Editor/Client가 같은 Type/Property catalog를 사용한다.

### Phase 3 — Code Structure Registry

- factory/container가 Component와 순서가 고정된 Child GameObject 구조 등록
- ObjectManager가 모든 Object ID를 검증·바인딩하고 index 관리
- ObjectManager가 모든 Hierarchy GameObject에 유일한 ObjectGuid 배정·복원
- 동일 Component TypeGuid 중복 거부
- 모든 Child는 ObjectGuid, 모든 Component는 InstanceGuid로 ObjectRegistry 등록
- 같은 TypeGuid의 Child가 여러 개여도 ObjectGuid key로 독립 추적
- Hierarchy 구조 lock metadata

완료: 캐릭터와 UI의 코드 구성 전체가 Hierarchy에 보이고 값 편집이 가능하다.

### Phase 4 — Full-Snapshot PrefabCore

- PrefabSnapshot/PrefabRegistry
- Root/Component/Child 상속 체인의 모든 `PrefabSave=true` property snapshot
- factory 생성 후 ObjectGuid 복원과 전체 값 적용
- Prefab 저장마다 변경 여부와 무관하게 전체 값 재기록
- Prefab 편집의 add/remove 차단
- drag-and-drop 배치

완료: Prefab은 구조를 만들지 않으며 코드 구조의 전체 저장 대상 값을 완전하게 보존·적용한다.

### Phase 5 — Scene·Reference

- Hierarchy ObjectGuid map과 전체 현재 값 저장
- Client load 시 code structure 재생성
- 새 InstanceGuid 발급
- ObjectGuid 기반 Camera Target/Script reference resolve
- 새 Scene/Prefab 데이터 제작

완료: Editor에서 추적·수정한 캐릭터/UI 구성이 Client에서 동일하게 재현된다.

### Phase 6 — Editor 전환

- Hierarchy/Inspector/ObjectPlacement/CommandStack 서비스화
- Prefab Browser와 value editor
- Child/Component add/delete UI 잠금
- 현재 ClientApp Play/Debug 회귀 유지

완료: Editor는 전체 Client 구조를 실행·표시하면서 허용된 property 값만 수정한다.

### Phase 7 — Legacy 제거·최적화

- 32비트 ID, PrototypeManager, HeaderParser, 구 serializer 제거
- 승인된 기존 데이터 제거
- ObjectGuid/InstanceGuid Registry와 Prefab apply 성능 측정
- 실제 병목이 있을 때만 추가 handle 검토

완료: Legacy 심볼이 없고 RuntimeHandle 없이 목표 성능을 만족한다.

## First Implementation Backlog

1. Guid128와 Object identity 타입
2. ObjectManager 단일 ID 발급·복원·등록·해제 proof-of-concept
3. Engine/Client reflection manifest schema
4. 한 base/derived TypeGuid와 `PrefabSave` PropertyGuid generated `.rttr.cpp`
5. 테스트 GameObject factory의 Component와 순서가 고정된 Child 구조 inventory
6. 코드 구조를 Hierarchy에 표시
7. 구조 add/delete 잠금과 property 편집
8. 단일 Root Prefab 전체 property snapshot
9. `Owner ObjectGuid + Component TypeGuid` 기반 Component 전체 값 snapshot
10. UI/Character Hierarchy ObjectGuid map과 전체 값 snapshot
11. ObjectGuid target reference 저장·resolve
12. 신규 Scene 저장 후 Launcher→Client 실행

## Confirmed Decisions

2026-08-26 사용자 확정:

1. Object identity 필드는 Object 단계에서 설계한다.
2. 기존 ObjectID의 논리 객체 추적 역할을 128비트 ObjectGuid로 명확히 한다.
3. RTTR type_id는 RuntimeTypeId이며 TypeGuid에 매핑하고 저장하지 않는다.
4. InstanceGuid는 모든 실제 GameObject/Component 생성 때마다 새로 발급한다.
5. PropertyGuid는 ReflectionRegistry, PrefabGuid는 PrefabRegistry가 관리한다.
6. Reflection manifest는 모듈별로 분리하고 EngineCore가 생성·검증한다.
7. GameObject/Child/Component 구조는 Client factory/container 코드가 미리 구성한다.
8. Prefab에서는 Child와 Component를 추가·삭제할 수 없다.
9. Prefab의 목적은 코드 구조의 reflection property 값 조정이다.
10. 모든 Child/Component는 Hierarchy와 Inspector에서 보고 수정할 수 있어야 한다.
11. 같은 GameObject에 같은 Component TypeGuid를 중복 부착하지 않는다.
12. Prefab 배치 후 원본과 연결하지 않고 배치 객체의 값을 별도로 저장한다.
13. 계층 밖 캐릭터·Camera Target·Script reference를 ObjectGuid로 추적할 수 있어야 한다.
14. Editor는 Client 코드와 ClientApp 전체를 현재처럼 실행한다.
15. RuntimeHandle, TypeIndex, PrefabLocalGuid와 Unity식 연결 기능은 도입하지 않는다.
16. 기존 데이터는 변환하지 않고 새 구조로 재제작한다.
17. ObjectGuid는 Hierarchy에 표시되는 GameObject 노드의 key다.
18. Container와 일반 Child는 같은 GameObject/ObjectGuid 규칙을 사용한다.
19. 같은 TypeGuid의 Child가 여러 개여도 각 ObjectGuid가 달라 독립적으로 추적한다.
20. Component의 영속 위치는 `Owner ObjectGuid + Component TypeGuid`로 식별한다.
21. Prefab은 변경분만 저장하지 않고 저장할 때마다 상속 체인의 모든 `PrefabSave=true` property 현재 값을 전체 snapshot으로 기록한다.
22. Prefab/Scene 저장 대상은 상속 체인의 `PrefabSave=true` property 전체로 한정한다.
23. generated `.rttr.cpp`가 base/derived 관계와 typed `PrefabSave` metadata를 등록하고 EngineCore가 base → derived effective property 목록을 합성한다.
24. ObjectManager는 RuntimeTypeId와 TypeGuid를 ReflectionRegistry에서 받아 Object에 바인딩하고 모든 Object ID index를 관리하며, ObjectGuid/InstanceGuid의 발급·복원은 직접 수행한다.
25. Object는 ID를 보관하고 읽기만 허용하며 임의 setter를 제공하지 않는다.
26. PropertyGuid와 PrefabGuid는 Object ID가 아니므로 각각 ReflectionRegistry와 PrefabRegistry가 관리한다.

## Links

- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/RTTR_리플렉션_구조와_한계_분석]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/sources/RTTR_라이브러리_개편_참고]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/dev-tasks/SoExDobin_git_history_summary]]
