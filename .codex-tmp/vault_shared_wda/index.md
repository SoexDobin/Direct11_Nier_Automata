# Project Vault Index

This is the map for the Codex-operated Project Vault.

Start here:

- [[AGENTS]]
- [[log]]

## Workflow

- `AGENTS.md`: active Vault routing, commands, save filter, project hub rule, legacy rule, and document rules
- `log.md`: chronological work log
- [[AI-Sessions/log-archive/2026-07]] — archived Project Vault work log entries from July 2026
- `prompts/`: reusable natural-language commands

## Core Rules

- [[AI-Sessions/wiki/rules/Project_Vault_Global_Project_Routing]] — includes the mandatory Vault discovery bootstrap for the first substantive project request

- [[AI-Sessions/wiki/rules/Project_Vault_구조와_Codex_활용_가이드]]

- [[AI-Sessions/wiki/rules/Project_Vault_Text_Encoding_And_Log_Retention]] — UTF-8 validation and rolling 30-day log retention

- [[AI-Sessions/wiki/rules/Portfolio_Video_Upload_Guidelines]]

## Design

- [[AI-Sessions/wiki/design/Figma_Portfolio_Video_Editing_Frame_Guide]]
- [[AI-Sessions/wiki/design/Portfolio_Video_Editing_Guide.excalidraw]]

## Wiki Areas

- `AI-Sessions/wiki/sources/`: processed source summaries
- `AI-Sessions/wiki/concepts/`: reusable concepts
- `AI-Sessions/wiki/decisions/`: decisions and rationale
- `AI-Sessions/wiki/errors/`: failed approaches and risks
- `AI-Sessions/wiki/projects/`: project hubs and repo links
- `AI-Sessions/wiki/design/`: design and IA rules
- `AI-Sessions/wiki/dev-tasks/`: implementation task notes
- `AI-Sessions/wiki/rules/`: reusable operating rules
- `AI-Sessions/wiki/handoffs/`: handoff notes
- `AI-Sessions/wiki/legacy/`: cross-project legacy summaries and archived handoff bundles

## Projects

- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata]] — 주 소스 저장소, `D:\NierRes` 리소스 작업 공간, 리소스 조사용 NotebookLM 연결
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/errors/bin_리소스와_빌드_산출물_분리]] — `bin` 리소스·셰이더 원본을 보존하면서 재생성 가능한 빌드 산출물만 Git에서 제외하는 규칙
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/dev-tasks/CPK_일괄_해제]] — CPK 24개 일괄 해제 결과, 재개 런처, 하위 변환 경고
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/dev-tasks/SoExDobin_git_history_summary]] — NieR 모작 엔진·클라이언트·에디터·내비게이션 Git 작업 근거
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/RTTR_리플렉션_구조와_한계_분석]] — RTTR 생성·등록·직렬화·Editor 구조와 172파일 SHA-256 기준선 및 한계 분석
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/에디터_리플렉션_프리팹_개편_계획]] — RTTR Registry·Unified ChildGraph·Container 제거, Phase 4 결정적 Child 복원 소스 적용, 리소스 복구 후 Pl0000 GUI 검증을 선행하는 Phase 5 UI 실행·편집 계획, `ProjectSetting/Scene`·`Prefab`·Editor 제작 asset 폴더 규약
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/모델_애니메이션_리소스_파이프라인_계획]] — `.model`·클립별 `.anim` 분리와 animation-only 변환, Camera/Object/Material/Light 상수 버퍼 규약 및 AnimatedMesh 수직 적용, `ModelSettings.xlsx` 직접 입력, Pl0000 실제 리소스·런타임 검증 순서
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/CityOfRuinEntry_월드_임포트_계획]] — 평탄화된 g11021 FBX를 visual, raw COL, raw LAY로 분리하고 material, spatial chunk, Navigation, instance와 wd2 LOD를 기존 엔진 경계에 연결하는 실행 계획
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/design/머티리얼_셰이더_그래프_잠정_방향]] — 전체 `materials.json`과 DDS 분석 전 유지할 공통 surface 출력, 복수 texture binding, Engine node·Client shader·Editor graph의 잠정 설계와 확정 조건
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/rules/기존_구조_우선_설계_규칙]] — 기존 타입·함수 수정/확장을 우선하는 리팩토링 원칙과 근거 없는 class/service/context/key/transaction 확장 억제 규칙
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/sources/CityOfRuinEntry_FBX_메시_인벤토리]] — CityOfRuinEntry FBX 916개 전체 이름과 raw COL 401, raw LAY 398 placements, material 50개를 대조한 원본 근거
- [[AI-Sessions/wiki/projects/Direct11_Nier_Automata/sources/RTTR_라이브러리_개편_참고]] — RTTR 설치 버전·핵심 API·DLL 경계·개편 의사결정과 회귀 테스트 기준
- [[AI-Sessions/wiki/projects/Expedition_158]]
- [[AI-Sessions/wiki/projects/SlimeDicer]] — Java Swing/AWT turn-based dice battle desktop game
- [[AI-Sessions/wiki/projects/Pagomoa]] — Unity 2D 공동 프로젝트의 SoExDobin 작성자 기여 경계와 현재 상태
- [[AI-Sessions/wiki/projects/Pagomoa/sources/기여_및_기술_근거]] — 416개 작성자 커밋과 현행 코드로 검증한 퀘스트·인벤토리·컷신·UI·월드 구현 및 포트폴리오 주장 범위
- [[AI-Sessions/wiki/projects/Over-the-breath]] — `cksuwjr/Over-the-breath` 저장소의 SoExDobin 기여 범위 분석 프로젝트
- [[AI-Sessions/wiki/projects/Career_Preparation|취업 준비]] — 포트폴리오, 지원 서류, 면접, 자료구조·알고리즘·CS 학습 및 문제 풀이를 한곳에서 관리

### 취업 준비

- [[AI-Sessions/wiki/projects/Career_Preparation/취업_준비_현황]]
- [[AI-Sessions/wiki/projects/Career_Preparation/이력서]] — 직무 관련 경험 첫 페이지 작성 원칙과 Expedition 33·NieR 실제 Git 근거 기반 게임 클라이언트 문안
- [[AI-Sessions/wiki/projects/Career_Preparation/보유_기술_원본]] — Expedition 33 실제 구현 근거를 바탕으로 한 이력서 기술 스택 장문 원본
- [[AI-Sessions/wiki/projects/Career_Preparation/노션_프로젝트_정리]] — 노션 한 페이지용 프로젝트 공개 문안; UE5 리타겟과 자체 엔진 통합의 역할 경계 포함
- [[AI-Sessions/wiki/projects/Career_Preparation/Integrated_Tech_Stack_Draft]] — Expedition33·NieR·Pagomoa·SlimeDicer의 실제 코드와 작성 이력에 근거한 입사지원서 기술 스택 통합 원본
- [[AI-Sessions/wiki/projects/Career_Preparation/rules/문서_구성_규칙]]
- [[AI-Sessions/wiki/projects/Career_Preparation/게임사_수학_물리_면접_30제]] — 수학·물리 면접 대비 30제
- [[AI-Sessions/wiki/projects/Career_Preparation/design/포트폴리오_A4_양식_구조도.excalidraw]] — 손글씨 메모 기반 A4 세로형 포트폴리오 영역·키워드 설명 구조도

### Expedition_158

- [[AI-Sessions/wiki/projects/Expedition_158/dev-tasks/SoExDobin_git_history_summary]] — Git 기반 기여 근거, UE5 리타겟 경계, 맵·머티리얼 협업 코드의 통합 범위
- [[AI-Sessions/wiki/projects/Expedition_158/dev-tasks/SoExDobin_portfolio_caption_plan]] — 포트폴리오 자막 구성과 도구·엔진 역할 경계
- [[AI-Sessions/wiki/projects/Expedition_158/dev-tasks/ScreenFx_radial_blur_rgba_debug_plan]] — implemented; checkbox displays a 400x400 overlay in the Scene panel; Debug/Release x64 verified
- [[AI-Sessions/wiki/projects/Expedition_158/dev-tasks/Maelle_back_hair_physics_priority]]
- [[AI-Sessions/wiki/projects/Expedition_158/dev-tasks/Maelle_hair_v143_stability_analysis]]
- [[AI-Sessions/wiki/projects/Expedition_158/design/BattleController_initial_turn_event_UML]]
- [[AI-Sessions/wiki/projects/Expedition_158/handoffs/legacy_maelle_back_hair_log_extract]]

## Dev Tasks

Project-specific dev tasks now live under each project folder. Use `## Projects` for project-owned work and keep this section for cross-project dev tasks only.

## Decisions

No durable decisions have been registered yet.

## Sources

No source summaries have been registered yet.



- [[AI-Sessions/wiki/projects/Expedition_158/design/Battle_Event_State_Flow_Clean.excalidraw]]


