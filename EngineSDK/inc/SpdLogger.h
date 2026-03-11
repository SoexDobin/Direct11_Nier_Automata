#pragma once
#include "Engine_Define.h"
#include <functional>
#include <spdlog/common.h> // source_loc를 위해 필요

NS_BEGIN(Engine)

enum class LogLevel : int { Trace = 0, Debug, Info, Warn, Error, Critical };
using LogCallback = std::function<void(LogLevel, const std::string&)>;

class ENGINE_DLL SpdLogger final {
    NO_COPY(SpdLogger)
public:
    static void Initialize();
    static void Shutdown();

    // spdlog::source_loc를 인자로 받는 형태로 변경
    static void Trace(spdlog::source_loc loc, const std::string& message);
    static void Debug(spdlog::source_loc loc, const std::string& message);
    static void Info(spdlog::source_loc loc, const std::string& message);
    static void Warn(spdlog::source_loc loc, const std::string& message);
    static void Error(spdlog::source_loc loc, const std::string& message);
    static void Critical(spdlog::source_loc loc, const std::string& message);

    static void Trace(spdlog::source_loc loc, const std::wstring& message);
    static void Debug(spdlog::source_loc loc, const std::wstring& message);
    static void Info(spdlog::source_loc loc, const std::wstring& message);
    static void Warn(spdlog::source_loc loc, const std::wstring& message);
    static void Error(spdlog::source_loc loc, const std::wstring& message);
    static void Critical(spdlog::source_loc loc, const std::wstring& message);

    static void SetLogCallback(LogCallback callback);
};
NS_END

#ifdef _DEBUG
#define LOG_INIT() Engine::SpdLogger::Initialize()
#define LOG_SHUTDOWN() Engine::SpdLogger::Shutdown()

// 매크로 수정: 호출 시점의 파일, 라인, 함수명을 source_loc로 묶어서 전달합니다.
#define LOG_TRACE(fmt, ...)    Engine::SpdLogger::Trace({__FILE__, __LINE__, __FUNCTION__}, std::format(fmt, __VA_ARGS__))
#define LOG_DEBUG(fmt, ...)    Engine::SpdLogger::Debug({__FILE__, __LINE__, __FUNCTION__}, std::format(fmt, __VA_ARGS__))
#define LOG_INFO(fmt, ...)     Engine::SpdLogger::Info({__FILE__, __LINE__, __FUNCTION__}, std::format(fmt, __VA_ARGS__))
#define LOG_WARN(fmt, ...)     Engine::SpdLogger::Warn({__FILE__, __LINE__, __FUNCTION__}, std::format(fmt, __VA_ARGS__))
#define LOG_ERROR(fmt, ...)    Engine::SpdLogger::Error({__FILE__, __LINE__, __FUNCTION__}, std::format(fmt, __VA_ARGS__))
#define LOG_CRITICAL(fmt, ...) Engine::SpdLogger::Critical({__FILE__, __LINE__, __FUNCTION__}, std::format(fmt, __VA_ARGS__))

#else
// Release 빌드 시 처리 (기존과 동일)
#define LOG_INIT()           do {} while (0)
#define LOG_SHUTDOWN()       do {} while (0)
#define LOG_TRACE(fmt, ...)  do {} while (0)
#define LOG_DEBUG(fmt, ...)  do {} while (0)
#define LOG_INFO(fmt, ...)   do {} while (0)
#define LOG_WARN(fmt, ...)   do {} while (0)
#define LOG_ERROR(fmt, ...)  do {} while (0)
#define LOG_CRITICAL(fmt, ...) do {} while (0)
#endif