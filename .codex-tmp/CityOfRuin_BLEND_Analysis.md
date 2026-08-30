# CityOfRuin `.blend` 구조 분석

- 근거: Blender 4.5.13에서 세 파일을 직접 열어 Collection, object, custom property, modifier, constraint, material node를 조사함.
- `LOD`는 이름이 아니라 `mesh_group_props.lod_level` / `lod_name`을 기준으로 집계함.
- CSV에는 세 파일의 모든 object 이름과 분류/속성을 기록함.

## CityOfRuinEntry.blend

- domain: `{'COL_MESH': 401, 'COL_TREE': 411, 'LAY_ASSET': 26, 'LAY_INSTANCE': 372, 'WMB_MESH': 515, 'WMB_TREE': 63}`
- LOD0: objects 512, semantic bases 12, vertices 736,839, triangles 388,579, materials 50
- LOD1: objects 3, semantic bases 2, vertices 106, triangles 86, materials 2

| Semantic mesh group | Object count by pass | Triangle count by pass |
|---|---:|---:|
| `g11021_build1` | LOD0=20, LOD1=2 | LOD0=20,811, LOD1=76 |
| `g11021_build2` | LOD0=19 | LOD0=15,403 |
| `g11021_build3` | LOD0=77 | LOD0=49,714 |
| `g11021_build4` | LOD0=53 | LOD0=42,806 |
| `g11021_build5` | LOD0=4 | LOD0=6,320 |
| `g11021_build6` | LOD0=9, LOD1=1 | LOD0=11,624, LOD1=10 |
| `g11021_build7` | LOD0=47 | LOD0=19,915 |
| `g11021_DistantView` | LOD0=16 | LOD0=8,776 |
| `g11021_highway` | LOD0=150 | LOD0=150,056 |
| `ground` | LOD0=73 | LOD0=6,333 |
| `MtRobot9` | LOD0=3 | LOD0=173 |
| `nolowmap_debris` | LOD0=41 | LOD0=56,648 |

- LOD level-set patterns: `{(0,): 10, (0, 1): 2}`
- LOD name/property mismatches: 0
- COL type: `{3: 297, 4: 5, 1: 68, 0: 31}`
- COL surface: `{3: 159, 1: 100, 13: 3, 10: 32, 15: 48, 4: 55, 6: 4}`
- COL modifier field: `{0: 394, 2: 7}`
- LAY model placements: `{'bg008e': 2, 'bg2060': 2, 'bg2004': 2, 'bg2043': 6, 'bgd000': 320, 'bgd004': 47, 'bgd002': 12, 'bgd00b': 6, 'bg2003': 1}`
- LAY orphan instance keys: `{}`
- WMB spatial tree: nodes 63, leaves 32, root [0], missing/out-of-range/duplicate/property-mismatch/branch-error 0/0/0/0/0
- COL spatial tree: nodes 411, leaves 206, root [0], missing/out-of-range/duplicate/branch-error 0/0/0/0
- non-identity transforms by domain: `{'COL_TREE': 411, 'WMB_TREE': 63, 'LAY_ASSET': 26, 'LAY_INSTANCE': 372}`
- Blender modifiers/constraints/parents/collection instances: 0/0/0/0
- WMB mesh UV/color signatures: `{(('UVMap1', 'UVMap2', 'UVMap3', 'UVMap4'), ('Col',)): 515}`
- materials using Vertex Color node: `['mas_concretefloor03_AAA3_tga', 'mas_concretewall01_aaax_tga', 'yo_asphalt_01_aaa4']`

## CityOfRuinBridge.blend

- domain: `{'COL_MESH': 140, 'COL_TREE': 135, 'LAY_ASSET': 24, 'LAY_INSTANCE': 451, 'WMB_MESH': 508, 'WMB_TREE': 63}`
- SHADOW: objects 36, semantic bases 8, vertices 3,727, triangles 1,704, materials 1
- LOD0: objects 367, semantic bases 14, vertices 386,311, triangles 250,363, materials 14
- LOD1: objects 81, semantic bases 13, vertices 225,554, triangles 97,766, materials 13
- LOD2: objects 24, semantic bases 4, vertices 7,410, triangles 2,704, materials 4

| Semantic mesh group | Object count by pass | Triangle count by pass |
|---|---:|---:|
| `_build_11120_05` | SHADOW=4, LOD0=28, LOD1=4 | SHADOW=20, LOD0=29,625, LOD1=12,174 |
| `build011120_0` | SHADOW=2, LOD0=22, LOD1=4, LOD2=4 | SHADOW=10, LOD0=18,149, LOD1=6,977, LOD2=883 |
| `build011120_1` | SHADOW=3, LOD0=43, LOD1=7 | SHADOW=10, LOD0=28,033, LOD1=10,542 |
| `build011120_2` | SHADOW=9, LOD0=50, LOD1=14 | SHADOW=28, LOD0=60,290, LOD1=31,870 |
| `build011120_3` | SHADOW=5, LOD0=30, LOD1=5, LOD2=5 | SHADOW=10, LOD0=30,355, LOD1=11,555, LOD2=628 |
| `build011120_4` | SHADOW=3, LOD0=44, LOD1=8, LOD2=8 | SHADOW=10, LOD0=32,446, LOD1=12,111, LOD2=519 |
| `build011120_5` | SHADOW=3, LOD0=25, LOD1=9 | SHADOW=10, LOD0=19,682, LOD1=2,064 |
| `GROUND_11120_00` | LOD0=22 | LOD0=9,285 |
| `GROUND_11120road_00` | LOD0=18, LOD1=6 | LOD0=568, LOD1=340 |
| `GROUND_11120road_01` | LOD0=10, LOD1=3 | LOD0=290, LOD1=174 |
| `GROUND_11120road_02` | LOD0=17, LOD1=4 | LOD0=1,305, LOD1=783 |
| `GROUND_11120road_03` | LOD0=10, LOD1=3 | LOD0=484, LOD1=290 |
| `GROUND_11120road_04` | LOD0=24, LOD1=7 | LOD0=7,302, LOD1=4,381 |
| `group358` | SHADOW=7 | SHADOW=1,606 |
| `highway11120` | LOD0=24, LOD1=7, LOD2=7 | LOD0=12,549, LOD1=4,505, LOD2=674 |

- LOD level-set patterns: `{(0,): 1, (0, 1, 2): 1, (-1, 0, 1, 2): 3, (-1, 0, 1): 4, (0, 1): 5, (-1,): 1}`
- LOD name/property mismatches: 0
- COL type: `{3: 121, 1: 19}`
- COL surface: `{3: 59, 1: 19, 9: 62}`
- COL modifier field: `{0: 140}`
- LAY model placements: `{'bgd005': 102, 'bgd000': 298, 'bgd004': 45, 'bgd024': 3, 'bgd025': 3, 'bgd026': 8, 'bgd00b': 15, 'bg2043': 1}`
- LAY orphan instance keys: `{}`
- WMB spatial tree: nodes 63, leaves 32, root [0], missing/out-of-range/duplicate/property-mismatch/branch-error 0/0/0/0/0
- COL spatial tree: nodes 135, leaves 68, root [0], missing/out-of-range/duplicate/branch-error 0/0/0/0
- non-identity transforms by domain: `{'COL_TREE': 135, 'WMB_TREE': 63, 'LAY_ASSET': 24, 'LAY_INSTANCE': 451}`
- Blender modifiers/constraints/parents/collection instances: 0/0/0/0
- WMB mesh UV/color signatures: `{(('UVMap1', 'UVMap2', 'UVMap3', 'UVMap4'), ('Col',)): 508}`
- materials using Vertex Color node: `['DRY_ground_grass', 'DRY_yo_asphalt_01']`

## CityOfRuinMainField.blend

- domain: `{'COL_MESH': 335, 'COL_TREE': 253, 'LAY_ASSET': 54, 'LAY_INSTANCE': 2051, 'WMB_MESH': 446, 'WMB_TREE': 63}`
- SHADOW: objects 34, semantic bases 8, vertices 20,840, triangles 7,972, materials 3
- LOD0: objects 306, semantic bases 16, vertices 269,979, triangles 185,435, materials 20
- LOD1: objects 69, semantic bases 14, vertices 152,158, triangles 61,657, materials 14
- LOD2: objects 37, semantic bases 7, vertices 9,130, triangles 4,269, materials 4

| Semantic mesh group | Object count by pass | Triangle count by pass |
|---|---:|---:|
| `_build_11220_07` | SHADOW=3, LOD0=14, LOD1=3, LOD2=3 | SHADOW=30, LOD0=13,129, LOD1=6,516, LOD2=296 |
| `_build_11220_16` | SHADOW=5, LOD0=18, LOD1=5, LOD2=5 | SHADOW=10, LOD0=23,643, LOD1=10,557, LOD2=1,589 |
| `add_debris` | LOD0=9 | LOD0=2,042 |
| `brokenhighway_11220` | SHADOW=2, LOD0=6, LOD1=2, LOD2=2 | SHADOW=1,863, LOD0=4,662, LOD1=1,863, LOD2=197 |
| `building_11220_0` | SHADOW=5, LOD0=24, LOD1=7, LOD2=8 | SHADOW=30, LOD0=12,213, LOD1=5,895, LOD2=452 |
| `building_11220_1` | SHADOW=2, LOD0=24, LOD1=6, LOD2=5 | SHADOW=10, LOD0=25,606, LOD1=7,559, LOD2=764 |
| `building_11220_2` | SHADOW=6, LOD0=36, LOD1=8, LOD2=8 | SHADOW=20, LOD0=24,902, LOD1=8,952, LOD2=682 |
| `building_11220_3` | SHADOW=4, LOD0=25, LOD1=6, LOD2=6 | SHADOW=10, LOD0=18,841, LOD1=6,286, LOD2=289 |
| `GROUND_11220` | LOD0=23 | LOD0=23,502 |
| `GROUND_11220road_00` | LOD0=11, LOD1=2 | LOD0=2,254, LOD1=1,352 |
| `GROUND_11220road_01` | LOD0=29, LOD1=7 | LOD0=4,046, LOD1=2,425 |
| `GROUND_11220road_02` | LOD0=9, LOD1=3 | LOD0=530, LOD1=317 |
| `GROUND_11220road_03` | LOD0=20, LOD1=6 | LOD0=1,832, LOD1=1,098 |
| `GROUND_11220road_04` | LOD0=8, LOD1=2 | LOD0=2,196, LOD1=1,317 |
| `GROUND_11220road_05` | LOD0=22, LOD1=5 | LOD0=2,768, LOD1=1,660 |
| `highway_11220` | SHADOW=7, LOD0=28, LOD1=7 | SHADOW=5,999, LOD0=23,269, LOD1=5,860 |

- LOD level-set patterns: `{(0,): 2, (-1, 0, 1, 2): 7, (-1, 0, 1): 1, (0, 1): 6}`
- LOD name/property mismatches: 0
- COL type: `{3: 282, 1: 37, 4: 12, 0: 4}`
- COL surface: `{3: 124, 9: 116, 1: 51, 19: 29, 4: 6, 8: 7, 6: 2}`
- COL modifier field: `{0: 335}`
- LAY model placements: `{'bgd000': 1612, 'bgd004': 177, 'bgd005': 249, 'bg2002': 4, 'bg2060': 1, 'bg2010': 2, 'bgd00b': 22, 'bgd023': 13, 'bgd026': 19, 'bgd024': 4, 'bgd025': 2}`
- LAY orphan instance keys: `{}`
- WMB spatial tree: nodes 63, leaves 32, root [0], missing/out-of-range/duplicate/property-mismatch/branch-error 0/0/0/0/0
- COL spatial tree: nodes 253, leaves 127, root [0], missing/out-of-range/duplicate/branch-error 0/0/0/0
- non-identity transforms by domain: `{'COL_TREE': 253, 'WMB_TREE': 63, 'LAY_ASSET': 54, 'LAY_INSTANCE': 2051}`
- Blender modifiers/constraints/parents/collection instances: 0/0/0/0
- WMB mesh UV/color signatures: `{(('UVMap1', 'UVMap2', 'UVMap3', 'UVMap4'), ('Col',)): 446}`
- materials using Vertex Color node: `['DRY_ground_grass', 'DRY_yo_asphalt_01', 'kaj_bark4_aaa9_1']`
