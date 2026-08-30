# CityOfRuin BLEND 기반 월드 임포트 PLAN v2

## 결론

`CityOfRuinEntry.fbx`를 단일 모델로 변환하는 방식은 폐기한다. FBX는 WMB와 COL을 한 레벨로 평탄화하고 LAY를 제거하며, `.blend` 대비 triangle 54개도 손실한다. 기준 입력은 `.codex-tmp`의 `.blend` 구조와 원본 WMB/COL/LAY이며, FBX는 WMB 시각 geometry를 엔진 `.model`로 넘기는 중간 산출물로만 쓴다.

엔진 입력은 다음 세 소유권으로 분리한다.

- WMB: 렌더 geometry, LOD/SHADOW pass, material, 원본 공간 트리
- COL: navigation/physical collision geometry, type/surface/modifier, 원본 공간 트리
- LAY: 외부 모델 key와 placement transform

## 확인된 BLEND 구조

| 파일 | WMB | WMB pass | COL | LAY asset + instance | WMB/COL tree |
|---|---:|---|---:|---:|---:|
| Entry `g11021` | 515 | LOD0 512, LOD1 3 | 401 | 26 + 372 = 398 | 63 / 411 |
| Bridge `g11120` | 508 | LOD0 367, LOD1 81, LOD2 24, SHADOW 36 | 140 | 24 + 451 = 475 | 63 / 135 |
| MainField `g11220` | 446 | LOD0 306, LOD1 69, LOD2 37, SHADOW 34 | 335 | 54 + 2051 = 2105 | 63 / 253 |

세 파일 공통:

- root Collection은 `WMB`, `COL`, `LAY`다.
- WMB mesh는 `mesh_group_props.lod_name/lod_level`, `colTreeNodeIndex`, `unknownWorldDataIndex`, `boneSetIndex`를 가진다.
- 모든 WMB mesh는 material 1개, `UVMap1~4`, vertex color `Col`을 가진다.
- 모든 COL mesh는 material/UV/color가 없고 `col_mesh_props`에 `col_type`, `surface_type`, `modifier`, `unk_byte`를 가진다.
- WMB/COL tree Empty는 이름 `node_left_right_wmb|col`, CUBE center/half-extent transform, leaf `meshIndices`를 가진다.
- 세 WMB tree는 모두 63 node/32 leaf이고, WMB mesh coverage 및 `colTreeNodeIndex`가 누락·중복·불일치 없이 맞는다.
- COL tree도 각 파일의 COL mesh index를 누락·중복 없이 모두 덮는다.
- Blender object modifier, Geometry Nodes modifier, constraint, parent, Collection Instance, shared mesh datablock은 모두 0개다.
- 존재하는 node group 5개는 material용 `ShaderNodeTree`이며 Geometry Nodes가 아니다.
- LAY의 `*-Instance`는 실제 Blender Collection Instance가 아니라 transform marker Empty다. `instance_type=NONE`이므로 이름/Collection/transform을 읽어야 한다.

## 반드시 정정할 이전 해석

1. Entry FBX 916 mesh는 렌더 916개가 아니다. WMB 515 + COL 401이다.
2. Entry split은 `near 512 + distant 16 + LOD1 3`이 아니다. LOD0 512 안에 `g11021_DistantView` 16개가 포함된다. 분리 시 `일반 LOD0 496 + DistantView 16 + LOD1 3 = 515`다.
3. `SHADOW`는 LOD-1이 아니다. `lod_level=-1`, `lod_name=SHADOW`로 저장된 별도 shadow pass다.
4. 이름의 `.001`은 instance가 아니다. 동일 semantic group/pass의 material 또는 spatial batch가 Blender에서 중복 이름 suffix를 받은 것이다.
5. `col_mesh_props.modifier`는 Blender modifier가 아니다. NieR collision behavior field다. Entry에는 `modifier=2`가 7개 있고 Bridge/MainField는 전부 0이다.
6. 임의 64m/128m 재청킹을 먼저 할 이유가 없다. 원본 WMB spatial tree를 우선 사용한다.
7. `g20608`은 data004 wd2에 존재하지만 Entry 전체 low LOD 대응물인지는 아직 bounds/origin/mesh-name 대조 전이므로 후보로만 둔다.

## Entry 메시 그룹

Entry는 12개 semantic group이다.

| Group | Pass object 수 | Triangle 수 | 처리 |
|---|---:|---:|---|
| `g11021_build1` | LOD0 20 / LOD1 2 | 20,811 / 76 | group 단위 LOD |
| `g11021_build2` | LOD0 19 | 15,403 | LOD0 고정 |
| `g11021_build3` | LOD0 77 | 49,714 | LOD0 고정 |
| `g11021_build4` | LOD0 53 | 42,806 | LOD0 고정 |
| `g11021_build5` | LOD0 4 | 6,320 | LOD0 고정 |
| `g11021_build6` | LOD0 9 / LOD1 1 | 11,624 / 10 | group 단위 LOD |
| `g11021_build7` | LOD0 47 | 19,915 | LOD0 고정 |
| `g11021_DistantView` | LOD0 16 | 8,776 | 별도 distance range |
| `g11021_highway` | LOD0 150 | 150,056 | LOD0 고정 |
| `ground` | LOD0 73 | 6,333 | LOD0 고정 |
| `MtRobot9` | LOD0 3 | 173 | LOD0 고정 |
| `nolowmap_debris` | LOD0 41 | 56,648 | LOD0 고정 |

Entry LOD1 3개는 86 triangles뿐이므로 지역 전체 LOD로 사용하면 안 된다.

## 산출물 계약

대량 원본/중간 산출물은 저장소와 Vault 밖 `D:\NierRes\work\CityOfRuin\`에 둔다.

```text
CityOfRuin/
  Entry/
    source-manifest.json
    visual.fbx
    visual.world.json
    collision.bin
    navigation.bin
    layout.json
  Bridge/
  MainField/
```

`visual.world.json`은 FBX가 잃는 데이터만 보존한다.

- stable source mesh index와 object name
- semantic group
- `lod_name`, `lod_level`
- material name
- `colTreeNodeIndex`, `unknownWorldDataIndex`
- WMB tree node index/children/center/half extent/leaf mesh indices
- model converter 출력 mesh index와의 1:1 mapping

`collision.bin`은 COL geometry와 `col_type/surface_type/modifier/unk_byte`, COL tree를 보존한다. `layout.json`은 asset record 자체의 첫 placement까지 포함한다.

## 실행 Phase

### Phase 0 — 재현 가능한 BLEND audit/exporter 고정

1. 세 `.blend`의 Collection과 custom property를 읽는 Blender batch exporter를 고정한다.
2. 이름은 표시에만 쓰고 WMB/COL/LAY 판정은 Collection과 property로 한다.
3. WMB/COL tree 무결성, LOD name/property 일치, LAY orphan 여부를 export 전 강제 검증한다.
4. source hash, Blender version, object/vertex/triangle count를 `source-manifest.json`에 기록한다.

완료 조건:

- Entry 515/401/398, Bridge 508/140/475, MainField 446/335/2105가 재실행마다 같다.
- 세 WMB/COL tree에서 missing/out-of-range/duplicate/branch error가 모두 0이다.

### Phase 1 — WMB-only visual vertical slice

1. `WMB/<gXXXXX>` Collection의 MESH만 FBX로 내보낸다. COL 및 모든 Empty를 제외한다.
2. mesh export 순서와 `.model` mesh index를 sidecar에 명시적으로 연결한다.
3. Entry에서는 우선 LOD0 color pass만 렌더하고 LOD1/DistantView는 토글 가능한 검증 대상으로 둔다.
4. 현재 `CityOfRuinEntry`와 `Model` 경로를 유지하고, 별도 global world manager는 추가하지 않는다.

완료 조건:

- Entry render 대상이 COL 없는 WMB 515개이며 color 기본 pass에는 LOD0 512개만 후보가 된다.
- `.blend` WMB 388,665 triangles와 export 결과 차이가 0이다.
- object name/material/source index mapping이 515/515다.

### Phase 2 — world vertex/material fidelity

1. 현재 `VTXMESH`가 버리는 UV2~4와 `Col`의 실제 shader 사용을 material별로 검증한다.
2. Entry에서 Vertex Color node를 쓰는 3 materials부터 렌더 차이를 비교한다.
3. 필요성이 확인된 channel만 versioned world vertex layout 또는 기존 model format 확장으로 보존한다.
4. material node graph 자체를 runtime에 복제하지 않고, 기존 material/shader graph 계획의 semantic slot으로 변환한다.

완료 조건:

- WMB 전 mesh의 UVMap1~4/Col audit가 유지된다.
- BaseColor/Normal/Mask 및 vertex-color 의존 재질에서 Blender와 엔진 비교가 통과한다.

### Phase 3 — 원본 WMB tree 기반 culling

1. 임의 grid 재청킹 대신 63-node WMB tree를 manifest에 직렬화한다.
2. 기존 `CityOfRuinEntry::Render()`의 전 mesh loop 앞에 node AABB frustum test를 넣는다.
3. leaf `meshIndices`와 model mesh index mapping으로 visible mesh만 draw한다.
4. 필요할 때만 동일 leaf/material 병합을 추가 최적화로 측정한다.

완료 조건:

- 원본 tree leaf coverage가 runtime mesh와 1:1이다.
- 대표 카메라에서 visible node/mesh/draw 수가 기록된다.
- culling on/off 영상 차이는 없고 draw 수만 감소한다.

### Phase 4 — LOD, DistantView, SHADOW 분리

1. `lod_level`과 `lod_name`을 authoritative pass로 사용한다.
2. semantic group별 union bounds와 거리로 LOD0/1/2 중 존재하는 pass만 선택한다.
3. LOD가 없는 group은 LOD0을 유지한다.
4. `DistantView`는 LOD metadata가 아니라 이름으로 확인되는 특수 group이므로 별도 visibility range로 둔다.
5. SHADOW는 color pass에서 제외하고 실제 shadow pass가 생길 때만 제출한다.
6. threshold/hysteresis는 blend에 없으므로 엔진 설정에서 실측 후 결정한다.

완료 조건:

- Entry에서 LOD1은 build1/build6에만 적용된다.
- Bridge/MainField의 LOD2와 SHADOW가 color LOD로 섞이지 않는다.
- 왕복 카메라 이동에서 double render와 thrashing이 없다.

### Phase 5 — COL navigation/collision

1. raw COL 또는 BLEND `COL` Collection에서 geometry와 metadata를 함께 export한다.
2. Entry의 401 mesh/13,934 vertices/15,011 triangles를 기준으로 삼고 FBX의 14,977 triangles는 사용하지 않는다.
3. `surface_type`으로 navigation 후보를 분류하되 `col_type`, `modifier`, `unk_byte`를 삭제하지 않는다.
4. Entry `modifier=2` 7개는 transparent-wall 동작을 별도 샘플로 검증한다.
5. 기존 `NavigationBuilder`로 nav를 먼저 bake하고, runtime triangle collision은 실제 요구가 확인된 뒤 기존 collision owner 아래 최소 확장한다.

완료 조건:

- raw/BLEND COL count와 metadata가 1:1이다.
- Blender COL, debug nav, visual WMB가 같은 좌표에 겹친다.

### Phase 6 — LAY placement 복원

1. `lay_layAssets`와 `lay_layInstances` Empty의 world transform을 모두 export한다.
2. asset record 자체를 첫 placement로 포함한다.
3. 이름의 `-Instance`는 asset key 연결 표시에만 쓰고 Blender instance 기능으로 해석하지 않는다.
4. model key별 prototype을 기존 prototype/object 경로로 준비한다.
5. 정확성 gate에서는 개별 placement로 비교하고, 통과 후 model/material별 immutable instance buffer로 묶는다.

완료 조건:

- Entry 398, Bridge 475, MainField 2105 placements와 model별 count가 일치한다.
- asset key orphan이 0이고 임의 표본 transform이 Blender와 일치한다.

### Phase 7 — 세 chunk 통합과 성능 gate

1. Entry에서 통과한 exporter/manifest/runtime 경로를 Bridge와 MainField에 그대로 적용한다.
2. chunk별 root bounds로 load/unload 및 visibility를 결정한다.
3. g20608 같은 wd2 후보는 별도 audit 후에만 region low LOD로 연결한다.
4. 실제 Editor/Client 실행에서 메모리, draw, visible triangle, frame time을 기록한다.

완료 조건:

- 세 chunk가 같은 포맷과 코드 경로로 로드된다.
- LOD/SHADOW/COL/LAY가 서로 다른 owner와 pass를 유지한다.
- Debug|x64 Engine/Client/Editor solution target과 실제 실행 검증이 통과한다.

## 구현 우선순위

1. Phase 0 exporter와 manifest
2. Phase 1 WMB-only visual
3. Phase 2 vertex/material fidelity
4. Phase 3 원본 tree culling
5. Phase 4 LOD/SHADOW/DistantView
6. Phase 5 COL navigation
7. Phase 6 LAY placement
8. Phase 7 세 chunk 통합과 전체 최적화

정확성 확인 전 batching/merge/새 manager 추가를 시작하지 않는다. 현재 코드의 `CityOfRuinEntry`, `Model`, `NavigationBuilder`, prototype 경로를 먼저 확장한다.
