---
type: source
date: 2026-08-30
status: active
source: Client/bin/resources/Models/World/CityOfRuinEntry/CityOfRuinEntry.fbx; g11021.col; Layout.lay
owner: user
repo_path: C:\Users\a9018\Desktop\Direct11_Nier_Automata
aliases:
  - CityOfRuinEntry FBX Mesh Inventory
  - g11021 Mesh Name Analysis
---

# CityOfRuinEntry FBX 전체 메시 이름 분석

## Summary

- 입력 FBX: `C:\Users\a9018\Desktop\Direct11_Nier_Automata\Client\bin\resources\Models\World\CityOfRuinEntry\CityOfRuinEntry.fbx` (124,656,236 bytes)
- 전체 메시: 916개, 정점 750,879개, 삼각형 403,622개
- 공간 범위: min `[-311.2834777832031, -523.998046875, -91.21161651611328]`, max `[373.9430236816406, 501.75018310546875, 176.9464111328125]`
- FBX에는 Collection, parent hierarchy, Empty, collection instance, 공유 Mesh datablock이 남아 있지 않다. 모든 메시가 루트에 평탄화되고 배치 transform이 vertex 좌표에 구워져 있다.
- `HexGround` 401개는 원본 `g11021.col`의 name group 1개, mesh/batch 401개와 정확히 대응한다. 원본 COL은 15,011 triangles지만 FBX에는 14,977 triangles만 남아 있어 FBX 왕복 중 34개 삼각형이 탈락했다.
- `HexGround`를 제외한 515개는 재질·UV4·vertex color를 가진 WMB 시각 메시다. 이 중 명시적 `-LOD1` 3개를 제외하면 512개로, g11021 wd1의 문서상 object 수와 일치한다.
- 원본 `Layout.lay`에는 model 9종, asset record 26개, 추가 instance 372개가 있다. FBX에는 `-Instance` Empty나 LAY collection이 하나도 없으므로 LAY 배치 데이터는 포함되지 않았다.
- WMB 시각 메시 515개는 모두 `UVMap1~4`와 `Col` vertex color를 갖지만 현재 ModelConverter/VTXMESH는 UV0 하나만 저장하고 vertex color를 버린다.
- FBX는 material name 50개를 보존했지만 image texture node가 확인된 material은 `kaj_tree_leaf_aaa1_1` 하나뿐이고, 같은 `5C912AB1.dds` 참조만 중복되어 있다. 따라서 FBX embedded material은 입력 계약으로 사용할 수 없다.
- 같은 폴더의 `materials.json`은 FBX 50개 material name과 정확히 1:1 일치하며 texture binding 360개, unique hash 131개를 보존한다. local g11021 및 같은 data family WDA cache 대조 결과 128개가 실제 DDS로 resolve되고 `1FBC0984`, `4E9C16F4`, `7FD4929A` 3개는 공통 Env/Irradiance, Light/Mask, DetailNormal placeholder 성격으로 별도 엔진 default binding이 필요하다.

## Classification Evidence

- `COL`: raw `g11021.col` parser 결과와 name·mesh count·vertex count가 일치하는 `HexGround`.
- `WMB Internal LOD1`: 이름에 `-LOD1`이 명시된 3개 시각 메시.
- `WMB Distant`: `g11021_DistantView` 16개.
- 나머지 WMB: 512개 원본 object 집합에 속하는 시각 메시. 이름은 object 의미를 나타내지만 LAY instance는 아니다.
- Blender `.001` suffix는 동일 이름 충돌을 피하기 위한 export/import suffix이며 원본 instance ID가 아니다.

## Base Name Groups

| Base name | Count | Source | Semantic | Vertices | Triangles | Unique geometry | Materials |
|---|---:|---|---|---:|---:|---:|---:|
| HexGround | 401 | COL | Static collision batch | 13,934 | 14,977 | 385 | 0 |
| g11021_highway | 150 | WMB | Highway visual | 207,006 | 150,056 | 150 | 16 |
| g11021_build3 | 77 | WMB | Building visual | 86,756 | 49,714 | 77 | 10 |
| ground | 73 | WMB | Ground visual | 9,681 | 6,318 | 73 | 12 |
| g11021_build4 | 53 | WMB | Building visual | 103,658 | 42,805 | 53 | 10 |
| g11021_build7 | 47 | WMB | Building visual | 41,800 | 19,915 | 46 | 6 |
| nolowmap_debris | 41 | WMB | Debris visual | 134,892 | 56,648 | 41 | 10 |
| g11021_build1 | 20 | WMB | Building visual | 48,532 | 20,807 | 20 | 8 |
| g11021_build2 | 19 | WMB | Building visual | 40,032 | 15,403 | 19 | 4 |
| g11021_DistantView | 16 | WMB | Distant backdrop visual | 16,727 | 8,776 | 16 | 4 |
| g11021_build6 | 9 | WMB | Building visual | 30,594 | 11,624 | 9 | 2 |
| g11021_build5 | 4 | WMB | Building visual | 16,912 | 6,320 | 4 | 1 |
| MtRobot9 | 3 | WMB | Large prop visual | 249 | 173 | 3 | 2 |
| g11021_build1-LOD1 | 2 | WMB | Internal LOD1 visual | 86 | 76 | 2 | 2 |
| g11021_build6-LOD1 | 1 | WMB | Internal LOD1 visual | 20 | 10 | 1 | 1 |

## Raw COL Summary

- names: 1, mesh records: 401, COL tree nodes: 411
- `HexGround` collision types: `127` 297개, `255` 5개, `3` 68개, 미등록값 `95` 4개와 `16` 27개.
- modifiers: `0` 394개, transparent wall `2` 7개.
- surfaces: rock `2` 159개, concrete `0` 100개, metal solid `3` 55개, rubble2 `16` 48개, grass2 `9` 32개, metal grate `5` 4개, gravel1 `13` 3개.

## Raw LAY Placements

Asset record 자체도 하나의 배치 transform이며 `instance_count`는 그 뒤에 추가되는 복제 수다. 따라서 총 placement는 26 + 372 = 398개다.

| Asset name | Model key | Base placement | Extra instances | Total placements |
|---|---|---:|---:|---:|
| bg008e_61 | bg008e | 1 | 0 | 1 |
| bg008e_60 | bg008e | 1 | 0 | 1 |
| bg2060_1 | bg2060 | 1 | 0 | 1 |
| bg2004_2 | bg2004 | 1 | 0 | 1 |
| bg2004_3 | bg2004 | 1 | 0 | 1 |
| bg2060_2 | bg2060 | 1 | 0 | 1 |
| bg2043_1 | bg2043 | 1 | 0 | 1 |
| bg2043_2 | bg2043 | 1 | 0 | 1 |
| bg2043_3 | bg2043 | 1 | 0 | 1 |
| bg2003_3 | bg2003 | 1 | 0 | 1 |
| bg2043_4 | bg2043 | 1 | 0 | 1 |
| bg2043_5 | bg2043 | 1 | 0 | 1 |
| bg2043_6 | bg2043 | 1 | 0 | 1 |
| bgd00b_6 | bgd00b | 1 | 5 | 6 |
| bgd004_4 | bgd004 | 1 | 15 | 16 |
| bgd004_5 | bgd004 | 1 | 4 | 5 |
| bgd004_15 | bgd004 | 1 | 20 | 21 |
| bgd004_43 | bgd004 | 1 | 4 | 5 |
| bgd002_1 | bgd002 | 1 | 6 | 7 |
| bgd002_12 | bgd002 | 1 | 4 | 5 |
| bgd000_287 | bgd000 | 1 | 28 | 29 |
| bgd000_169 | bgd000 | 1 | 74 | 75 |
| bgd000_291 | bgd000 | 1 | 40 | 41 |
| bgd000_23 | bgd000 | 1 | 67 | 68 |
| bgd000_202 | bgd000 | 1 | 40 | 41 |
| bgd000_289 | bgd000 | 1 | 65 | 66 |

## Full FBX Mesh Inventory

916개 이름을 Blender import 순서대로 모두 보존한다. 좌표 범위와 geometry hash까지 포함한 기계 판독용 전체 필드는 companion CSV를 사용한다.

| # | Mesh name | Base name | Source | Semantic | Vertices | Triangles | Material | UV | Color |
|---:|---|---|---|---|---:|---:|---|---|---|
| 0 | g11021_build1 | g11021_build1 | WMB | Building visual | 2425 | 819 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 1 | g11021_build1-LOD1 | g11021_build1-LOD1 | WMB | Internal LOD1 visual | 20 | 10 | lowbuildingmodel_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 2 | g11021_build1-LOD1.001 | g11021_build1-LOD1 | WMB | Internal LOD1 visual | 66 | 66 | mas_low_building01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 3 | g11021_build1.001 | g11021_build1 | WMB | Building visual | 488 | 246 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 4 | g11021_build1.002 | g11021_build1 | WMB | Building visual | 180 | 90 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 5 | g11021_build1.003 | g11021_build1 | WMB | Building visual | 282 | 140 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 6 | g11021_build1.004 | g11021_build1 | WMB | Building visual | 3386 | 1672 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 7 | g11021_build1.005 | g11021_build1 | WMB | Building visual | 1190 | 586 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 8 | g11021_build1.006 | g11021_build1 | WMB | Building visual | 2896 | 1428 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 9 | g11021_build1.007 | g11021_build1 | WMB | Building visual | 1189 | 590 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 10 | g11021_build1.008 | g11021_build1 | WMB | Building visual | 550 | 264 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 11 | g11021_build1.009 | g11021_build1 | WMB | Building visual | 1905 | 894 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 12 | g11021_build1.010 | g11021_build1 | WMB | Building visual | 2584 | 1338 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 13 | g11021_build1.011 | g11021_build1 | WMB | Building visual | 788 | 406 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 14 | g11021_build1.012 | g11021_build1 | WMB | Building visual | 2271 | 1214 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 15 | g11021_build1.013 | g11021_build1 | WMB | Building visual | 14436 | 5792 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 16 | g11021_build1.014 | g11021_build1 | WMB | Building visual | 6481 | 2481 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 17 | g11021_build1.015 | g11021_build1 | WMB | Building visual | 5743 | 2176 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 18 | g11021_build1.016 | g11021_build1 | WMB | Building visual | 295 | 107 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 19 | g11021_build1.017 | g11021_build1 | WMB | Building visual | 910 | 326 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 20 | g11021_build1.018 | g11021_build1 | WMB | Building visual | 521 | 232 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 21 | g11021_build1.019 | g11021_build1 | WMB | Building visual | 12 | 6 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 22 | g11021_build2 | g11021_build2 | WMB | Building visual | 20 | 10 | lowbuildingmodel_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 23 | g11021_build2.001 | g11021_build2 | WMB | Building visual | 16 | 8 | lowbuildingmodel_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 24 | g11021_build2.002 | g11021_build2 | WMB | Building visual | 4 | 2 | lowbuildingmodel_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 25 | g11021_build2.003 | g11021_build2 | WMB | Building visual | 194 | 97 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 26 | g11021_build2.004 | g11021_build2 | WMB | Building visual | 790 | 385 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 27 | g11021_build2.005 | g11021_build2 | WMB | Building visual | 24 | 8 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 28 | g11021_build2.006 | g11021_build2 | WMB | Building visual | 48 | 42 | mas_low_building01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 29 | g11021_build2.007 | g11021_build2 | WMB | Building visual | 33 | 24 | mas_low_building01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 30 | g11021_build2.008 | g11021_build2 | WMB | Building visual | 65 | 66 | mas_low_building01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 31 | g11021_build2.009 | g11021_build2 | WMB | Building visual | 7187 | 2890 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 32 | g11021_build2.010 | g11021_build2 | WMB | Building visual | 3156 | 1220 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 33 | g11021_build2.011 | g11021_build2 | WMB | Building visual | 3944 | 1459 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 34 | g11021_build2.012 | g11021_build2 | WMB | Building visual | 9015 | 3331 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 35 | g11021_build2.013 | g11021_build2 | WMB | Building visual | 1828 | 707 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 36 | g11021_build2.014 | g11021_build2 | WMB | Building visual | 2649 | 992 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 37 | g11021_build2.015 | g11021_build2 | WMB | Building visual | 2452 | 890 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 38 | g11021_build2.016 | g11021_build2 | WMB | Building visual | 54 | 18 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 39 | g11021_build2.017 | g11021_build2 | WMB | Building visual | 7966 | 3043 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 40 | g11021_build2.018 | g11021_build2 | WMB | Building visual | 587 | 211 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 41 | g11021_build3 | g11021_build3 | WMB | Building visual | 1432 | 956 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 42 | g11021_build3.001 | g11021_build3 | WMB | Building visual | 1053 | 715 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 43 | g11021_build3.002 | g11021_build3 | WMB | Building visual | 3681 | 1803 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 44 | g11021_build3.003 | g11021_build3 | WMB | Building visual | 5008 | 3282 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 45 | g11021_build3.004 | g11021_build3 | WMB | Building visual | 5756 | 3566 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 46 | g11021_build3.005 | g11021_build3 | WMB | Building visual | 252 | 127 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 47 | g11021_build3.006 | g11021_build3 | WMB | Building visual | 428 | 215 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 48 | g11021_build3.007 | g11021_build3 | WMB | Building visual | 1177 | 781 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 49 | g11021_build3.008 | g11021_build3 | WMB | Building visual | 3196 | 2396 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 50 | g11021_build3.009 | g11021_build3 | WMB | Building visual | 4 | 2 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 51 | g11021_build3.010 | g11021_build3 | WMB | Building visual | 18 | 6 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 52 | g11021_build3.011 | g11021_build3 | WMB | Building visual | 159 | 92 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 53 | g11021_build3.012 | g11021_build3 | WMB | Building visual | 15 | 11 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 54 | g11021_build3.013 | g11021_build3 | WMB | Building visual | 50 | 43 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 55 | g11021_build3.014 | g11021_build3 | WMB | Building visual | 12 | 4 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 56 | g11021_build3.015 | g11021_build3 | WMB | Building visual | 20 | 14 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 57 | g11021_build3.016 | g11021_build3 | WMB | Building visual | 1050 | 520 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 58 | g11021_build3.017 | g11021_build3 | WMB | Building visual | 420 | 208 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 59 | g11021_build3.018 | g11021_build3 | WMB | Building visual | 913 | 376 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 60 | g11021_build3.019 | g11021_build3 | WMB | Building visual | 4884 | 2044 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 61 | g11021_build3.020 | g11021_build3 | WMB | Building visual | 2972 | 1364 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 62 | g11021_build3.021 | g11021_build3 | WMB | Building visual | 3356 | 1355 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 63 | g11021_build3.022 | g11021_build3 | WMB | Building visual | 365 | 268 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 64 | g11021_build3.023 | g11021_build3 | WMB | Building visual | 397 | 283 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 65 | g11021_build3.024 | g11021_build3 | WMB | Building visual | 118 | 96 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 66 | g11021_build3.025 | g11021_build3 | WMB | Building visual | 118 | 96 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 67 | g11021_build3.026 | g11021_build3 | WMB | Building visual | 16 | 8 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 68 | g11021_build3.027 | g11021_build3 | WMB | Building visual | 28 | 14 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 69 | g11021_build3.028 | g11021_build3 | WMB | Building visual | 118 | 96 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 70 | g11021_build3.029 | g11021_build3 | WMB | Building visual | 2356 | 1250 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 71 | g11021_build3.030 | g11021_build3 | WMB | Building visual | 2705 | 1639 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 72 | g11021_build3.031 | g11021_build3 | WMB | Building visual | 2668 | 1601 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 73 | g11021_build3.032 | g11021_build3 | WMB | Building visual | 224 | 112 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 74 | g11021_build3.033 | g11021_build3 | WMB | Building visual | 802 | 425 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 75 | g11021_build3.034 | g11021_build3 | WMB | Building visual | 2154 | 1292 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 76 | g11021_build3.035 | g11021_build3 | WMB | Building visual | 872 | 483 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 77 | g11021_build3.036 | g11021_build3 | WMB | Building visual | 502 | 396 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 78 | g11021_build3.037 | g11021_build3 | WMB | Building visual | 659 | 464 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 79 | g11021_build3.038 | g11021_build3 | WMB | Building visual | 380 | 190 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 80 | g11021_build3.039 | g11021_build3 | WMB | Building visual | 726 | 496 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 81 | g11021_build3.040 | g11021_build3 | WMB | Building visual | 593 | 431 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 82 | g11021_build3.041 | g11021_build3 | WMB | Building visual | 878 | 548 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 83 | g11021_build3.042 | g11021_build3 | WMB | Building visual | 702 | 471 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 84 | g11021_build3.043 | g11021_build3 | WMB | Building visual | 726 | 483 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 85 | g11021_build3.044 | g11021_build3 | WMB | Building visual | 248 | 130 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 86 | g11021_build3.045 | g11021_build3 | WMB | Building visual | 1100 | 686 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 87 | g11021_build3.046 | g11021_build3 | WMB | Building visual | 186 | 128 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 88 | g11021_build3.047 | g11021_build3 | WMB | Building visual | 4077 | 2618 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 89 | g11021_build3.048 | g11021_build3 | WMB | Building visual | 996 | 534 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 90 | g11021_build3.049 | g11021_build3 | WMB | Building visual | 2032 | 1092 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 91 | g11021_build3.050 | g11021_build3 | WMB | Building visual | 587 | 419 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 92 | g11021_build3.051 | g11021_build3 | WMB | Building visual | 3200 | 1692 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 93 | g11021_build3.052 | g11021_build3 | WMB | Building visual | 2193 | 1290 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 94 | g11021_build3.053 | g11021_build3 | WMB | Building visual | 2941 | 1590 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 95 | g11021_build3.054 | g11021_build3 | WMB | Building visual | 203 | 121 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 96 | g11021_build3.055 | g11021_build3 | WMB | Building visual | 28 | 14 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 97 | g11021_build3.056 | g11021_build3 | WMB | Building visual | 448 | 273 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 98 | g11021_build3.057 | g11021_build3 | WMB | Building visual | 87 | 59 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 99 | g11021_build3.058 | g11021_build3 | WMB | Building visual | 11 | 7 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 100 | g11021_build3.059 | g11021_build3 | WMB | Building visual | 373 | 153 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 101 | g11021_build3.060 | g11021_build3 | WMB | Building visual | 2410 | 982 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 102 | g11021_build3.061 | g11021_build3 | WMB | Building visual | 1551 | 874 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 103 | g11021_build3.062 | g11021_build3 | WMB | Building visual | 2573 | 1278 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 104 | g11021_build3.063 | g11021_build3 | WMB | Building visual | 5 | 3 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 105 | g11021_build3.064 | g11021_build3 | WMB | Building visual | 39 | 21 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 106 | g11021_build3.065 | g11021_build3 | WMB | Building visual | 364 | 241 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 107 | g11021_build3.066 | g11021_build3 | WMB | Building visual | 880 | 663 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 108 | g11021_build3.067 | g11021_build3 | WMB | Building visual | 4027 | 2155 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 109 | g11021_build3.068 | g11021_build3 | WMB | Building visual | 270 | 288 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 110 | g11021_build3.069 | g11021_build3 | WMB | Building visual | 309 | 270 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 111 | g11021_build3.070 | g11021_build3 | WMB | Building visual | 116 | 43 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 112 | g11021_build3.071 | g11021_build3 | WMB | Building visual | 343 | 241 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 113 | g11021_build3.072 | g11021_build3 | WMB | Building visual | 402 | 258 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 114 | g11021_build3.073 | g11021_build3 | WMB | Building visual | 320 | 240 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 115 | g11021_build3.074 | g11021_build3 | WMB | Building visual | 395 | 277 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 116 | g11021_build3.075 | g11021_build3 | WMB | Building visual | 71 | 48 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 117 | g11021_build3.076 | g11021_build3 | WMB | Building visual | 8 | 4 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 118 | g11021_build4 | g11021_build4 | WMB | Building visual | 774 | 260 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 119 | g11021_build4.001 | g11021_build4 | WMB | Building visual | 1843 | 619 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 120 | g11021_build4.002 | g11021_build4 | WMB | Building visual | 1978 | 660 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 121 | g11021_build4.003 | g11021_build4 | WMB | Building visual | 11243 | 3776 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 122 | g11021_build4.004 | g11021_build4 | WMB | Building visual | 9074 | 3036 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 123 | g11021_build4.005 | g11021_build4 | WMB | Building visual | 4 | 2 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 124 | g11021_build4.006 | g11021_build4 | WMB | Building visual | 10 | 4 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 125 | g11021_build4.007 | g11021_build4 | WMB | Building visual | 48 | 23 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 126 | g11021_build4.008 | g11021_build4 | WMB | Building visual | 31 | 11 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 127 | g11021_build4.009 | g11021_build4 | WMB | Building visual | 161 | 60 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 128 | g11021_build4.010 | g11021_build4 | WMB | Building visual | 110 | 57 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 129 | g11021_build4.011 | g11021_build4 | WMB | Building visual | 10 | 4 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 130 | g11021_build4.012 | g11021_build4 | WMB | Building visual | 1377 | 623 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 131 | g11021_build4.013 | g11021_build4 | WMB | Building visual | 1761 | 791 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 132 | g11021_build4.014 | g11021_build4 | WMB | Building visual | 7310 | 2501 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 133 | g11021_build4.015 | g11021_build4 | WMB | Building visual | 2552 | 904 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 134 | g11021_build4.016 | g11021_build4 | WMB | Building visual | 489 | 246 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 135 | g11021_build4.017 | g11021_build4 | WMB | Building visual | 148 | 74 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 136 | g11021_build4.018 | g11021_build4 | WMB | Building visual | 268 | 136 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 137 | g11021_build4.019 | g11021_build4 | WMB | Building visual | 191 | 78 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 138 | g11021_build4.020 | g11021_build4 | WMB | Building visual | 369 | 158 | os11_ruinsparts00_floor_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 139 | g11021_build4.021 | g11021_build4 | WMB | Building visual | 3410 | 1684 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 140 | g11021_build4.022 | g11021_build4 | WMB | Building visual | 1086 | 534 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 141 | g11021_build4.023 | g11021_build4 | WMB | Building visual | 2592 | 1284 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 142 | g11021_build4.024 | g11021_build4 | WMB | Building visual | 3267 | 1592 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 143 | g11021_build4.025 | g11021_build4 | WMB | Building visual | 5661 | 2784 | os11_ruinsparts00_parts_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 144 | g11021_build4.026 | g11021_build4 | WMB | Building visual | 1188 | 590 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 145 | g11021_build4.027 | g11021_build4 | WMB | Building visual | 550 | 264 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 146 | g11021_build4.028 | g11021_build4 | WMB | Building visual | 1652 | 795 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 147 | g11021_build4.029 | g11021_build4 | WMB | Building visual | 568 | 296 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 148 | g11021_build4.030 | g11021_build4 | WMB | Building visual | 1439 | 696 | os11_ruinsparts00_wall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 149 | g11021_build4.031 | g11021_build4 | WMB | Building visual | 2676 | 1338 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 150 | g11021_build4.032 | g11021_build4 | WMB | Building visual | 812 | 406 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 151 | g11021_build4.033 | g11021_build4 | WMB | Building visual | 2106 | 1131 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 152 | g11021_build4.034 | g11021_build4 | WMB | Building visual | 741 | 390 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 153 | g11021_build4.035 | g11021_build4 | WMB | Building visual | 1310 | 731 | os11_ruinsparts00_wall02_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 154 | g11021_build4.036 | g11021_build4 | WMB | Building visual | 4297 | 1728 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 155 | g11021_build4.037 | g11021_build4 | WMB | Building visual | 910 | 448 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 156 | g11021_build4.038 | g11021_build4 | WMB | Building visual | 1824 | 847 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 157 | g11021_build4.039 | g11021_build4 | WMB | Building visual | 1369 | 611 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 158 | g11021_build4.040 | g11021_build4 | WMB | Building visual | 2479 | 1000 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 159 | g11021_build4.041 | g11021_build4 | WMB | Building visual | 5341 | 2287 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 160 | g11021_build4.042 | g11021_build4 | WMB | Building visual | 1873 | 851 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 161 | g11021_build4.043 | g11021_build4 | WMB | Building visual | 127 | 59 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 162 | g11021_build4.044 | g11021_build4 | WMB | Building visual | 525 | 183 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 163 | g11021_build4.045 | g11021_build4 | WMB | Building visual | 36 | 19 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 164 | g11021_build4.046 | g11021_build4 | WMB | Building visual | 4108 | 1510 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 165 | g11021_build4.047 | g11021_build4 | WMB | Building visual | 2623 | 970 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 166 | g11021_build4.048 | g11021_build4 | WMB | Building visual | 6278 | 2155 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 167 | g11021_build4.049 | g11021_build4 | WMB | Building visual | 632 | 305 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 168 | g11021_build4.050 | g11021_build4 | WMB | Building visual | 830 | 524 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 169 | g11021_build4.051 | g11021_build4 | WMB | Building visual | 745 | 336 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 170 | g11021_build4.052 | g11021_build4 | WMB | Building visual | 852 | 434 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 171 | g11021_build5 | g11021_build5 | WMB | Building visual | 124 | 42 | g11021_build5_material_albedo_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 172 | g11021_build5.001 | g11021_build5 | WMB | Building visual | 6612 | 2527 | g11021_build5_material_albedo_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 173 | g11021_build5.002 | g11021_build5 | WMB | Building visual | 7065 | 2595 | g11021_build5_material_albedo_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 174 | g11021_build5.003 | g11021_build5 | WMB | Building visual | 3111 | 1156 | g11021_build5_material_albedo_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 175 | g11021_build6 | g11021_build6 | WMB | Building visual | 20 | 8 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 176 | g11021_build6-LOD1 | g11021_build6-LOD1 | WMB | Internal LOD1 visual | 20 | 10 | lowbuildingmodel_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 177 | g11021_build6.001 | g11021_build6 | WMB | Building visual | 150 | 95 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 178 | g11021_build6.002 | g11021_build6 | WMB | Building visual | 2595 | 1169 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 179 | g11021_build6.003 | g11021_build6 | WMB | Building visual | 1867 | 719 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 180 | g11021_build6.004 | g11021_build6 | WMB | Building visual | 7337 | 2751 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 181 | g11021_build6.005 | g11021_build6 | WMB | Building visual | 14722 | 5428 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 182 | g11021_build6.006 | g11021_build6 | WMB | Building visual | 3182 | 1190 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 183 | g11021_build6.007 | g11021_build6 | WMB | Building visual | 3 | 1 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 184 | g11021_build6.008 | g11021_build6 | WMB | Building visual | 718 | 263 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 185 | g11021_build7 | g11021_build7 | WMB | Building visual | 6232 | 2838 | bg008b_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 186 | g11021_build7.001 | g11021_build7 | WMB | Building visual | 347 | 190 | bg008b_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 187 | g11021_build7.002 | g11021_build7 | WMB | Building visual | 614 | 284 | bg008b_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 188 | g11021_build7.003 | g11021_build7 | WMB | Building visual | 1546 | 630 | bg008b_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 189 | g11021_build7.004 | g11021_build7 | WMB | Building visual | 1885 | 764 | bg008b_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 190 | g11021_build7.005 | g11021_build7 | WMB | Building visual | 4723 | 2130 | bg008c_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 191 | g11021_build7.006 | g11021_build7 | WMB | Building visual | 637 | 290 | bg008c_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 192 | g11021_build7.007 | g11021_build7 | WMB | Building visual | 280 | 125 | bg008c_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 193 | g11021_build7.008 | g11021_build7 | WMB | Building visual | 1745 | 759 | bg008c_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 194 | g11021_build7.009 | g11021_build7 | WMB | Building visual | 327 | 138 | bg008c_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 195 | g11021_build7.010 | g11021_build7 | WMB | Building visual | 525 | 216 | bg008c_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 196 | g11021_build7.011 | g11021_build7 | WMB | Building visual | 1363 | 564 | bg008c_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 197 | g11021_build7.012 | g11021_build7 | WMB | Building visual | 509 | 232 | bg008c_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 198 | g11021_build7.013 | g11021_build7 | WMB | Building visual | 30 | 12 | bg008c_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 199 | g11021_build7.014 | g11021_build7 | WMB | Building visual | 2177 | 903 | bg008c_building1_Material | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 200 | g11021_build7.015 | g11021_build7 | WMB | Building visual | 40 | 20 | mas_pipebig01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 201 | g11021_build7.016 | g11021_build7 | WMB | Building visual | 8 | 4 | mas_pipebig01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 202 | g11021_build7.017 | g11021_build7 | WMB | Building visual | 8 | 4 | mas_pipebig01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 203 | g11021_build7.018 | g11021_build7 | WMB | Building visual | 28 | 14 | mas_pipebig01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 204 | g11021_build7.019 | g11021_build7 | WMB | Building visual | 8 | 4 | mas_pipebig01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 205 | g11021_build7.020 | g11021_build7 | WMB | Building visual | 20 | 10 | mas_pipebig01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 206 | g11021_build7.021 | g11021_build7 | WMB | Building visual | 16 | 8 | mas_pipebig01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 207 | g11021_build7.022 | g11021_build7 | WMB | Building visual | 40 | 20 | mas_pipebig01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 208 | g11021_build7.023 | g11021_build7 | WMB | Building visual | 2104 | 1052 | os09_buildingparts01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 209 | g11021_build7.024 | g11021_build7 | WMB | Building visual | 304 | 152 | os09_buildingparts01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 210 | g11021_build7.025 | g11021_build7 | WMB | Building visual | 24 | 12 | os09_buildingparts01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 211 | g11021_build7.026 | g11021_build7 | WMB | Building visual | 568 | 284 | os09_buildingparts01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 212 | g11021_build7.027 | g11021_build7 | WMB | Building visual | 8 | 4 | os09_buildingparts01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 213 | g11021_build7.028 | g11021_build7 | WMB | Building visual | 704 | 352 | os09_buildingparts01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 214 | g11021_build7.029 | g11021_build7 | WMB | Building visual | 168 | 84 | os09_buildingparts01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 215 | g11021_build7.030 | g11021_build7 | WMB | Building visual | 844 | 422 | os09_buildingparts01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 216 | g11021_build7.031 | g11021_build7 | WMB | Building visual | 192 | 96 | os09_pipetiling01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 217 | g11021_build7.032 | g11021_build7 | WMB | Building visual | 32 | 16 | os09_pipetiling01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 218 | g11021_build7.033 | g11021_build7 | WMB | Building visual | 32 | 16 | os09_pipetiling01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 219 | g11021_build7.034 | g11021_build7 | WMB | Building visual | 32 | 16 | os09_pipetiling01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 220 | g11021_build7.035 | g11021_build7 | WMB | Building visual | 32 | 16 | os09_pipetiling01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 221 | g11021_build7.036 | g11021_build7 | WMB | Building visual | 120 | 60 | os09_pipetiling01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 222 | g11021_build7.037 | g11021_build7 | WMB | Building visual | 5500 | 2904 | os09_stairladder01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 223 | g11021_build7.038 | g11021_build7 | WMB | Building visual | 1846 | 1038 | os09_stairladder01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 224 | g11021_build7.039 | g11021_build7 | WMB | Building visual | 288 | 158 | os09_stairladder01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 225 | g11021_build7.040 | g11021_build7 | WMB | Building visual | 1823 | 966 | os09_stairladder01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 226 | g11021_build7.041 | g11021_build7 | WMB | Building visual | 96 | 48 | os09_stairladder01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 227 | g11021_build7.042 | g11021_build7 | WMB | Building visual | 236 | 112 | os09_stairladder01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 228 | g11021_build7.043 | g11021_build7 | WMB | Building visual | 1288 | 706 | os09_stairladder01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 229 | g11021_build7.044 | g11021_build7 | WMB | Building visual | 364 | 176 | os09_stairladder01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 230 | g11021_build7.045 | g11021_build7 | WMB | Building visual | 8 | 4 | os09_stairladder01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 231 | g11021_build7.046 | g11021_build7 | WMB | Building visual | 2079 | 1062 | os09_stairladder01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 232 | g11021_DistantView | g11021_DistantView | WMB | Distant backdrop visual | 1862 | 1443 | kaj_tree_leaf_aaa1_1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 233 | g11021_DistantView.001 | g11021_DistantView | WMB | Distant backdrop visual | 16 | 8 | kaj_tree_leaf_aaa1_1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 234 | g11021_DistantView.002 | g11021_DistantView | WMB | Distant backdrop visual | 36 | 16 | kaj_tree_leaf_aaa1_1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 235 | g11021_DistantView.003 | g11021_DistantView | WMB | Distant backdrop visual | 36 | 16 | kaj_tree_leaf_aaa1_1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 236 | g11021_DistantView.004 | g11021_DistantView | WMB | Distant backdrop visual | 600 | 200 | g11121_buildings_06_material1_albedo_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 237 | g11021_DistantView.005 | g11021_DistantView | WMB | Distant backdrop visual | 69 | 23 | g11121_buildings_06_material1_albedo_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 238 | g11021_DistantView.006 | g11021_DistantView | WMB | Distant backdrop visual | 132 | 44 | g11121_buildings_06_material1_albedo_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 239 | g11021_DistantView.007 | g11021_DistantView | WMB | Distant backdrop visual | 264 | 88 | g11121_buildings_06_material1_albedo_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 240 | g11021_DistantView.008 | g11021_DistantView | WMB | Distant backdrop visual | 35 | 48 | kaj_ground_weed_01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 241 | g11021_DistantView.009 | g11021_DistantView | WMB | Distant backdrop visual | 418 | 731 | kaj_ground_weed_01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 242 | g11021_DistantView.010 | g11021_DistantView | WMB | Distant backdrop visual | 830 | 1425 | kaj_ground_weed_01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 243 | g11021_DistantView.011 | g11021_DistantView | WMB | Distant backdrop visual | 7212 | 2672 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 244 | g11021_DistantView.012 | g11021_DistantView | WMB | Distant backdrop visual | 950 | 371 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 245 | g11021_DistantView.013 | g11021_DistantView | WMB | Distant backdrop visual | 1798 | 698 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 246 | g11021_DistantView.014 | g11021_DistantView | WMB | Distant backdrop visual | 2463 | 991 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 247 | g11021_DistantView.015 | g11021_DistantView | WMB | Distant backdrop visual | 6 | 2 | yam_build01_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 248 | g11021_highway | g11021_highway | WMB | Highway visual | 1306 | 1033 | mas_BarricadeSet01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 249 | g11021_highway.001 | g11021_highway | WMB | Highway visual | 4348 | 3268 | mas_BarricadeSet01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 250 | g11021_highway.002 | g11021_highway | WMB | Highway visual | 14251 | 11214 | mas_BarricadeSet01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 251 | g11021_highway.003 | g11021_highway | WMB | Highway visual | 2978 | 2316 | mas_BarricadeSet01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 252 | g11021_highway.004 | g11021_highway | WMB | Highway visual | 189 | 126 | mas_BarricadeSet01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 253 | g11021_highway.005 | g11021_highway | WMB | Highway visual | 3554 | 2688 | mas_BarricadeSet01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 254 | g11021_highway.006 | g11021_highway | WMB | Highway visual | 3089 | 2454 | mas_BarricadeSet01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 255 | g11021_highway.007 | g11021_highway | WMB | Highway visual | 2552 | 1993 | mas_BarricadeSet01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 256 | g11021_highway.008 | g11021_highway | WMB | Highway visual | 268 | 181 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 257 | g11021_highway.009 | g11021_highway | WMB | Highway visual | 479 | 308 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 258 | g11021_highway.010 | g11021_highway | WMB | Highway visual | 3878 | 2687 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 259 | g11021_highway.011 | g11021_highway | WMB | Highway visual | 2340 | 1941 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 260 | g11021_highway.012 | g11021_highway | WMB | Highway visual | 2178 | 1547 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 261 | g11021_highway.013 | g11021_highway | WMB | Highway visual | 1887 | 1398 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 262 | g11021_highway.014 | g11021_highway | WMB | Highway visual | 2807 | 2144 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 263 | g11021_highway.015 | g11021_highway | WMB | Highway visual | 1203 | 925 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 264 | g11021_highway.016 | g11021_highway | WMB | Highway visual | 584 | 455 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 265 | g11021_highway.017 | g11021_highway | WMB | Highway visual | 2292 | 1702 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 266 | g11021_highway.018 | g11021_highway | WMB | Highway visual | 5526 | 4549 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 267 | g11021_highway.019 | g11021_highway | WMB | Highway visual | 5222 | 4272 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 268 | g11021_highway.020 | g11021_highway | WMB | Highway visual | 2015 | 1575 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 269 | g11021_highway.021 | g11021_highway | WMB | Highway visual | 1898 | 1292 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 270 | g11021_highway.022 | g11021_highway | WMB | Highway visual | 68 | 39 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 271 | g11021_highway.023 | g11021_highway | WMB | Highway visual | 12822 | 6810 | mas_fence01_AAA1_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 272 | g11021_highway.024 | g11021_highway | WMB | Highway visual | 6804 | 3628 | mas_fence01_AAA1_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 273 | g11021_highway.025 | g11021_highway | WMB | Highway visual | 11573 | 6162 | mas_fence01_AAA1_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 274 | g11021_highway.026 | g11021_highway | WMB | Highway visual | 5884 | 3144 | mas_fence01_AAA1_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 275 | g11021_highway.027 | g11021_highway | WMB | Highway visual | 2767 | 1433 | mas_fence01_AAA1_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 276 | g11021_highway.028 | g11021_highway | WMB | Highway visual | 352 | 176 | mas_fence01_AAA1_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 277 | g11021_highway.029 | g11021_highway | WMB | Highway visual | 4458 | 2403 | mas_fence01_AAA1_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 278 | g11021_highway.030 | g11021_highway | WMB | Highway visual | 276 | 148 | mas_fence01_AAA1_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 279 | g11021_highway.031 | g11021_highway | WMB | Highway visual | 4224 | 2491 | mas_objects01_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 280 | g11021_highway.032 | g11021_highway | WMB | Highway visual | 146 | 89 | mas_objects01_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 281 | g11021_highway.033 | g11021_highway | WMB | Highway visual | 792 | 437 | mas_objects01_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 282 | g11021_highway.034 | g11021_highway | WMB | Highway visual | 3341 | 2001 | mas_objects01_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 283 | g11021_highway.035 | g11021_highway | WMB | Highway visual | 60 | 35 | mas_objects01_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 284 | g11021_highway.036 | g11021_highway | WMB | Highway visual | 178 | 107 | mas_objects01_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 285 | g11021_highway.037 | g11021_highway | WMB | Highway visual | 330 | 200 | mas_pipebig01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 286 | g11021_highway.038 | g11021_highway | WMB | Highway visual | 165 | 100 | mas_pipebig01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 287 | g11021_highway.039 | g11021_highway | WMB | Highway visual | 465 | 300 | mas_pipebig01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 288 | g11021_highway.040 | g11021_highway | WMB | Highway visual | 270 | 180 | mas_pipebig01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 289 | g11021_highway.041 | g11021_highway | WMB | Highway visual | 1043 | 552 | metal_p | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 290 | g11021_highway.042 | g11021_highway | WMB | Highway visual | 581 | 312 | metal_p | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 291 | g11021_highway.043 | g11021_highway | WMB | Highway visual | 1620 | 864 | metal_p | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 292 | g11021_highway.044 | g11021_highway | WMB | Highway visual | 1620 | 864 | metal_p | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 293 | g11021_highway.045 | g11021_highway | WMB | Highway visual | 1620 | 864 | metal_p | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 294 | g11021_highway.046 | g11021_highway | WMB | Highway visual | 15 | 7 | metal_plate | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 295 | g11021_highway.047 | g11021_highway | WMB | Highway visual | 435 | 386 | metal_plate | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 296 | g11021_highway.048 | g11021_highway | WMB | Highway visual | 189 | 150 | metal_plate | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 297 | g11021_highway.049 | g11021_highway | WMB | Highway visual | 90 | 90 | metal_plate | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 298 | g11021_highway.050 | g11021_highway | WMB | Highway visual | 320 | 220 | metal_plate | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 299 | g11021_highway.051 | g11021_highway | WMB | Highway visual | 484 | 342 | metal_plate | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 300 | g11021_highway.052 | g11021_highway | WMB | Highway visual | 162 | 110 | metal_plate | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 301 | g11021_highway.053 | g11021_highway | WMB | Highway visual | 12 | 6 | metal_plate | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 302 | g11021_highway.054 | g11021_highway | WMB | Highway visual | 90 | 58 | metal_plate | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 303 | g11021_highway.055 | g11021_highway | WMB | Highway visual | 205 | 167 | metal_plate | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 304 | g11021_highway.056 | g11021_highway | WMB | Highway visual | 162 | 112 | metal_plate | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 305 | g11021_highway.057 | g11021_highway | WMB | Highway visual | 30 | 25 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 306 | g11021_highway.058 | g11021_highway | WMB | Highway visual | 172 | 167 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 307 | g11021_highway.059 | g11021_highway | WMB | Highway visual | 1253 | 1277 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 308 | g11021_highway.060 | g11021_highway | WMB | Highway visual | 446 | 352 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 309 | g11021_highway.061 | g11021_highway | WMB | Highway visual | 498 | 519 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 310 | g11021_highway.062 | g11021_highway | WMB | Highway visual | 702 | 673 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 311 | g11021_highway.063 | g11021_highway | WMB | Highway visual | 532 | 482 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 312 | g11021_highway.064 | g11021_highway | WMB | Highway visual | 279 | 248 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 313 | g11021_highway.065 | g11021_highway | WMB | Highway visual | 309 | 266 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 314 | g11021_highway.066 | g11021_highway | WMB | Highway visual | 794 | 686 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 315 | g11021_highway.067 | g11021_highway | WMB | Highway visual | 656 | 508 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 316 | g11021_highway.068 | g11021_highway | WMB | Highway visual | 1157 | 1010 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 317 | g11021_highway.069 | g11021_highway | WMB | Highway visual | 520 | 447 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 318 | g11021_highway.070 | g11021_highway | WMB | Highway visual | 776 | 799 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 319 | g11021_highway.071 | g11021_highway | WMB | Highway visual | 262 | 259 | os08_concrete_tileable01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 320 | g11021_highway.072 | g11021_highway | WMB | Highway visual | 76 | 56 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 321 | g11021_highway.073 | g11021_highway | WMB | Highway visual | 205 | 149 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 322 | g11021_highway.074 | g11021_highway | WMB | Highway visual | 2354 | 1842 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 323 | g11021_highway.075 | g11021_highway | WMB | Highway visual | 3270 | 2536 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 324 | g11021_highway.076 | g11021_highway | WMB | Highway visual | 2106 | 1650 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 325 | g11021_highway.077 | g11021_highway | WMB | Highway visual | 2367 | 1730 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 326 | g11021_highway.078 | g11021_highway | WMB | Highway visual | 2642 | 2028 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 327 | g11021_highway.079 | g11021_highway | WMB | Highway visual | 106 | 78 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 328 | g11021_highway.080 | g11021_highway | WMB | Highway visual | 20 | 10 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 329 | g11021_highway.081 | g11021_highway | WMB | Highway visual | 1033 | 823 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 330 | g11021_highway.082 | g11021_highway | WMB | Highway visual | 6232 | 4572 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 331 | g11021_highway.083 | g11021_highway | WMB | Highway visual | 6090 | 4536 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 332 | g11021_highway.084 | g11021_highway | WMB | Highway visual | 2463 | 1818 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 333 | g11021_highway.085 | g11021_highway | WMB | Highway visual | 1947 | 1519 | os09_buildingtank01_aaax_tga5 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 334 | g11021_highway.086 | g11021_highway | WMB | Highway visual | 401 | 246 | os10_blocks_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 335 | g11021_highway.087 | g11021_highway | WMB | Highway visual | 154 | 89 | os10_blocks_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 336 | g11021_highway.088 | g11021_highway | WMB | Highway visual | 161 | 95 | os10_blocks_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 337 | g11021_highway.089 | g11021_highway | WMB | Highway visual | 658 | 369 | os10_blocks_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 338 | g11021_highway.090 | g11021_highway | WMB | Highway visual | 808 | 464 | os10_blocks_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 339 | g11021_highway.091 | g11021_highway | WMB | Highway visual | 260 | 151 | os10_blocks_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 340 | g11021_highway.092 | g11021_highway | WMB | Highway visual | 10 | 4 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 341 | g11021_highway.093 | g11021_highway | WMB | Highway visual | 113 | 109 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 342 | g11021_highway.094 | g11021_highway | WMB | Highway visual | 178 | 175 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 343 | g11021_highway.095 | g11021_highway | WMB | Highway visual | 18 | 16 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 344 | g11021_highway.096 | g11021_highway | WMB | Highway visual | 185 | 175 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 345 | g11021_highway.097 | g11021_highway | WMB | Highway visual | 9 | 8 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 346 | g11021_highway.098 | g11021_highway | WMB | Highway visual | 69 | 70 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 347 | g11021_highway.099 | g11021_highway | WMB | Highway visual | 33 | 28 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 348 | g11021_highway.100 | g11021_highway | WMB | Highway visual | 27 | 24 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 349 | g11021_highway.101 | g11021_highway | WMB | Highway visual | 80 | 69 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 350 | g11021_highway.102 | g11021_highway | WMB | Highway visual | 132 | 118 | yam_build03_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 351 | g11021_highway.103 | g11021_highway | WMB | Highway visual | 77 | 61 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 352 | g11021_highway.104 | g11021_highway | WMB | Highway visual | 91 | 75 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 353 | g11021_highway.105 | g11021_highway | WMB | Highway visual | 601 | 538 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 354 | g11021_highway.106 | g11021_highway | WMB | Highway visual | 84 | 70 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 355 | g11021_highway.107 | g11021_highway | WMB | Highway visual | 580 | 513 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 356 | g11021_highway.108 | g11021_highway | WMB | Highway visual | 290 | 248 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 357 | g11021_highway.109 | g11021_highway | WMB | Highway visual | 273 | 239 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 358 | g11021_highway.110 | g11021_highway | WMB | Highway visual | 284 | 238 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 359 | g11021_highway.111 | g11021_highway | WMB | Highway visual | 301 | 253 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 360 | g11021_highway.112 | g11021_highway | WMB | Highway visual | 192 | 164 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 361 | g11021_highway.113 | g11021_highway | WMB | Highway visual | 377 | 332 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 362 | g11021_highway.114 | g11021_highway | WMB | Highway visual | 479 | 421 | yam_build06_aaa9 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 363 | g11021_highway.115 | g11021_highway | WMB | Highway visual | 969 | 810 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 364 | g11021_highway.116 | g11021_highway | WMB | Highway visual | 236 | 244 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 365 | g11021_highway.117 | g11021_highway | WMB | Highway visual | 528 | 466 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 366 | g11021_highway.118 | g11021_highway | WMB | Highway visual | 309 | 293 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 367 | g11021_highway.119 | g11021_highway | WMB | Highway visual | 607 | 567 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 368 | g11021_highway.120 | g11021_highway | WMB | Highway visual | 363 | 306 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 369 | g11021_highway.121 | g11021_highway | WMB | Highway visual | 601 | 543 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 370 | g11021_highway.122 | g11021_highway | WMB | Highway visual | 601 | 615 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 371 | g11021_highway.123 | g11021_highway | WMB | Highway visual | 837 | 794 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 372 | g11021_highway.124 | g11021_highway | WMB | Highway visual | 111 | 111 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 373 | g11021_highway.125 | g11021_highway | WMB | Highway visual | 12 | 6 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 374 | g11021_highway.126 | g11021_highway | WMB | Highway visual | 2553 | 2329 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 375 | g11021_highway.127 | g11021_highway | WMB | Highway visual | 662 | 406 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 376 | g11021_highway.128 | g11021_highway | WMB | Highway visual | 1176 | 932 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 377 | g11021_highway.129 | g11021_highway | WMB | Highway visual | 944 | 820 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 378 | g11021_highway.130 | g11021_highway | WMB | Highway visual | 221 | 149 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 379 | g11021_highway.131 | g11021_highway | WMB | Highway visual | 2365 | 2157 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 380 | g11021_highway.132 | g11021_highway | WMB | Highway visual | 136 | 92 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 381 | g11021_highway.133 | g11021_highway | WMB | Highway visual | 444 | 425 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 382 | g11021_highway.134 | g11021_highway | WMB | Highway visual | 1884 | 1575 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 383 | g11021_highway.135 | g11021_highway | WMB | Highway visual | 1319 | 1132 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 384 | g11021_highway.136 | g11021_highway | WMB | Highway visual | 2321 | 1882 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 385 | g11021_highway.137 | g11021_highway | WMB | Highway visual | 80 | 56 | yo_highway_misc01 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 386 | g11021_highway.138 | g11021_highway | WMB | Highway visual | 1173 | 1086 | yo_road1_damage_a_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 387 | g11021_highway.139 | g11021_highway | WMB | Highway visual | 525 | 474 | yo_road1_damage_a_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 388 | g11021_highway.140 | g11021_highway | WMB | Highway visual | 659 | 612 | yo_road1_damage_a_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 389 | g11021_highway.141 | g11021_highway | WMB | Highway visual | 2845 | 2748 | yo_road1_damage_a_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 390 | g11021_highway.142 | g11021_highway | WMB | Highway visual | 2254 | 2226 | yo_road1_damage_a_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 391 | g11021_highway.143 | g11021_highway | WMB | Highway visual | 571 | 522 | yo_road1_damage_a_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 392 | g11021_highway.144 | g11021_highway | WMB | Highway visual | 902 | 768 | yo_road1_damage_b_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 393 | g11021_highway.145 | g11021_highway | WMB | Highway visual | 407 | 339 | yo_road1_damage_b_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 394 | g11021_highway.146 | g11021_highway | WMB | Highway visual | 519 | 429 | yo_road1_damage_b_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 395 | g11021_highway.147 | g11021_highway | WMB | Highway visual | 1670 | 1408 | yo_road1_damage_b_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 396 | g11021_highway.148 | g11021_highway | WMB | Highway visual | 1071 | 890 | yo_road1_damage_b_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 397 | g11021_highway.149 | g11021_highway | WMB | Highway visual | 442 | 362 | yo_road1_damage_b_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 398 | ground | ground | WMB | Ground visual | 63 | 84 | kaj_ground_weed_01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 399 | ground.001 | ground | WMB | Ground visual | 14 | 12 | kaj_ground_weed_01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 400 | ground.002 | ground | WMB | Ground visual | 30 | 40 | kaj_ground_weed_01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 401 | ground.003 | ground | WMB | Ground visual | 170 | 276 | kaj_ground_weed_01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 402 | ground.004 | ground | WMB | Ground visual | 279 | 486 | kaj_ground_weed_01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 403 | ground.005 | ground | WMB | Ground visual | 102 | 147 | kaj_ground_weed_01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 404 | ground.006 | ground | WMB | Ground visual | 244 | 194 | kaj_tree_leaf_aaa1_1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 405 | ground.007 | ground | WMB | Ground visual | 1056 | 816 | kaj_tree_leaf_aaa1_1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 406 | ground.008 | ground | WMB | Ground visual | 3 | 1 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 407 | ground.009 | ground | WMB | Ground visual | 4 | 2 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 408 | ground.010 | ground | WMB | Ground visual | 3 | 1 | mas_breakconcrete02_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 409 | ground.011 | ground | WMB | Ground visual | 32 | 28 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 410 | ground.012 | ground | WMB | Ground visual | 23 | 28 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 411 | ground.013 | ground | WMB | Ground visual | 75 | 97 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 412 | ground.014 | ground | WMB | Ground visual | 13 | 11 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 413 | ground.015 | ground | WMB | Ground visual | 40 | 48 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 414 | ground.016 | ground | WMB | Ground visual | 41 | 56 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 415 | ground.017 | ground | WMB | Ground visual | 22 | 20 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 416 | ground.018 | ground | WMB | Ground visual | 60 | 56 | mas_concretewall01_aaax_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 417 | ground.019 | ground | WMB | Ground visual | 12 | 12 | mas_concretewall01_aaax_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 418 | ground.020 | ground | WMB | Ground visual | 6 | 4 | mas_concretewall01_aaax_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 419 | ground.021 | ground | WMB | Ground visual | 59 | 86 | mas_concretewall01_aaax_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 420 | ground.022 | ground | WMB | Ground visual | 21 | 20 | mas_concretewall01_aaax_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 421 | ground.023 | ground | WMB | Ground visual | 4 | 2 | mas_concretewall01_aaax_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 422 | ground.024 | ground | WMB | Ground visual | 7 | 4 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 423 | ground.025 | ground | WMB | Ground visual | 37 | 17 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 424 | ground.026 | ground | WMB | Ground visual | 33 | 15 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 425 | ground.027 | ground | WMB | Ground visual | 17 | 8 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 426 | ground.028 | ground | WMB | Ground visual | 30 | 16 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 427 | ground.029 | ground | WMB | Ground visual | 8 | 6 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 428 | ground.030 | ground | WMB | Ground visual | 25 | 18 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 429 | ground.031 | ground | WMB | Ground visual | 20 | 10 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 430 | ground.032 | ground | WMB | Ground visual | 88 | 48 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 431 | ground.033 | ground | WMB | Ground visual | 19 | 8 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 432 | ground.034 | ground | WMB | Ground visual | 77 | 66 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 433 | ground.035 | ground | WMB | Ground visual | 22 | 16 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 434 | ground.036 | ground | WMB | Ground visual | 54 | 30 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 435 | ground.037 | ground | WMB | Ground visual | 20 | 10 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 436 | ground.038 | ground | WMB | Ground visual | 6 | 2 | mas_concretewall01_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 437 | ground.039 | ground | WMB | Ground visual | 141 | 76 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 438 | ground.040 | ground | WMB | Ground visual | 131 | 79 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 439 | ground.041 | ground | WMB | Ground visual | 38 | 16 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 440 | ground.042 | ground | WMB | Ground visual | 313 | 180 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 441 | ground.043 | ground | WMB | Ground visual | 40 | 24 | yo_asphalt_01_aaa3 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 442 | ground.044 | ground | WMB | Ground visual | 875 | 409 | yo_asphalt_01_aaa4 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 443 | ground.045 | ground | WMB | Ground visual | 119 | 151 | yo_asphalt_01_aaa4 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 444 | ground.046 | ground | WMB | Ground visual | 77 | 96 | yo_asphalt_01_aaa4 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 445 | ground.047 | ground | WMB | Ground visual | 45 | 47 | yo_asphalt_01_aaa4 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 446 | ground.048 | ground | WMB | Ground visual | 3 | 1 | yo_road1_texa_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 447 | ground.049 | ground | WMB | Ground visual | 3 | 1 | yo_road1_texa_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 448 | ground.050 | ground | WMB | Ground visual | 29 | 14 | yo_road1_texa_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 449 | ground.051 | ground | WMB | Ground visual | 107 | 52 | yo_road1_texa_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 450 | ground.052 | ground | WMB | Ground visual | 72 | 34 | yo_road1_texa_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 451 | ground.053 | ground | WMB | Ground visual | 79 | 38 | yo_road1_texa_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 452 | ground.054 | ground | WMB | Ground visual | 241 | 120 | yo_road1_texa_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 453 | ground.055 | ground | WMB | Ground visual | 20 | 10 | yo_road1_texa_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 454 | ground.056 | ground | WMB | Ground visual | 12 | 6 | yo_road1_texa_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 455 | ground.057 | ground | WMB | Ground visual | 14 | 8 | yo_road1_texb_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 456 | ground.058 | ground | WMB | Ground visual | 1546 | 619 | yo_road1_texb_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 457 | ground.059 | ground | WMB | Ground visual | 600 | 325 | yo_road1_texb_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 458 | ground.060 | ground | WMB | Ground visual | 153 | 84 | yo_road1_texb_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 459 | ground.061 | ground | WMB | Ground visual | 526 | 288 | yo_road1_texb_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 460 | ground.062 | ground | WMB | Ground visual | 644 | 328 | yo_road1_texb_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 461 | ground.063 | ground | WMB | Ground visual | 109 | 52 | yo_road1_texb_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 462 | ground.064 | ground | WMB | Ground visual | 343 | 144 | yo_road1_texc_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 463 | ground.065 | ground | WMB | Ground visual | 60 | 48 | yo_road1_texc_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 464 | ground.066 | ground | WMB | Ground visual | 59 | 48 | yo_road1_texc_aaax | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 465 | ground.067 | ground | WMB | Ground visual | 4 | 2 | yo_road1_texd_aaa1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 466 | ground.068 | ground | WMB | Ground visual | 91 | 43 | yo_road1_texd_aaa1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 467 | ground.069 | ground | WMB | Ground visual | 87 | 45 | yo_road1_texd_aaa1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 468 | ground.070 | ground | WMB | Ground visual | 33 | 15 | yo_road1_texd_aaa1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 469 | ground.071 | ground | WMB | Ground visual | 184 | 92 | yo_road1_texd_aaa1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 470 | ground.072 | ground | WMB | Ground visual | 44 | 22 | yo_road1_texd_aaa1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 471 | HexGround | HexGround | COL | Static collision batch | 18 | 18 | - | - | - |
| 472 | HexGround.001 | HexGround | COL | Static collision batch | 43 | 40 | - | - | - |
| 473 | HexGround.002 | HexGround | COL | Static collision batch | 48 | 45 | - | - | - |
| 474 | HexGround.003 | HexGround | COL | Static collision batch | 54 | 47 | - | - | - |
| 475 | HexGround.004 | HexGround | COL | Static collision batch | 18 | 18 | - | - | - |
| 476 | HexGround.005 | HexGround | COL | Static collision batch | 13 | 7 | - | - | - |
| 477 | HexGround.006 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 478 | HexGround.007 | HexGround | COL | Static collision batch | 59 | 72 | - | - | - |
| 479 | HexGround.008 | HexGround | COL | Static collision batch | 55 | 66 | - | - | - |
| 480 | HexGround.009 | HexGround | COL | Static collision batch | 49 | 46 | - | - | - |
| 481 | HexGround.010 | HexGround | COL | Static collision batch | 75 | 79 | - | - | - |
| 482 | HexGround.011 | HexGround | COL | Static collision batch | 51 | 49 | - | - | - |
| 483 | HexGround.012 | HexGround | COL | Static collision batch | 68 | 74 | - | - | - |
| 484 | HexGround.013 | HexGround | COL | Static collision batch | 51 | 49 | - | - | - |
| 485 | HexGround.014 | HexGround | COL | Static collision batch | 21 | 15 | - | - | - |
| 486 | HexGround.015 | HexGround | COL | Static collision batch | 46 | 49 | - | - | - |
| 487 | HexGround.016 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 488 | HexGround.017 | HexGround | COL | Static collision batch | 61 | 79 | - | - | - |
| 489 | HexGround.018 | HexGround | COL | Static collision batch | 33 | 35 | - | - | - |
| 490 | HexGround.019 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 491 | HexGround.020 | HexGround | COL | Static collision batch | 15 | 7 | - | - | - |
| 492 | HexGround.021 | HexGround | COL | Static collision batch | 9 | 8 | - | - | - |
| 493 | HexGround.022 | HexGround | COL | Static collision batch | 12 | 8 | - | - | - |
| 494 | HexGround.023 | HexGround | COL | Static collision batch | 7 | 3 | - | - | - |
| 495 | HexGround.024 | HexGround | COL | Static collision batch | 51 | 58 | - | - | - |
| 496 | HexGround.025 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 497 | HexGround.026 | HexGround | COL | Static collision batch | 5 | 3 | - | - | - |
| 498 | HexGround.027 | HexGround | COL | Static collision batch | 77 | 91 | - | - | - |
| 499 | HexGround.028 | HexGround | COL | Static collision batch | 64 | 63 | - | - | - |
| 500 | HexGround.029 | HexGround | COL | Static collision batch | 36 | 34 | - | - | - |
| 501 | HexGround.030 | HexGround | COL | Static collision batch | 56 | 54 | - | - | - |
| 502 | HexGround.031 | HexGround | COL | Static collision batch | 81 | 94 | - | - | - |
| 503 | HexGround.032 | HexGround | COL | Static collision batch | 35 | 35 | - | - | - |
| 504 | HexGround.033 | HexGround | COL | Static collision batch | 63 | 54 | - | - | - |
| 505 | HexGround.034 | HexGround | COL | Static collision batch | 52 | 52 | - | - | - |
| 506 | HexGround.035 | HexGround | COL | Static collision batch | 46 | 58 | - | - | - |
| 507 | HexGround.036 | HexGround | COL | Static collision batch | 58 | 65 | - | - | - |
| 508 | HexGround.037 | HexGround | COL | Static collision batch | 61 | 75 | - | - | - |
| 509 | HexGround.038 | HexGround | COL | Static collision batch | 48 | 54 | - | - | - |
| 510 | HexGround.039 | HexGround | COL | Static collision batch | 56 | 70 | - | - | - |
| 511 | HexGround.040 | HexGround | COL | Static collision batch | 56 | 63 | - | - | - |
| 512 | HexGround.041 | HexGround | COL | Static collision batch | 47 | 43 | - | - | - |
| 513 | HexGround.042 | HexGround | COL | Static collision batch | 89 | 110 | - | - | - |
| 514 | HexGround.043 | HexGround | COL | Static collision batch | 46 | 40 | - | - | - |
| 515 | HexGround.044 | HexGround | COL | Static collision batch | 61 | 62 | - | - | - |
| 516 | HexGround.045 | HexGround | COL | Static collision batch | 11 | 5 | - | - | - |
| 517 | HexGround.046 | HexGround | COL | Static collision batch | 47 | 49 | - | - | - |
| 518 | HexGround.047 | HexGround | COL | Static collision batch | 51 | 48 | - | - | - |
| 519 | HexGround.048 | HexGround | COL | Static collision batch | 43 | 36 | - | - | - |
| 520 | HexGround.049 | HexGround | COL | Static collision batch | 66 | 62 | - | - | - |
| 521 | HexGround.050 | HexGround | COL | Static collision batch | 49 | 56 | - | - | - |
| 522 | HexGround.051 | HexGround | COL | Static collision batch | 62 | 81 | - | - | - |
| 523 | HexGround.052 | HexGround | COL | Static collision batch | 48 | 42 | - | - | - |
| 524 | HexGround.053 | HexGround | COL | Static collision batch | 73 | 73 | - | - | - |
| 525 | HexGround.054 | HexGround | COL | Static collision batch | 41 | 45 | - | - | - |
| 526 | HexGround.055 | HexGround | COL | Static collision batch | 63 | 72 | - | - | - |
| 527 | HexGround.056 | HexGround | COL | Static collision batch | 14 | 11 | - | - | - |
| 528 | HexGround.057 | HexGround | COL | Static collision batch | 70 | 81 | - | - | - |
| 529 | HexGround.058 | HexGround | COL | Static collision batch | 44 | 45 | - | - | - |
| 530 | HexGround.059 | HexGround | COL | Static collision batch | 130 | 139 | - | - | - |
| 531 | HexGround.060 | HexGround | COL | Static collision batch | 122 | 96 | - | - | - |
| 532 | HexGround.061 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 533 | HexGround.062 | HexGround | COL | Static collision batch | 94 | 141 | - | - | - |
| 534 | HexGround.063 | HexGround | COL | Static collision batch | 85 | 82 | - | - | - |
| 535 | HexGround.064 | HexGround | COL | Static collision batch | 54 | 76 | - | - | - |
| 536 | HexGround.065 | HexGround | COL | Static collision batch | 74 | 75 | - | - | - |
| 537 | HexGround.066 | HexGround | COL | Static collision batch | 52 | 52 | - | - | - |
| 538 | HexGround.067 | HexGround | COL | Static collision batch | 60 | 58 | - | - | - |
| 539 | HexGround.068 | HexGround | COL | Static collision batch | 64 | 55 | - | - | - |
| 540 | HexGround.069 | HexGround | COL | Static collision batch | 27 | 34 | - | - | - |
| 541 | HexGround.070 | HexGround | COL | Static collision batch | 37 | 34 | - | - | - |
| 542 | HexGround.071 | HexGround | COL | Static collision batch | 62 | 63 | - | - | - |
| 543 | HexGround.072 | HexGround | COL | Static collision batch | 52 | 81 | - | - | - |
| 544 | HexGround.073 | HexGround | COL | Static collision batch | 33 | 42 | - | - | - |
| 545 | HexGround.074 | HexGround | COL | Static collision batch | 47 | 64 | - | - | - |
| 546 | HexGround.075 | HexGround | COL | Static collision batch | 45 | 59 | - | - | - |
| 547 | HexGround.076 | HexGround | COL | Static collision batch | 29 | 32 | - | - | - |
| 548 | HexGround.077 | HexGround | COL | Static collision batch | 52 | 44 | - | - | - |
| 549 | HexGround.078 | HexGround | COL | Static collision batch | 32 | 32 | - | - | - |
| 550 | HexGround.079 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 551 | HexGround.080 | HexGround | COL | Static collision batch | 88 | 81 | - | - | - |
| 552 | HexGround.081 | HexGround | COL | Static collision batch | 189 | 182 | - | - | - |
| 553 | HexGround.082 | HexGround | COL | Static collision batch | 116 | 139 | - | - | - |
| 554 | HexGround.083 | HexGround | COL | Static collision batch | 57 | 72 | - | - | - |
| 555 | HexGround.084 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 556 | HexGround.085 | HexGround | COL | Static collision batch | 25 | 17 | - | - | - |
| 557 | HexGround.086 | HexGround | COL | Static collision batch | 6 | 5 | - | - | - |
| 558 | HexGround.087 | HexGround | COL | Static collision batch | 12 | 9 | - | - | - |
| 559 | HexGround.088 | HexGround | COL | Static collision batch | 50 | 67 | - | - | - |
| 560 | HexGround.089 | HexGround | COL | Static collision batch | 36 | 42 | - | - | - |
| 561 | HexGround.090 | HexGround | COL | Static collision batch | 97 | 101 | - | - | - |
| 562 | HexGround.091 | HexGround | COL | Static collision batch | 41 | 58 | - | - | - |
| 563 | HexGround.092 | HexGround | COL | Static collision batch | 67 | 86 | - | - | - |
| 564 | HexGround.093 | HexGround | COL | Static collision batch | 66 | 86 | - | - | - |
| 565 | HexGround.094 | HexGround | COL | Static collision batch | 49 | 63 | - | - | - |
| 566 | HexGround.095 | HexGround | COL | Static collision batch | 70 | 75 | - | - | - |
| 567 | HexGround.096 | HexGround | COL | Static collision batch | 58 | 61 | - | - | - |
| 568 | HexGround.097 | HexGround | COL | Static collision batch | 73 | 90 | - | - | - |
| 569 | HexGround.098 | HexGround | COL | Static collision batch | 56 | 69 | - | - | - |
| 570 | HexGround.099 | HexGround | COL | Static collision batch | 81 | 93 | - | - | - |
| 571 | HexGround.100 | HexGround | COL | Static collision batch | 71 | 97 | - | - | - |
| 572 | HexGround.101 | HexGround | COL | Static collision batch | 37 | 54 | - | - | - |
| 573 | HexGround.102 | HexGround | COL | Static collision batch | 32 | 43 | - | - | - |
| 574 | HexGround.103 | HexGround | COL | Static collision batch | 57 | 53 | - | - | - |
| 575 | HexGround.104 | HexGround | COL | Static collision batch | 27 | 31 | - | - | - |
| 576 | HexGround.105 | HexGround | COL | Static collision batch | 91 | 109 | - | - | - |
| 577 | HexGround.106 | HexGround | COL | Static collision batch | 64 | 75 | - | - | - |
| 578 | HexGround.107 | HexGround | COL | Static collision batch | 74 | 96 | - | - | - |
| 579 | HexGround.108 | HexGround | COL | Static collision batch | 69 | 83 | - | - | - |
| 580 | HexGround.109 | HexGround | COL | Static collision batch | 72 | 71 | - | - | - |
| 581 | HexGround.110 | HexGround | COL | Static collision batch | 90 | 58 | - | - | - |
| 582 | HexGround.111 | HexGround | COL | Static collision batch | 73 | 65 | - | - | - |
| 583 | HexGround.112 | HexGround | COL | Static collision batch | 93 | 76 | - | - | - |
| 584 | HexGround.113 | HexGround | COL | Static collision batch | 71 | 64 | - | - | - |
| 585 | HexGround.114 | HexGround | COL | Static collision batch | 92 | 71 | - | - | - |
| 586 | HexGround.115 | HexGround | COL | Static collision batch | 80 | 69 | - | - | - |
| 587 | HexGround.116 | HexGround | COL | Static collision batch | 56 | 73 | - | - | - |
| 588 | HexGround.117 | HexGround | COL | Static collision batch | 41 | 46 | - | - | - |
| 589 | HexGround.118 | HexGround | COL | Static collision batch | 105 | 85 | - | - | - |
| 590 | HexGround.119 | HexGround | COL | Static collision batch | 78 | 72 | - | - | - |
| 591 | HexGround.120 | HexGround | COL | Static collision batch | 85 | 91 | - | - | - |
| 592 | HexGround.121 | HexGround | COL | Static collision batch | 91 | 91 | - | - | - |
| 593 | HexGround.122 | HexGround | COL | Static collision batch | 64 | 73 | - | - | - |
| 594 | HexGround.123 | HexGround | COL | Static collision batch | 107 | 98 | - | - | - |
| 595 | HexGround.124 | HexGround | COL | Static collision batch | 49 | 61 | - | - | - |
| 596 | HexGround.125 | HexGround | COL | Static collision batch | 68 | 85 | - | - | - |
| 597 | HexGround.126 | HexGround | COL | Static collision batch | 52 | 76 | - | - | - |
| 598 | HexGround.127 | HexGround | COL | Static collision batch | 51 | 62 | - | - | - |
| 599 | HexGround.128 | HexGround | COL | Static collision batch | 81 | 91 | - | - | - |
| 600 | HexGround.129 | HexGround | COL | Static collision batch | 80 | 78 | - | - | - |
| 601 | HexGround.130 | HexGround | COL | Static collision batch | 9 | 7 | - | - | - |
| 602 | HexGround.131 | HexGround | COL | Static collision batch | 7 | 5 | - | - | - |
| 603 | HexGround.132 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 604 | HexGround.133 | HexGround | COL | Static collision batch | 5 | 3 | - | - | - |
| 605 | HexGround.134 | HexGround | COL | Static collision batch | 31 | 27 | - | - | - |
| 606 | HexGround.135 | HexGround | COL | Static collision batch | 5 | 3 | - | - | - |
| 607 | HexGround.136 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 608 | HexGround.137 | HexGround | COL | Static collision batch | 12 | 10 | - | - | - |
| 609 | HexGround.138 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 610 | HexGround.139 | HexGround | COL | Static collision batch | 8 | 6 | - | - | - |
| 611 | HexGround.140 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 612 | HexGround.141 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 613 | HexGround.142 | HexGround | COL | Static collision batch | 48 | 44 | - | - | - |
| 614 | HexGround.143 | HexGround | COL | Static collision batch | 76 | 98 | - | - | - |
| 615 | HexGround.144 | HexGround | COL | Static collision batch | 48 | 52 | - | - | - |
| 616 | HexGround.145 | HexGround | COL | Static collision batch | 31 | 37 | - | - | - |
| 617 | HexGround.146 | HexGround | COL | Static collision batch | 34 | 47 | - | - | - |
| 618 | HexGround.147 | HexGround | COL | Static collision batch | 41 | 52 | - | - | - |
| 619 | HexGround.148 | HexGround | COL | Static collision batch | 43 | 63 | - | - | - |
| 620 | HexGround.149 | HexGround | COL | Static collision batch | 10 | 10 | - | - | - |
| 621 | HexGround.150 | HexGround | COL | Static collision batch | 44 | 57 | - | - | - |
| 622 | HexGround.151 | HexGround | COL | Static collision batch | 19 | 20 | - | - | - |
| 623 | HexGround.152 | HexGround | COL | Static collision batch | 24 | 25 | - | - | - |
| 624 | HexGround.153 | HexGround | COL | Static collision batch | 35 | 46 | - | - | - |
| 625 | HexGround.154 | HexGround | COL | Static collision batch | 14 | 16 | - | - | - |
| 626 | HexGround.155 | HexGround | COL | Static collision batch | 53 | 80 | - | - | - |
| 627 | HexGround.156 | HexGround | COL | Static collision batch | 43 | 56 | - | - | - |
| 628 | HexGround.157 | HexGround | COL | Static collision batch | 15 | 16 | - | - | - |
| 629 | HexGround.158 | HexGround | COL | Static collision batch | 16 | 18 | - | - | - |
| 630 | HexGround.159 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 631 | HexGround.160 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 632 | HexGround.161 | HexGround | COL | Static collision batch | 7 | 5 | - | - | - |
| 633 | HexGround.162 | HexGround | COL | Static collision batch | 7 | 6 | - | - | - |
| 634 | HexGround.163 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 635 | HexGround.164 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 636 | HexGround.165 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 637 | HexGround.166 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 638 | HexGround.167 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 639 | HexGround.168 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 640 | HexGround.169 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 641 | HexGround.170 | HexGround | COL | Static collision batch | 8 | 6 | - | - | - |
| 642 | HexGround.171 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 643 | HexGround.172 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 644 | HexGround.173 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 645 | HexGround.174 | HexGround | COL | Static collision batch | 21 | 20 | - | - | - |
| 646 | HexGround.175 | HexGround | COL | Static collision batch | 9 | 5 | - | - | - |
| 647 | HexGround.176 | HexGround | COL | Static collision batch | 5 | 3 | - | - | - |
| 648 | HexGround.177 | HexGround | COL | Static collision batch | 7 | 3 | - | - | - |
| 649 | HexGround.178 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 650 | HexGround.179 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 651 | HexGround.180 | HexGround | COL | Static collision batch | 15 | 9 | - | - | - |
| 652 | HexGround.181 | HexGround | COL | Static collision batch | 5 | 2 | - | - | - |
| 653 | HexGround.182 | HexGround | COL | Static collision batch | 13 | 8 | - | - | - |
| 654 | HexGround.183 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 655 | HexGround.184 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 656 | HexGround.185 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 657 | HexGround.186 | HexGround | COL | Static collision batch | 12 | 10 | - | - | - |
| 658 | HexGround.187 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 659 | HexGround.188 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 660 | HexGround.189 | HexGround | COL | Static collision batch | 5 | 2 | - | - | - |
| 661 | HexGround.190 | HexGround | COL | Static collision batch | 12 | 8 | - | - | - |
| 662 | HexGround.191 | HexGround | COL | Static collision batch | 15 | 9 | - | - | - |
| 663 | HexGround.192 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 664 | HexGround.193 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 665 | HexGround.194 | HexGround | COL | Static collision batch | 12 | 8 | - | - | - |
| 666 | HexGround.195 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 667 | HexGround.196 | HexGround | COL | Static collision batch | 12 | 6 | - | - | - |
| 668 | HexGround.197 | HexGround | COL | Static collision batch | 10 | 4 | - | - | - |
| 669 | HexGround.198 | HexGround | COL | Static collision batch | 6 | 3 | - | - | - |
| 670 | HexGround.199 | HexGround | COL | Static collision batch | 5 | 2 | - | - | - |
| 671 | HexGround.200 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 672 | HexGround.201 | HexGround | COL | Static collision batch | 7 | 3 | - | - | - |
| 673 | HexGround.202 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 674 | HexGround.203 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 675 | HexGround.204 | HexGround | COL | Static collision batch | 8 | 5 | - | - | - |
| 676 | HexGround.205 | HexGround | COL | Static collision batch | 6 | 3 | - | - | - |
| 677 | HexGround.206 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 678 | HexGround.207 | HexGround | COL | Static collision batch | 8 | 4 | - | - | - |
| 679 | HexGround.208 | HexGround | COL | Static collision batch | 9 | 4 | - | - | - |
| 680 | HexGround.209 | HexGround | COL | Static collision batch | 8 | 4 | - | - | - |
| 681 | HexGround.210 | HexGround | COL | Static collision batch | 14 | 10 | - | - | - |
| 682 | HexGround.211 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 683 | HexGround.212 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 684 | HexGround.213 | HexGround | COL | Static collision batch | 5 | 2 | - | - | - |
| 685 | HexGround.214 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 686 | HexGround.215 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 687 | HexGround.216 | HexGround | COL | Static collision batch | 8 | 6 | - | - | - |
| 688 | HexGround.217 | HexGround | COL | Static collision batch | 19 | 17 | - | - | - |
| 689 | HexGround.218 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 690 | HexGround.219 | HexGround | COL | Static collision batch | 15 | 8 | - | - | - |
| 691 | HexGround.220 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 692 | HexGround.221 | HexGround | COL | Static collision batch | 5 | 2 | - | - | - |
| 693 | HexGround.222 | HexGround | COL | Static collision batch | 10 | 6 | - | - | - |
| 694 | HexGround.223 | HexGround | COL | Static collision batch | 6 | 2 | - | - | - |
| 695 | HexGround.224 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 696 | HexGround.225 | HexGround | COL | Static collision batch | 11 | 7 | - | - | - |
| 697 | HexGround.226 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 698 | HexGround.227 | HexGround | COL | Static collision batch | 10 | 8 | - | - | - |
| 699 | HexGround.228 | HexGround | COL | Static collision batch | 8 | 6 | - | - | - |
| 700 | HexGround.229 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 701 | HexGround.230 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 702 | HexGround.231 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 703 | HexGround.232 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 704 | HexGround.233 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 705 | HexGround.234 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 706 | HexGround.235 | HexGround | COL | Static collision batch | 10 | 8 | - | - | - |
| 707 | HexGround.236 | HexGround | COL | Static collision batch | 15 | 16 | - | - | - |
| 708 | HexGround.237 | HexGround | COL | Static collision batch | 15 | 16 | - | - | - |
| 709 | HexGround.238 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 710 | HexGround.239 | HexGround | COL | Static collision batch | 12 | 12 | - | - | - |
| 711 | HexGround.240 | HexGround | COL | Static collision batch | 36 | 44 | - | - | - |
| 712 | HexGround.241 | HexGround | COL | Static collision batch | 48 | 52 | - | - | - |
| 713 | HexGround.242 | HexGround | COL | Static collision batch | 36 | 46 | - | - | - |
| 714 | HexGround.243 | HexGround | COL | Static collision batch | 20 | 24 | - | - | - |
| 715 | HexGround.244 | HexGround | COL | Static collision batch | 10 | 8 | - | - | - |
| 716 | HexGround.245 | HexGround | COL | Static collision batch | 10 | 8 | - | - | - |
| 717 | HexGround.246 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 718 | HexGround.247 | HexGround | COL | Static collision batch | 15 | 16 | - | - | - |
| 719 | HexGround.248 | HexGround | COL | Static collision batch | 12 | 12 | - | - | - |
| 720 | HexGround.249 | HexGround | COL | Static collision batch | 10 | 8 | - | - | - |
| 721 | HexGround.250 | HexGround | COL | Static collision batch | 8 | 6 | - | - | - |
| 722 | HexGround.251 | HexGround | COL | Static collision batch | 12 | 12 | - | - | - |
| 723 | HexGround.252 | HexGround | COL | Static collision batch | 8 | 6 | - | - | - |
| 724 | HexGround.253 | HexGround | COL | Static collision batch | 25 | 32 | - | - | - |
| 725 | HexGround.254 | HexGround | COL | Static collision batch | 28 | 36 | - | - | - |
| 726 | HexGround.255 | HexGround | COL | Static collision batch | 36 | 39 | - | - | - |
| 727 | HexGround.256 | HexGround | COL | Static collision batch | 10 | 8 | - | - | - |
| 728 | HexGround.257 | HexGround | COL | Static collision batch | 20 | 24 | - | - | - |
| 729 | HexGround.258 | HexGround | COL | Static collision batch | 8 | 6 | - | - | - |
| 730 | HexGround.259 | HexGround | COL | Static collision batch | 12 | 13 | - | - | - |
| 731 | HexGround.260 | HexGround | COL | Static collision batch | 28 | 36 | - | - | - |
| 732 | HexGround.261 | HexGround | COL | Static collision batch | 20 | 21 | - | - | - |
| 733 | HexGround.262 | HexGround | COL | Static collision batch | 36 | 49 | - | - | - |
| 734 | HexGround.263 | HexGround | COL | Static collision batch | 24 | 28 | - | - | - |
| 735 | HexGround.264 | HexGround | COL | Static collision batch | 21 | 24 | - | - | - |
| 736 | HexGround.265 | HexGround | COL | Static collision batch | 18 | 20 | - | - | - |
| 737 | HexGround.266 | HexGround | COL | Static collision batch | 30 | 40 | - | - | - |
| 738 | HexGround.267 | HexGround | COL | Static collision batch | 60 | 57 | - | - | - |
| 739 | HexGround.268 | HexGround | COL | Static collision batch | 12 | 8 | - | - | - |
| 740 | HexGround.269 | HexGround | COL | Static collision batch | 56 | 56 | - | - | - |
| 741 | HexGround.270 | HexGround | COL | Static collision batch | 88 | 98 | - | - | - |
| 742 | HexGround.271 | HexGround | COL | Static collision batch | 55 | 66 | - | - | - |
| 743 | HexGround.272 | HexGround | COL | Static collision batch | 46 | 57 | - | - | - |
| 744 | HexGround.273 | HexGround | COL | Static collision batch | 53 | 44 | - | - | - |
| 745 | HexGround.274 | HexGround | COL | Static collision batch | 63 | 74 | - | - | - |
| 746 | HexGround.275 | HexGround | COL | Static collision batch | 88 | 93 | - | - | - |
| 747 | HexGround.276 | HexGround | COL | Static collision batch | 96 | 94 | - | - | - |
| 748 | HexGround.277 | HexGround | COL | Static collision batch | 76 | 91 | - | - | - |
| 749 | HexGround.278 | HexGround | COL | Static collision batch | 81 | 83 | - | - | - |
| 750 | HexGround.279 | HexGround | COL | Static collision batch | 58 | 34 | - | - | - |
| 751 | HexGround.280 | HexGround | COL | Static collision batch | 55 | 57 | - | - | - |
| 752 | HexGround.281 | HexGround | COL | Static collision batch | 93 | 81 | - | - | - |
| 753 | HexGround.282 | HexGround | COL | Static collision batch | 86 | 99 | - | - | - |
| 754 | HexGround.283 | HexGround | COL | Static collision batch | 27 | 30 | - | - | - |
| 755 | HexGround.284 | HexGround | COL | Static collision batch | 43 | 61 | - | - | - |
| 756 | HexGround.285 | HexGround | COL | Static collision batch | 90 | 97 | - | - | - |
| 757 | HexGround.286 | HexGround | COL | Static collision batch | 66 | 70 | - | - | - |
| 758 | HexGround.287 | HexGround | COL | Static collision batch | 46 | 45 | - | - | - |
| 759 | HexGround.288 | HexGround | COL | Static collision batch | 71 | 67 | - | - | - |
| 760 | HexGround.289 | HexGround | COL | Static collision batch | 58 | 58 | - | - | - |
| 761 | HexGround.290 | HexGround | COL | Static collision batch | 41 | 50 | - | - | - |
| 762 | HexGround.291 | HexGround | COL | Static collision batch | 107 | 113 | - | - | - |
| 763 | HexGround.292 | HexGround | COL | Static collision batch | 36 | 33 | - | - | - |
| 764 | HexGround.293 | HexGround | COL | Static collision batch | 44 | 39 | - | - | - |
| 765 | HexGround.294 | HexGround | COL | Static collision batch | 63 | 60 | - | - | - |
| 766 | HexGround.295 | HexGround | COL | Static collision batch | 40 | 40 | - | - | - |
| 767 | HexGround.296 | HexGround | COL | Static collision batch | 63 | 62 | - | - | - |
| 768 | HexGround.297 | HexGround | COL | Static collision batch | 64 | 60 | - | - | - |
| 769 | HexGround.298 | HexGround | COL | Static collision batch | 57 | 55 | - | - | - |
| 770 | HexGround.299 | HexGround | COL | Static collision batch | 64 | 66 | - | - | - |
| 771 | HexGround.300 | HexGround | COL | Static collision batch | 9 | 8 | - | - | - |
| 772 | HexGround.301 | HexGround | COL | Static collision batch | 61 | 66 | - | - | - |
| 773 | HexGround.302 | HexGround | COL | Static collision batch | 57 | 57 | - | - | - |
| 774 | HexGround.303 | HexGround | COL | Static collision batch | 54 | 54 | - | - | - |
| 775 | HexGround.304 | HexGround | COL | Static collision batch | 48 | 42 | - | - | - |
| 776 | HexGround.305 | HexGround | COL | Static collision batch | 62 | 63 | - | - | - |
| 777 | HexGround.306 | HexGround | COL | Static collision batch | 48 | 48 | - | - | - |
| 778 | HexGround.307 | HexGround | COL | Static collision batch | 52 | 53 | - | - | - |
| 779 | HexGround.308 | HexGround | COL | Static collision batch | 60 | 67 | - | - | - |
| 780 | HexGround.309 | HexGround | COL | Static collision batch | 35 | 35 | - | - | - |
| 781 | HexGround.310 | HexGround | COL | Static collision batch | 53 | 59 | - | - | - |
| 782 | HexGround.311 | HexGround | COL | Static collision batch | 28 | 33 | - | - | - |
| 783 | HexGround.312 | HexGround | COL | Static collision batch | 25 | 29 | - | - | - |
| 784 | HexGround.313 | HexGround | COL | Static collision batch | 5 | 3 | - | - | - |
| 785 | HexGround.314 | HexGround | COL | Static collision batch | 41 | 50 | - | - | - |
| 786 | HexGround.315 | HexGround | COL | Static collision batch | 37 | 55 | - | - | - |
| 787 | HexGround.316 | HexGround | COL | Static collision batch | 57 | 88 | - | - | - |
| 788 | HexGround.317 | HexGround | COL | Static collision batch | 87 | 130 | - | - | - |
| 789 | HexGround.318 | HexGround | COL | Static collision batch | 59 | 61 | - | - | - |
| 790 | HexGround.319 | HexGround | COL | Static collision batch | 30 | 35 | - | - | - |
| 791 | HexGround.320 | HexGround | COL | Static collision batch | 80 | 125 | - | - | - |
| 792 | HexGround.321 | HexGround | COL | Static collision batch | 52 | 90 | - | - | - |
| 793 | HexGround.322 | HexGround | COL | Static collision batch | 73 | 102 | - | - | - |
| 794 | HexGround.323 | HexGround | COL | Static collision batch | 59 | 69 | - | - | - |
| 795 | HexGround.324 | HexGround | COL | Static collision batch | 53 | 53 | - | - | - |
| 796 | HexGround.325 | HexGround | COL | Static collision batch | 6 | 2 | - | - | - |
| 797 | HexGround.326 | HexGround | COL | Static collision batch | 29 | 31 | - | - | - |
| 798 | HexGround.327 | HexGround | COL | Static collision batch | 20 | 24 | - | - | - |
| 799 | HexGround.328 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 800 | HexGround.329 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 801 | HexGround.330 | HexGround | COL | Static collision batch | 12 | 20 | - | - | - |
| 802 | HexGround.331 | HexGround | COL | Static collision batch | 16 | 16 | - | - | - |
| 803 | HexGround.332 | HexGround | COL | Static collision batch | 16 | 18 | - | - | - |
| 804 | HexGround.333 | HexGround | COL | Static collision batch | 8 | 4 | - | - | - |
| 805 | HexGround.334 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 806 | HexGround.335 | HexGround | COL | Static collision batch | 49 | 77 | - | - | - |
| 807 | HexGround.336 | HexGround | COL | Static collision batch | 34 | 48 | - | - | - |
| 808 | HexGround.337 | HexGround | COL | Static collision batch | 39 | 50 | - | - | - |
| 809 | HexGround.338 | HexGround | COL | Static collision batch | 52 | 51 | - | - | - |
| 810 | HexGround.339 | HexGround | COL | Static collision batch | 55 | 61 | - | - | - |
| 811 | HexGround.340 | HexGround | COL | Static collision batch | 21 | 18 | - | - | - |
| 812 | HexGround.341 | HexGround | COL | Static collision batch | 45 | 63 | - | - | - |
| 813 | HexGround.342 | HexGround | COL | Static collision batch | 24 | 26 | - | - | - |
| 814 | HexGround.343 | HexGround | COL | Static collision batch | 57 | 51 | - | - | - |
| 815 | HexGround.344 | HexGround | COL | Static collision batch | 31 | 41 | - | - | - |
| 816 | HexGround.345 | HexGround | COL | Static collision batch | 35 | 43 | - | - | - |
| 817 | HexGround.346 | HexGround | COL | Static collision batch | 51 | 68 | - | - | - |
| 818 | HexGround.347 | HexGround | COL | Static collision batch | 44 | 51 | - | - | - |
| 819 | HexGround.348 | HexGround | COL | Static collision batch | 55 | 59 | - | - | - |
| 820 | HexGround.349 | HexGround | COL | Static collision batch | 53 | 52 | - | - | - |
| 821 | HexGround.350 | HexGround | COL | Static collision batch | 84 | 110 | - | - | - |
| 822 | HexGround.351 | HexGround | COL | Static collision batch | 5 | 4 | - | - | - |
| 823 | HexGround.352 | HexGround | COL | Static collision batch | 24 | 33 | - | - | - |
| 824 | HexGround.353 | HexGround | COL | Static collision batch | 52 | 62 | - | - | - |
| 825 | HexGround.354 | HexGround | COL | Static collision batch | 49 | 68 | - | - | - |
| 826 | HexGround.355 | HexGround | COL | Static collision batch | 77 | 96 | - | - | - |
| 827 | HexGround.356 | HexGround | COL | Static collision batch | 57 | 77 | - | - | - |
| 828 | HexGround.357 | HexGround | COL | Static collision batch | 49 | 53 | - | - | - |
| 829 | HexGround.358 | HexGround | COL | Static collision batch | 84 | 109 | - | - | - |
| 830 | HexGround.359 | HexGround | COL | Static collision batch | 9 | 8 | - | - | - |
| 831 | HexGround.360 | HexGround | COL | Static collision batch | 50 | 68 | - | - | - |
| 832 | HexGround.361 | HexGround | COL | Static collision batch | 61 | 85 | - | - | - |
| 833 | HexGround.362 | HexGround | COL | Static collision batch | 82 | 144 | - | - | - |
| 834 | HexGround.363 | HexGround | COL | Static collision batch | 79 | 118 | - | - | - |
| 835 | HexGround.364 | HexGround | COL | Static collision batch | 9 | 7 | - | - | - |
| 836 | HexGround.365 | HexGround | COL | Static collision batch | 18 | 16 | - | - | - |
| 837 | HexGround.366 | HexGround | COL | Static collision batch | 15 | 9 | - | - | - |
| 838 | HexGround.367 | HexGround | COL | Static collision batch | 10 | 7 | - | - | - |
| 839 | HexGround.368 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 840 | HexGround.369 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 841 | HexGround.370 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 842 | HexGround.371 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 843 | HexGround.372 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 844 | HexGround.373 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 845 | HexGround.374 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 846 | HexGround.375 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 847 | HexGround.376 | HexGround | COL | Static collision batch | 9 | 5 | - | - | - |
| 848 | HexGround.377 | HexGround | COL | Static collision batch | 12 | 6 | - | - | - |
| 849 | HexGround.378 | HexGround | COL | Static collision batch | 10 | 4 | - | - | - |
| 850 | HexGround.379 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 851 | HexGround.380 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 852 | HexGround.381 | HexGround | COL | Static collision batch | 5 | 2 | - | - | - |
| 853 | HexGround.382 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 854 | HexGround.383 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 855 | HexGround.384 | HexGround | COL | Static collision batch | 7 | 3 | - | - | - |
| 856 | HexGround.385 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 857 | HexGround.386 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 858 | HexGround.387 | HexGround | COL | Static collision batch | 8 | 5 | - | - | - |
| 859 | HexGround.388 | HexGround | COL | Static collision batch | 6 | 3 | - | - | - |
| 860 | HexGround.389 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 861 | HexGround.390 | HexGround | COL | Static collision batch | 9 | 4 | - | - | - |
| 862 | HexGround.391 | HexGround | COL | Static collision batch | 8 | 4 | - | - | - |
| 863 | HexGround.392 | HexGround | COL | Static collision batch | 3 | 1 | - | - | - |
| 864 | HexGround.393 | HexGround | COL | Static collision batch | 13 | 9 | - | - | - |
| 865 | HexGround.394 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 866 | HexGround.395 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 867 | HexGround.396 | HexGround | COL | Static collision batch | 5 | 2 | - | - | - |
| 868 | HexGround.397 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 869 | HexGround.398 | HexGround | COL | Static collision batch | 4 | 2 | - | - | - |
| 870 | HexGround.399 | HexGround | COL | Static collision batch | 6 | 4 | - | - | - |
| 871 | HexGround.400 | HexGround | COL | Static collision batch | 8 | 6 | - | - | - |
| 872 | MtRobot9 | MtRobot9 | WMB | Large prop visual | 154 | 114 | mas_wall01_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 873 | MtRobot9.001 | MtRobot9 | WMB | Large prop visual | 56 | 32 | mas_wall01_AAAx_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 874 | MtRobot9.002 | MtRobot9 | WMB | Large prop visual | 39 | 27 | os09_pipetiling01_aaa1_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 875 | nolowmap_debris | nolowmap_debris | WMB | Debris visual | 9201 | 6492 | bg1006_aaax_tga1_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 876 | nolowmap_debris.001 | nolowmap_debris | WMB | Debris visual | 966 | 663 | bg1006_aaax_tga1_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 877 | nolowmap_debris.002 | nolowmap_debris | WMB | Debris visual | 251 | 93 | debrisset_material_albedo_aaax_tga1_LT_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 878 | nolowmap_debris.003 | nolowmap_debris | WMB | Debris visual | 7086 | 2483 | debrisset_material_albedo_aaax_tga1_LT_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 879 | nolowmap_debris.004 | nolowmap_debris | WMB | Debris visual | 38 | 15 | debrisset_material_albedo_aaax_tga1_LT_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 880 | nolowmap_debris.005 | nolowmap_debris | WMB | Debris visual | 22750 | 7979 | debrisset_material_albedo_aaax_tga1_LT_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 881 | nolowmap_debris.006 | nolowmap_debris | WMB | Debris visual | 5 | 2 | debrisset_material_albedo_aaax_tga1_LT_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 882 | nolowmap_debris.007 | nolowmap_debris | WMB | Debris visual | 19289 | 6759 | debrisset_material_albedo_aaax_tga1_LT_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 883 | nolowmap_debris.008 | nolowmap_debris | WMB | Debris visual | 6514 | 2287 | debrisset_material_albedo_aaax_tga1_LT_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 884 | nolowmap_debris.009 | nolowmap_debris | WMB | Debris visual | 4969 | 1741 | debrisset_material_albedo_aaax_tga1_LT_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 885 | nolowmap_debris.010 | nolowmap_debris | WMB | Debris visual | 844 | 294 | debrisset_material_albedo_aaax_tga1_LT_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 886 | nolowmap_debris.011 | nolowmap_debris | WMB | Debris visual | 5872 | 2072 | debrisset_material_albedo_aaax_tga1_LT_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 887 | nolowmap_debris.012 | nolowmap_debris | WMB | Debris visual | 13905 | 4849 | debrisset_material_albedo_aaax_tga1_LT_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 888 | nolowmap_debris.013 | nolowmap_debris | WMB | Debris visual | 10702 | 3706 | debrisset_material_albedo_aaax_tga1_LT_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 889 | nolowmap_debris.014 | nolowmap_debris | WMB | Debris visual | 554 | 544 | mas_breakconcrete02_AAAx_tga_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 890 | nolowmap_debris.015 | nolowmap_debris | WMB | Debris visual | 882 | 796 | mas_breakconcrete02_AAAx_tga_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 891 | nolowmap_debris.016 | nolowmap_debris | WMB | Debris visual | 150 | 134 | mas_breakconcrete02_AAAx_tga_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 892 | nolowmap_debris.017 | nolowmap_debris | WMB | Debris visual | 118 | 120 | yo_asphalt_01_aaa3_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 893 | nolowmap_debris.018 | nolowmap_debris | WMB | Debris visual | 178 | 178 | yo_asphalt_01_aaa3_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 894 | nolowmap_debris.019 | nolowmap_debris | WMB | Debris visual | 26 | 20 | yo_asphalt_01_aaa3_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 895 | nolowmap_debris.020 | nolowmap_debris | WMB | Debris visual | 676 | 660 | yo_road1_damage_a_aaax_tga1_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 896 | nolowmap_debris.021 | nolowmap_debris | WMB | Debris visual | 1040 | 972 | yo_road1_damage_a_aaax_tga1_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 897 | nolowmap_debris.022 | nolowmap_debris | WMB | Debris visual | 160 | 156 | yo_road1_damage_a_aaax_tga1_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 898 | nolowmap_debris.023 | nolowmap_debris | WMB | Debris visual | 510 | 440 | yo_road1_damage_b_aaa1_tga1_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 899 | nolowmap_debris.024 | nolowmap_debris | WMB | Debris visual | 792 | 648 | yo_road1_damage_b_aaa1_tga1_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 900 | nolowmap_debris.025 | nolowmap_debris | WMB | Debris visual | 126 | 104 | yo_road1_damage_b_aaa1_tga1_LT | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 901 | nolowmap_debris.026 | nolowmap_debris | WMB | Debris visual | 6996 | 3642 | bg1006_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 902 | nolowmap_debris.027 | nolowmap_debris | WMB | Debris visual | 5904 | 3113 | bg1006_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 903 | nolowmap_debris.028 | nolowmap_debris | WMB | Debris visual | 385 | 134 | mas_concretedebris04_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 904 | nolowmap_debris.029 | nolowmap_debris | WMB | Debris visual | 685 | 239 | mas_concretedebris04_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 905 | nolowmap_debris.030 | nolowmap_debris | WMB | Debris visual | 8849 | 3101 | mas_concretedebris04_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 906 | nolowmap_debris.031 | nolowmap_debris | WMB | Debris visual | 2001 | 718 | mas_concretedebris04_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 907 | nolowmap_debris.032 | nolowmap_debris | WMB | Debris visual | 625 | 216 | mas_concretedebris04_aaax_tga1 | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 908 | nolowmap_debris.033 | nolowmap_debris | WMB | Debris visual | 8 | 6 | mas_concretedecal01_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 909 | nolowmap_debris.034 | nolowmap_debris | WMB | Debris visual | 16 | 12 | mas_concretedecal01_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 910 | nolowmap_debris.035 | nolowmap_debris | WMB | Debris visual | 30 | 16 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 911 | nolowmap_debris.036 | nolowmap_debris | WMB | Debris visual | 279 | 194 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 912 | nolowmap_debris.037 | nolowmap_debris | WMB | Debris visual | 14 | 7 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 913 | nolowmap_debris.038 | nolowmap_debris | WMB | Debris visual | 851 | 603 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 914 | nolowmap_debris.039 | nolowmap_debris | WMB | Debris visual | 449 | 309 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |
| 915 | nolowmap_debris.040 | nolowmap_debris | WMB | Debris visual | 196 | 131 | mas_concretefloor03_AAA3_tga | UVMap1;UVMap2;UVMap3;UVMap4 | Col |

## Links

- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/CityOfRuinEntry_월드_임포트_계획]]
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata]]
