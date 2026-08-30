# CityOfRuin Texture 복사 검증

| Resource | DDS | Material hash 미해결 |
|---|---:|---|
| CityOfRuinEntry | 128 | `1FBC0984`, `4E9C16F4`, `7FD4929A` |
| CityOfRuinBridge | 92 | `1FBC0984`, `4E9C16F4`, `7FD4929A` |
| CityOfRuinMainField | 101 | `1FBC0984`, `4E9C16F4`, `6B4C494D`, `7FD4929A` |
| CityOfRuinManHole | 127 | `1FBC0984`, `4E9C16F4`, `5864A24F`, `6B4C494D`, `7FD4929A` |
| CityOfRuinEntry_LOD | 27 | `1FBC0984`, `4E9C16F4`, `7FD4929A` |
| CityOfRuin_LOD | 38 | `1FBC0984`, `4E9C16F4`, `7FD4929A` |

검증 내용:

- 처음 복사한 343 DDS는 각 DTT 추출 폴더와 대상의 filename 및 SHA-256이 전부 일치했다.
- Bridge/MainField/ManHole에는 `materials.json` hash를 기준으로 base `data004/wda/ga0001.dtt`에서 47/55/68 DDS를 추가 복사했다.
- `.blend`가 연결한 image basename은 여섯 resource 모두 대상 `Textures`에서 누락 0개다.
- LOD material custom property의 실제 texture hash는 local DDS와 공통 placeholder 3개로 모두 설명된다.
- `6B4C494D.dds`, `5864A24F.dds`는 `D:\NierRes` 전체 검색 결과가 없다.
