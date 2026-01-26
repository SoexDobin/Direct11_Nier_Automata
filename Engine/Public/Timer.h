#pragma once
#include "Object.h"

NS_BEGIN(Engine)

class ENGINE_DLL Timer final : public Object
{
public:
	Timer();
	~Timer() override = default;

public:
	void Initialize(void* args) override;
	void Update_Timer();

public:
	Float	GetDeltaTime()	const { return m_DeltaTime; }
	Float	GetTotalTime()	const { return m_TotalTime; }
	uint32	GetFPS()		const { return m_FPS; }
	uint32	GetFrameCount() const { return m_FrameCount; }

	Float GetFixedDeltaTime()					const { return m_FixedDeltaTime; }
	Float GetFixedTimeAccumulator()				const { return m_FixedTimeAcc; }
	Bool ShouldFixedUpdate() const;
	void ConsumeFixedDeltaTime();

	void SetFixedDeltaTime(Float fixedDelta)	{ m_FixedDeltaTime = fixedDelta; }
	void SetTimeScale(Float scale)				{ m_TimeScale = scale; }
	Float GetTimeScale()						const { return m_TimeScale; }
	Float GetScaledDeltaTime()					const { return m_DeltaTime * m_TimeScale; }

private:
	LARGE_INTEGER m_Frequency = {};
	LARGE_INTEGER m_PrevTime = {};
	LARGE_INTEGER m_CurrentTime = {};

	Float m_DeltaTime = {};
	Float m_TotalTime = {};
	Float m_TimeScale = {};

	Float m_FixedDeltaTime = {};
	Float m_FixedTimeAcc = {};

	uint32 m_FPS = {};           // 초당 프레임 수
	uint32 m_FrameCount = {};    // 총 프레임 카운트
	uint32 m_FPSCount = {};      // FPS 계산용 임시 카운터
	Float m_FPSTimeElapsed = {}; // FPS 갱신용 경과 시간

public:
	static Shared<Timer> Create();
};

NS_END