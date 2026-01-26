#include "Timer.h"

Timer::Timer() {}

void Timer::Initialize(void* args) {
    QueryPerformanceFrequency(&m_Frequency);
    QueryPerformanceCounter(&m_PrevTime);

    m_CurrentTime = m_PrevTime;
    m_DeltaTime = 0.0f;
    m_TotalTime = 0.0f;
    m_TimeScale = 1.0f;
    m_FixedDeltaTime = 0.016f; // 60Hz
    m_FixedTimeAcc = 0.0f;
    m_FPS = 0;
    m_FrameCount = 0;
    m_FPSCount = 0;
    m_FPSTimeElapsed = 0.0f;
}

void Timer::Update_Timer() {
    if (!m_IsActive) return;

    QueryPerformanceCounter(&m_CurrentTime);

    m_DeltaTime =
        static_cast<Float>(m_CurrentTime.QuadPart - m_PrevTime.QuadPart) /
        static_cast<Float>(m_Frequency.QuadPart);

    m_PrevTime = m_CurrentTime;     // 이전 시간 업데이트
    m_TotalTime += m_DeltaTime;     // 총 경과 시간 누적 
    m_FixedTimeAcc += m_DeltaTime * m_TimeScale; // FixedUpdate 누적 시간

    ++m_FrameCount;

    // FPS 계산 (1초마다 갱신)
    ++m_FPSCount;
    m_FPSTimeElapsed += m_DeltaTime;

    if (m_FPSTimeElapsed >= 1.0f) {
        m_FPS = m_FPSCount;
        m_FPSCount = 0;
        m_FPSTimeElapsed = 0.0f;
    }
}

Bool Timer::ShouldFixedUpdate() const
{
    return m_FixedTimeAcc >= m_FixedDeltaTime;
}

void Timer::ConsumeFixedDeltaTime()
{
    m_FixedTimeAcc -= m_FixedDeltaTime;
}

Shared<Timer> Timer::Create() {
    auto timer = std::make_shared<Timer>();

    if (timer.get() == nullptr)
    {
        MSG_BOX("Failed To Create Timer");
        return nullptr;
    }

    timer->Initialize(nullptr);
    return timer;
}