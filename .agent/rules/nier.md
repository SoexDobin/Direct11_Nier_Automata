---
trigger: always_on
glob:
description: "NieR Automata DX11 프로젝트 전역 룰 - 아키텍처, 코딩 컨벤션, 활성 스킬"
---

# NieR Automata DirectX 11 Project Rules

## 프로젝트 개요

DirectX 11 기반 니어 오토마타 모작 프로젝트. 학습과 게임 개발을 겸한다.

## 아키텍처

```
Engine (정적 라이브러리)
  ├── public/    → 외부 공개 헤더
  ├── private/   → 내부 구현
  ├── default/   → 기본 리소스
  └── external/  → 외부 라이브러리 (vcpkg 등)
Client (실행 파일)
  └── Engine에 의존
Editor (실행 파일)
  └── Engine + ImGui에 의존
EngineSDK
  └── 엔진 유틸리티/도구
```

**의존성 방향**: Client/Editor → Engine (역방향 금지)

## 코딩 컨벤션 (C++)

### 들여쓰기
- **Tab** 사용 필수 (4칸 탭 기준)
- Space 들여쓰기 금지

### 중괄호 스타일
- **Allman Style** (여는 중괄호 `{`는 항상 새 줄)

```cpp
void Function()
{
	if (condition)
	{
		DoSomething();
	}
}
```

### 포인터/참조자 표기
- `Type *Var`, `Type &Var` 형식 (변수명 앞에 붙임)

```cpp
void Func(const Shared<GameObject> &object);
GameObject *ptr = nullptr;
```

### 명명 규칙
- 클래스: PascalCase (`RenderSystem`, `GameObject`)
- 멤버 변수: m_ 접두사 (`m_Device`, `m_Context`)
- 함수: PascalCase (`Initialize()`, `GetWidth()`)

### DirectX 11 리소스 관리
- `Microsoft::WRL::ComPtr<>` 필수 사용
- Raw 포인터 Release 패턴 지양
- RAII 원칙 준수
- 리소스 해제 순서: 생성 역순

## 활성 스킬 & 사용 가이드

### 자동 트리거 스킬 (항상 활성)

이 스킬들은 관련 키워드나 작업 시 자동으로 활성화됩니다:

#### 리서치 & 문서
- **notebooklm**: 자료 리서치, 코드 래퍼런스, 기술 질문 시
  - 트리거: NotebookLM URL 언급, `/research` 워크플로우

#### C++ & 게임 개발
- **cpp-pro**: C++ 코드 작성/리팩토링 시
  - 트리거: Modern C++, RAII, 스마트 포인터 관련 작업
- **game-development**: 게임 시스템 설계 시
  - 트리거: 게임 루프, FSM, ECS, 컴포넌트 시스템 작업

#### 디버깅 & 아키텍처
- **debugger**: 에러/버그 발생 시
  - 트리거: `/debug` 워크플로우, 에러 메시지 분석
- **architecture**: 아키텍처 결정 시
  - 트리거: 시스템 설계, 의존성 분석, ADR 작성

#### 코드 품질 & 메모리 안전성 (신규 추가)
- **code-review-checklist**: 코드 리뷰 시 체계적 체크리스트 제공
  - 트리거: 코드 리뷰 요청, PR 검토, 품질 검증
  - 용도: 기능, 보안, 성능, 가독성, 테스트 커버리지 검증
- **memory-safety-patterns**: C++ 메모리 안전성 패턴 가이드
  - 트리거: 메모리 관리, DirectX 리소스, ComPtr, Live Object 경고
  - 용도: RAII, 스마트 포인터, 리소스 누수 방지
- **context-window-management**: 대화 컨텍스트 최적화
  - 트리거: 대규모 코드베이스 분석, 긴 에러 로그 처리
  - 용도: 토큰 절약, 중요 정보 우선순위화

### 명시적 호출 방법

필요 시 명시적으로 특정 스킬 호출 가능:
```
Use @[스킬명] [작업 내용]
```

**예시:**
```
Use @code-review-checklist to review Engine/private/RenderSystem.cpp
Use @memory-safety-patterns to analyze memory management in Engine/private/TextureManager.cpp
```


## 활성 NotebookLM 노트북

프로젝트 관련 노트북 2개가 등록되어 있습니다:

### Notebook 1: NieR Automata 모딩 기술 가이드
- **URL**: https://notebooklm.google.com/notebook/88617913-333a-42b4-82bb-6eeb6bac60ea
- **주제**: 니어 오토마타 게임 모딩 제작 매뉴얼
- **내용**:
  - 모델/텍스처 교체 (Blender + NieR2Blender2NieR 플러그인)
  - PBR 렌더링 시스템 (마스크 맵, DDS 포맷)
  - 애니메이션(.mot) 편집
  - 사운드 모딩, FAR 텍스처 주입
- **활용**: 니어 오토마타 원본 리소스 분석 시 참조

### Notebook 2: Game Engine Development (DX11 + C++)
- **URL**: https://notebooklm.google.com/notebook/f60f2104-bc9f-4429-86ce-f646d0c633a7
- **주제**: DirectX 11 게임 엔진 개발 종합 가이드
- **내용**:
  - 렌더링 아키텍처 (Forward, Deferred, Forward+ Tiled)
  - 라이팅 모델 (Phong, Blinn-Phong)
  - 텍스처링 (UV 매핑, mipmapping)
  - ImGui 에디터 개발 (Scene View, ImGuizmo)
  - 엔진 아키텍처 (셰이더 관리, Constant Buffer, DirectXTex)
- **활용**: 엔진 구현 시 기술 레퍼런스

> [!TIP]
> `/research` 워크플로우 사용 시 자동으로 관련 노트북을 선택하여 검색합니다.


## 중요 원칙

1. **모든 답변은 한국어** 기본
2. **파일 수정 전 반드시 사용자 승인** 획득
3. **토큰 최적화**: 핵심 정보 중심 Markdown 처리
4. **능동적 제안**: 답변 마지막에 개선 방안/다음 단계 포함
5. **데이터 보안**: 요청하지 않은 파일 데이터 노출 금지
