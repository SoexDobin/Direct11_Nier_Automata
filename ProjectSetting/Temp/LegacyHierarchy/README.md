# Legacy Hierarchy Reference

이 폴더의 `LevelData_1.json`, `LevelData_2.json`, `LevelData_3.json`은 과거 `objectID`/`parentObjectID` 기반 Hierarchy 구조를 참고하기 위한 보존 자료다.

- 런타임과 Editor의 Scene load 대상이 아니다.
- 현재 Scene 문서는 `ProjectSetting/Scene/LevelData_<level>.json`의 ObjectGuid 기반 schema만 사용한다.
- 새 Scene 제작이나 migration 입력으로 자동 사용하지 않는다.
