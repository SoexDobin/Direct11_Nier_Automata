# CityOfRuin LOD 분석 및 DTT 준비 목록

## 결론

- `CityOfRuinEntry_LOD.blend`의 실제 WMB는 `g20608`이다.
- `CityOfRuin_LOD.blend`의 실제 WMB는 `g20708`이다.
- 두 파일은 개별 고해상도 WMB의 내부 LOD가 아니라 여러 `wd1` 타일을 합친 `wd2` 지역 저해상도 WMB다.
- 두 파일의 모든 mesh property는 `LOD0`이며 `colTreeNodeIndex=-1`이다. WMB tree, COL, LAY는 없다.
- 엔진에서는 고해상도 파일의 `lod_level`과 섞지 말고 파일 단위 Region LOD로 취급해야 한다.

## LOD 파일 자체

| Resource | WMB/DTT | Source DTT | Companion DAT | Mesh | Vertices | Triangles | Materials | DDS 준비 |
|---|---|---|---|---:|---:|---:|---:|---:|
| `CityOfRuinEntry_LOD` | `g20608` | `data004/wd2/g20608.dtt` | `data003/wd2/g20608.dat` | 63 | 254,929 | 118,079 | 12 | 27 |
| `CityOfRuin_LOD` | `g20708` | `data004/wd2/g20708.dtt` | `data003/wd2/g20708.dat` | 159 | 301,655 | 151,608 | 17 | 38 |

`wd2` shared texture archive는 다음을 사용한다.

- `data003/wda/ga0002.dat`
- `data004/wda/ga0002.dtt`

## g20608이 대표하는 고해상도 DTT

| High DTT | LOD mesh | LOD vertices | LOD triangles | DTT | DAT | 현재 BLEND |
|---|---:|---:|---:|---|---|---|
| `g10821` | 3 | 64,296 | 31,708 | `data004/wd1/g10821.dtt` | `data003/wd1/g10821.dat` | 추가 필요 |
| `g10921` | 2 | 6,860 | 2,695 | `data004/wd1/g10921.dtt` | `data003/wd1/g10921.dat` | 추가 필요 |
| `g10922` | 11 | 45,533 | 21,573 | `data004/wd1/g10922.dtt` | `data003/wd1/g10922.dat` | 추가 필요 |
| `g11021` | 16 | 104,524 | 46,600 | `data004/wd1/g11021.dtt` | `data003/wd1/g11021.dat` | `CityOfRuinEntry.blend` 확보 |
| `g11120` | 15 | 12,700 | 5,842 | `data014/wd1/g11120.dtt` | `data013/wd1/g11120.dat` | `CityOfRuinBridge.blend` 확보 |
| `g11121` | 16 | 21,016 | 9,661 | `data014/wd1/g11121.dtt` | `data013/wd1/g11121.dat` | 추가 필요 |

## g20708이 대표하는 고해상도 DTT

| High DTT | LOD mesh | LOD vertices | LOD triangles | DTT | DAT | 현재 BLEND |
|---|---:|---:|---:|---|---|---|
| `g11219` | 27 | 44,867 | 21,378 | `data014/wd1/g11219.dtt` | `data013/wd1/g11219.dat` | 추가 필요 |
| `g11220` | 15 | 20,660 | 11,741 | `data014/wd1/g11220.dtt` | `data013/wd1/g11220.dat` | `CityOfRuinMainField.blend` 확보 |
| `g11318` | 27 | 63,522 | 30,334 | `data014/wd1/g11318.dtt` | `data013/wd1/g11318.dat` | 추가 필요 |
| `g11319` | 26 | 47,897 | 26,102 | `data014/wd1/g11319.dtt` | `data013/wd1/g11319.dat` | 추가 필요 |
| `g11320` | 23 | 43,980 | 23,175 | `data014/wd1/g11320.dtt` | `data013/wd1/g11320.dat` | 추가 필요 |
| `g11418` | 18 | 35,291 | 17,179 | `data014/wd1/g11418.dtt` | `data013/wd1/g11418.dat` | `CityOfRuinManHole.blend` 확보 |
| `g11419` | 23 | 45,438 | 21,699 | `data014/wd1/g11419.dtt` | `data013/wd1/g11419.dat` | 추가 필요 |

## 추가로 import할 고해상도 DTT 9종

```text
data004/wd1/g10821.dtt + data003/wd1/g10821.dat
data004/wd1/g10921.dtt + data003/wd1/g10921.dat
data004/wd1/g10922.dtt + data003/wd1/g10922.dat
data014/wd1/g11121.dtt + data013/wd1/g11121.dat
data014/wd1/g11219.dtt + data013/wd1/g11219.dat
data014/wd1/g11318.dtt + data013/wd1/g11318.dat
data014/wd1/g11319.dtt + data013/wd1/g11319.dat
data014/wd1/g11320.dtt + data013/wd1/g11320.dat
data014/wd1/g11419.dtt + data013/wd1/g11419.dat
```

`data004/wd1` 3종은 shared texture `data003/wda/ga0001.dat` + `data004/wda/ga0001.dtt`를 사용한다.

`data014/wd1` 6종은 patch-family `data013/wda/ga0000.dat` + `data014/wda/ga0000.dtt`만 보면 부족하다. 현재 확보된 g11120/g11220/g11418 material hash 대조에서 base-family `data004/wda/ga0001.dtt`가 각각 47/55/68개를 추가 resolve했다. 따라서 import 시 local → 같은 family ga0000 → base data004 ga0001 순의 후보 검증이 필요하다.

## Resource texture 복사 결과

| Resource folder | Source | DDS |
|---|---|---:|
| `CityOfRuinEntry/Textures` | `g11021.dtt/textures` | 128 |
| `CityOfRuinBridge/Textures` | `g11120.dtt/textures` 45 + base `ga0001` 47 | 92 |
| `CityOfRuinMainField/Textures` | `g11220.dtt/textures` 46 + base `ga0001` 55 | 101 |
| `CityOfRuinManHole/Textures` | `g11418.dtt/textures` 59 + base `ga0001` 68 | 127 |
| `CityOfRuinEntry_LOD/Textures` | `g20608.dtt/textures` | 27 |
| `CityOfRuin_LOD/Textures` | `g20708.dtt/textures` | 38 |

추출 폴더에 준비된 DDS 전체와 `materials.json`이 실제 요구하며 base `ga0001`에서 resolve되는 DDS를 model별로 분리 복사했다. 서로 다른 model 폴더 사이에서 같은 hash filename이 있어도 합치지 않는다.

끝까지 파일이 없는 hash는 다음과 같다.

- 모든 모델 공통 placeholder: `1FBC0984`, `4E9C16F4`, `7FD4929A`
- MainField 추가 미해결: `6B4C494D`
- ManHole 추가 미해결: `5864A24F`, `6B4C494D`

`6B4C494D`와 `5864A24F`는 `D:\NierRes` 전체에서 DDS 파일을 찾지 못했으므로 복사 대상에서 제외하고 runtime default/원본 archive 추가 조사 대상으로 남긴다.

## Runtime 연결 규칙

1. `g20608`은 `g11021` 단독 LOD가 아니라 `g10821~g11121` 6개 타일의 지역 LOD다.
2. `g20708`은 `g11219~g11419` 7개 타일의 지역 LOD다.
3. high tile 일부만 로드한 상태에서 regional LOD와 동시에 그리면 중복 geometry가 생기므로 region ownership과 visibility 전환을 먼저 정의한다.
4. LOD 파일 내부의 `LOD0` property는 파일 내부 pass일 뿐 high WMB의 LOD0과 같은 계층으로 직접 묶지 않는다.
5. `g20608/g20708`에는 WMB tree가 없으므로 region bounds는 전체 mesh bounds로 생성하거나 대응 high tile bounds의 union을 사용한다.
