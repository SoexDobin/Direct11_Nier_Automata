#pragma once
#include "Object.h"

NS_BEGIN(Engine)

class Timer final
{
	NO_COPY(Timer)
public:
	Timer() = default;
	~Timer() = default;

public:
	Bool IsActive() const { return m_isActive; }
	void SetActive(Bool isActive) { m_isActive = isActive; }

	Float	GetDeltaTime()	const { return m_DeltaTime; }
	Float	GetTotalTime()	const { return m_DeltaAcc; }
	uint32	GetFPS()		const { return m_FPS; }
	uint32	GetFrameCount() const { return m_FrameAcc; }

	Float GetFixedDeltaTime()					const { return m_FixedDeltaTime; }
	Float GetFixedTimeAccumulator()				const { return m_FixedAcc; }

	void SetFixedDeltaTime(Float fixedDelta)	{ m_FixedDeltaTime = fixedDelta; }
	void SetTimeScale(Float scale)				{ m_TimeScale = scale; }
	Float GetTimeScale()						const { return m_TimeScale; }
	Float GetScaledDeltaTime()					const { return m_DeltaTime * m_TimeScale; }

public:
	HRESULT Initialize();
	Float Update_Timer();
	Bool IsFixedUpdate() const;
	void ConsumeFixedDeltaTime();

private:
	LARGE_INTEGER m_Frequency = {};
	LARGE_INTEGER m_PrevTime = {};
	LARGE_INTEGER m_CurrentTime = {};

	Float m_TimeScale = {};

	Float m_DeltaTime = {};
	Float m_DeltaAcc = {};
	
	Float m_FixedDeltaTime = {};
	Float m_FixedAcc = {};

	uint32 m_FPS = {};
	uint32 m_FrameAcc = {};
	uint32 m_TempFPS = {};
	Float m_FPSTimeElapsed = {};

	Bool m_isActive = { true };

public:
	static Shared<Timer> Create();
};

NS_END