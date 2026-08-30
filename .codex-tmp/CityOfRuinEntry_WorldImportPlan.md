---
type: design
date: 2026-08-30
status: draft
owner: user
repo_path: C:\Users\a9018\Desktop\Direct11_Nier_Automata
aliases:
  - CityOfRuinEntry World Import Plan
  - g11021 Engine Import Plan
---

# CityOfRuinEntry 월드 분해·엔진 임포트 계획

## Summary

`CityOfRuinEntry.fbx`를 그대로 ModelConverter에 넣지 않는다. 현재 FBX는 g11021 WMB 시각 메시와 COL 메시를 한 파일에 평탄화했지만 LAY 배치는 잃어버렸고, 엔진의 `.model` v2에는 LOD·collision·instance·mesh bounds 분류가 없기 때문이다.

입력은 다음 세 원본으로 다시 분리한다.

1. FBX/WMB 시각 메시: 515개
2. raw `g11021.col`: collision batch 401개와 collision metadata
3. raw `Layout.lay`: asset record 26개와 추가 instance 372개

첫 수직 슬라이스는 시각 메시와 BaseColor/Normal/Mask texture를 정상 표시하는 데 집중한다. 이후 같은 좌표계에서 COL 기반 Navigation, LAY 배치, 지역 LOD와 공간 chunk 최적화를 순서대로 추가한다. 새 전역 World/Asset service는 만들지 않고 기존 `ModelConverter`, `Model`, `Mesh`, `NavigationBuilder`, `CityOfRuinEntry`의 책임을 먼저 확장한다.

전체 916개 FBX 메시 이름의 행 단위 근거는 [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/sources/CityOfRuinEntry_FBX_메시_인벤토리]]에 둔다.

## Verified Input Facts

### FBX

- 경로: `Client/bin/resources/Models/World/CityOfRuinEntry/CityOfRuinEntry.fbx`
- 크기: 124,656,236 bytes
- 전체: mesh 916, vertices 750,879, triangles 403,622
- bounds: min `(-311.2835, -523.9980, -91.2116)`, max `(373.9430, 501.7502, 176.9464)`
- hierarchy/parent/collection/Empty/collection instance: 없음
- transform: 대부분 identity이며 실제 배치 좌표가 vertex에 구워져 있음
- shared Mesh datablock: 없음

### Name Groups

| Base name | Count | Source/role |
|---|---:|---|
| `HexGround` | 401 | raw COL과 정확히 대응하는 collision batch |
| `g11021_highway` | 150 | WMB highway visual |
| `g11021_build3` | 77 | WMB building visual |
| `ground` | 73 | material/UV를 가진 WMB ground visual; collision이 아님 |
| `g11021_build4` | 53 | WMB building visual |
| `g11021_build7` | 47 | WMB building visual |
| `nolowmap_debris` | 41 | WMB debris visual |
| `g11021_build1` | 20 | WMB building visual |
| `g11021_build2` | 19 | WMB building visual |
| `g11021_DistantView` | 16 | WMB distant backdrop |
| `g11021_build6` | 9 | WMB building visual |
| `g11021_build5` | 4 | WMB building visual |
| `MtRobot9` | 3 | WMB large prop visual |
| `g11021_build1-LOD1` | 2 | 명시적 internal LOD1 |
| `g11021_build6-LOD1` | 1 | 명시적 internal LOD1 |

`HexGround`를 제외하면 WMB 시각 메시가 515개다. 명시적 `-LOD1` 3개를 제외한 512개는 g11021 wd1의 문서상 object count와 일치한다. Blender `.001` suffix는 이름 충돌을 피하는 suffix이며 LAY instance ID가 아니다.

### COL

- raw `g11021.col`: name group `HexGround` 1개, mesh/batch 401개, vertices 13,934, triangles 15,011, COL tree node 411개
- FBX의 `HexGround`: vertices 13,934, triangles 14,977
- FBX 왕복에서 COL triangle 34개가 탈락했으므로 collision 산출물은 FBX가 아니라 raw COL을 사용한다.
- collision type: `127` 297, `255` 5, `3` 68, 미등록 `95` 4, 미등록 `16` 27
- modifier: none `0` 394, transparent wall `2` 7
- surface: rock 159, concrete 100, metal solid 55, rubble2 48, grass2 32, metal grate 4, gravel1 3

### LAY

- raw `Layout.lay`: model 9종, asset record 26개, 추가 instance 372개
- asset record 자체도 한 placement이므로 총 placement는 398개다.
- 모델별 총 placement: `bg008e` 2, `bg2060` 2, `bg2004` 2, `bg2043` 6, `bg2003` 1, `bgd00b` 6, `bgd004` 47, `bgd002` 12, `bgd000` 320
- FBX에는 `LAY`, `lay_layAssets`, `lay_layInstances`, `*-Instance` Empty가 없으므로 LAY transform은 포함되지 않았다.

### Materials and Textures

- FBX material name 50개와 `materials.json` record 50개가 정확히 일치한다.
- shader 분포: `PBS00_XXXXX` 46, `PBS10_XXXXX` 3, `Plt00_XXXXX` 1
- sidecar에는 texture binding 360개, unique hash 131개가 있다.
- local g11021 및 같은 data family WDA cache에서 128개 hash가 실제 DDS로 resolve된다.
- 미해결 `1FBC0984`, `4E9C16F4`, `7FD4929A`는 모든 material이 공유하는 Env/Irradiance, Light/Mask, DetailNormal 계열 placeholder이므로 엔진 default texture로 명시적으로 대체해야 한다.
- FBX embedded image 연결은 50개 중 material 1개에만 남아 있으므로 authoritative material 입력은 FBX가 아니라 `materials.json`이다.
- WMB 시각 메시 515개는 모두 UV 4개와 vertex color `Col`을 갖지만 현재 `VTXMESH`와 ModelConverter는 UV0 하나만 저장하고 vertex color를 버린다.

## Current Engine Fit and Gaps

### Reusable Existing Path

- `CityOfRuinEntry` GameObject가 이미 `Model`과 `Navigation`을 소유하고 모든 mesh를 렌더한다.
- model prototype tag는 `CityOfRuinEntry1`, navigation tag는 `CityOfRuinEntry`다.
- `ModelConverter`는 static FBX를 `.model` v2로 변환하고 mesh name과 material index를 보존한다.
- `Model::Extract_RawMeshData()`와 `NavigationBuilder`를 이용해 Model geometry에서 Recast navigation을 bake할 수 있다.

### Blocking Gaps

1. 전체 FBX를 변환하면 COL 401개까지 render mesh로 들어가 총 916 draw 대상이 된다.
2. `.model` v2는 mesh source class, bounds, LOD group, collision metadata, instance transform을 저장하지 않는다.
3. 현재 `CityOfRuinEntry::Render()`는 mesh별 draw이므로 시각 메시만 넣어도 최대 515 draw다.
4. 현재 static vertex는 UV0만 보존하고 vertex color와 UV1~3을 버린다.
5. FBX embedded material은 실제 texture mapping 입력으로 사용할 수 없다.
6. Engine collision은 AABB/OBB/Sphere 중심이며 raw COL 같은 정적 concave triangle mesh를 직접 수용하는 경로가 없다.
7. 기존 instance buffer는 particle 계열이며 범용 static Model instance 경로가 없다.
8. current FBX에는 LAY 398 placements가 없으므로 FBX만 변환해서 원본 월드를 복원할 수 없다.
9. 명시적 internal `-LOD1`은 3개 submesh뿐이다. 지역 전체 low LOD는 대응 wd2 리소스 `g20608`을 별도로 확보해야 한다.

## Target Asset Split

중간 산출물은 저장소가 아니라 `D:\NierRes\work\CityOfRuinEntry\` 아래에 둔다.

```text
CityOfRuinEntry/
  source-manifest.json
  mesh-inventory.csv
  visual/
    near.fbx
    internal-lod1.fbx
    distant.fbx
  collision/
    g11021.col
    collision.fbx
    collision-metadata.json
  layout/
    Layout.lay
    placements.json
  material/
    materials.json
    textures/
```

runtime 출력은 실제 reader가 생긴 자산만 `Client/bin/resources/Models/World/CityOfRuinEntry/`에 복사한다. 분석 중간 파일과 대량 원본 추출물은 저장소와 Vault에 넣지 않는다.

## Execution PLAN

### Phase W0 — Fixture Freeze and Deterministic Audit

1. FBX, `materials.json`, raw WMB/COL/LAY, local/WDA texture directory의 SHA-256을 manifest에 기록한다.
2. 916개 mesh inventory를 매번 다시 생성할 수 있는 Blender batch script를 고정한다.
3. axis, unit, origin, handedness를 한 번만 정의하고 모든 split output에 같은 변환을 적용한다.
4. export 과정에서 recenter, merge-by-name, apply transform을 임의로 사용하지 않는다.

완료 조건:

- 재실행 시 916행 이름, 15개 base group, bounds와 총 vertex/triangle 수가 동일하다.
- COL 401 batch와 LAY 398 placement가 별도 입력으로 보존된다.

### Phase W1 — Semantic Split

1. FBX에서 `HexGround` 401개를 제거해 visual 원본을 만든다.
2. `g11021_DistantView` 16개를 distant asset으로 분리한다.
3. `g11021_build1-LOD1` 2개와 `g11021_build6-LOD1` 1개를 internal LOD1 asset으로 분리한다.
4. 나머지 WMB 512개를 near visual로 유지한다.
5. collision은 FBX의 `HexGround`가 아니라 raw COL parser 출력으로 다시 생성하고 type/modifier/surface metadata sidecar를 함께 만든다.
6. LAY는 26 asset transform과 372 instance transform을 보존한 placement JSON으로 변환한다.

완료 조건:

- visual near 512, internal LOD1 3, distant 16, collision 401이라는 분할 count가 맞는다.
- 원본과 split asset을 겹쳐서 bounds와 위치가 일치한다.

### Phase W2 — Visual Vertical Slice

1. `near.fbx`만 현재 ModelConverter로 변환해 static `.model` loading 한계를 먼저 확인한다.
2. ModelConverter가 FBX embedded texture 대신 같은 폴더의 NieR `materials.json`을 선택적으로 읽도록 기존 `Converter::ReadMaterialData()`를 확장한다.
3. material name으로 50개 record를 연결하고 hash DDS 경로를 semantic slot에 매핑한다.
4. 128개 실제 texture만 runtime resource로 복사하고 3개 공통 missing hash는 Engine default SRV로 대체한다.
5. `CityOfRuinEntry1`과 `CityOfRuinEntry` tag 불일치를 하나의 명시적 prototype 계약으로 정리한 뒤 ModelSettings workbook에 등록한다.
6. 첫 gate에서는 UV0 기반 BaseColor/Normal/Mask 표시까지만 통과시키고 UV1~3/vertex color 확장은 별도 gate로 둔다.

완료 조건:

- Editor에서 near visual 512개만 보이고 `HexGround`가 렌더되지 않는다.
- material 50개가 이름 기준으로 매칭되고 이전 draw의 texture 잔존 없이 모든 slot이 바인딩된다.
- 모델 bounds와 축이 Blender 기준과 일치한다.

### Phase W3 — Spatial Chunk and Draw Cost

1. 정확성 gate 후 near visual을 공간 cell과 material 조합으로 offline batch한다.
2. cell 크기는 64m/128m 후보를 실측하고 frustum culling 효율, draw count, vertex duplication을 비교해 선택한다.
3. 서로 다른 material, transparent/cutout pass, distant, internal LOD는 합치지 않는다.
4. `Model` 또는 `CityOfRuinEntry`가 기존 mesh bounds를 이용해 chunk 단위 frustum culling하도록 최소 확장한다.
5. 새 WorldRenderer나 AssetDatabase를 만들지 않는다.

완료 조건:

- 515개 무조건 draw 경로가 제거된다.
- 카메라 대표 위치별 visible chunk와 draw count가 기록된다.
- batching 전후 시각 차이와 bounds 누락이 없다.

### Phase W4 — COL Navigation and Physical Collision

1. raw COL geometry를 surface/type별로 분리하되 world alignment를 유지한다.
2. transparent wall modifier `2`와 미등록 collision type `16/95`는 의미를 추측해 삭제하지 않고 별도 검증 그룹으로 둔다.
3. walkable surface와 slope 기준을 정해 `NavigationBuilder` 입력용 collision model을 만든다.
4. 기존 `NavigationBuilder::Export_Binary()`로 `CityOfRuinEntry` navigation을 bake하고 현재 Navigation tag에 연결한다.
5. 플레이어 바닥 이동은 Navigation으로 먼저 검증한다.
6. 벽·천장·낙하 방지까지 실제 triangle collision이 필요하다는 런타임 증거가 생기면 그때 기존 `Collider`/`CollisionManager` 아래 static `MeshCollider`와 BVH를 검토한다. 401개의 primitive collider로 근사하지 않는다.

완료 조건:

- Blender COL과 Debug Nav가 같은 위치에 겹친다.
- 다리 진입, 경사, 가장자리, transparent wall 대표 지점에서 이동 결과를 기록한다.
- 미등록 type의 처리 결정을 샘플 충돌 테스트 뒤 확정한다.

### Phase W5 — LAY Placement Restore

1. LAY model key 9종의 실제 DTT/FBX와 material/texture를 각각 Model prototype으로 준비한다.
2. 자동차·구조물 계열 `bg008e/bg2060/bg2004/bg2043/bg2003` 13 placements는 일반 정적 object 경로로 먼저 복원한다.
3. vegetation `bgd00b/bgd004/bgd002/bgd000` 385 placements는 model key별 matrix batch로 묶는다.
4. 범용 manager를 추가하지 않고 `CityOfRuinEntry`가 layout batch를 소유하며, 기존 `Mesh/Model`에 필요한 immutable instance buffer와 `Render_Instanced`만 최소 확장하는 방안을 우선한다.
5. asset record transform과 추가 instance transform을 모두 포함하고 `name[:6]` model key 규칙을 검증한다.

완료 조건:

- 총 placement 398개와 model별 count가 raw LAY와 일치한다.
- 임의 10개 placement의 position/rotation/scale이 Blender LAY import와 일치한다.
- vegetation이 개별 385 draw가 아니라 model별 instance draw로 제출된다.

### Phase W6 — LOD and Distant View

1. internal `-LOD1` 3개는 대응 `build1/build6` submesh에만 연결한다. 지역 전체 LOD로 오인하지 않는다.
2. g11021 wd1 high와 대응하는 g20608 wd2 low를 같은 절차로 추출·분석하고 bounds/origin을 비교한다.
3. 지역 전체 high/low 전환은 tile center가 아니라 spatial chunk bounds와 camera distance를 사용한다.
4. `DistantView`는 near geometry와 별도 visibility range로 관리한다.
5. threshold와 hysteresis는 대표 카메라 이동에서 pop/flicker를 측정한 뒤 확정한다.

완료 조건:

- near/internal LOD/distant/wd2 low가 동시에 겹쳐 그려지지 않는다.
- 왕복 이동에서 LOD thrashing이 없다.
- 전환 전후 실루엣과 material pass가 유지된다.

### Phase W7 — Vertex and Material Fidelity

1. PBS10 terrain material 3개가 실제로 요구하는 UV와 vertex color channel을 우선 식별한다.
2. UV1~3 또는 `Col`이 실제 shader output에 필요하다는 material별 증거가 확인되면 static world 전용 vertex layout을 기존 `VTXMESH`와 분리하거나 versioned 확장한다.
3. 모든 캐릭터/기존 static model의 vertex layout을 한 번에 바꾸지 않는다.
4. `materials.json` shader/technique/variables는 import hint로 보존하되 현재 Engine shader slot에 검증된 의미만 매핑한다.

완료 조건:

- PBS00/PBS10/Plt00 대표 material 각각에서 BaseColor, Normal, Mask와 alpha/cutout 동작이 검증된다.
- 사용하지 않는 UV/color channel은 추측으로 runtime format에 추가하지 않는다.

## Verification Matrix

| Gate | Verification |
|---|---|
| Inventory | FBX 916 rows, base groups 15, COL 401, WMB 515 |
| Split | near 512, internal LOD1 3, distant 16, collision 401 |
| Material | 50/50 name match, 360 bindings, 128 DDS resolve, 3 defaults |
| Visual | bounds/axis match, COL invisible, no stale texture binding |
| Navigation | raw COL alignment, representative traversal and edges |
| LAY | 9 model keys, 26 assets, 372 instances, total 398 placements |
| Draw cost | visible chunks, draw count, frame time before/after |
| LOD | g11021/g20608 alignment, hysteresis, no double render |
| Runtime | Editor and Client actual launch, first error fix and rerun |

구현 시 빌드는 solution-level `Debug|x64` Engine, Client, Editor 순서로 실행하고, 실제 Editor/Client 실행 전에는 완료로 처리하지 않는다.

## Decisions and Risks

- 현재 FBX는 visual geometry fixture로는 사용할 수 있지만 collision과 LAY의 authoritative source가 아니다.
- `HexGround`를 visual `ground`와 합치지 않는다. 이름이 아니라 raw COL 대조와 UV/material 유무가 근거다.
- `.001` suffix를 instance로 해석하지 않는다.
- internal `-LOD1` 3개를 전체 map LOD로 사용하지 않는다.
- g20608 wd2 low 확보 전에는 지역 전체 LOD가 완료된 것이 아니다.
- `materials.json`과 hash texture cache 없이 FBX embedded material만으로 변환하지 않는다.
- raw COL의 미등록 type `16/95`를 삭제하거나 임의로 Block All로 치환하지 않는다.
- 401 primitive collider, 398 개별 vegetation draw, 916 mesh 단일 모델 렌더를 최종 구조로 채택하지 않는다.

## Links

- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/sources/CityOfRuinEntry_FBX_메시_인벤토리]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/모델_애니메이션_리소스_파이프라인_계획]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/머티리얼_셰이더_그래프_잠정_방향]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/rules/기존_구조_우선_설계_규칙]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata]]
