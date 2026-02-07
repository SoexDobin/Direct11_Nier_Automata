#pragma once
#include "Engine_Define.h"

NS_BEGIN(Engine)

class ENGINE_DLL SpdLogger final
{
	NO_COPY(SpdLogger)
public:
	explicit SpdLogger() = default;
	~SpdLogger() = default;
public:
	static void Initialize();
	static void Shutdown();

	static void Trace(const std::string& message);
	static void Debug(const std::string& message);
	static void Info(const std::string& message);
	static void Warn(const std::string& message);
	static void Error(const std::string& message);
	static void Critical(const std::string& message);
	
	static void Trace(const std::wstring& message);
	static void Debug(const std::wstring& message);
	static void Info(const std::wstring& message);
	static void Warn(const std::wstring& message);
	static void Error(const std::wstring& message);
	static void Critical(const std::wstring& message);
};
NS_END

#ifdef _DEBUG
#define LOG_INIT()			Engine::SpdLogger::Initialize()
#define LOG_SHUTDOWN()		Engine::SpdLogger::Shutdown()
// ✅ 가변 인자 매크로
#define LOG_TRACE(fmt, ...)		Engine::SpdLogger::Trace(std::format(fmt, __VA_ARGS__))
#define LOG_DEBUG(fmt, ...)		Engine::SpdLogger::Debug(std::format(fmt, __VA_ARGS__))
#define LOG_INFO(fmt, ...)		Engine::SpdLogger::Info(std::format(fmt, __VA_ARGS__))
#define LOG_WARN(fmt, ...)		Engine::SpdLogger::Warn(std::format(fmt, __VA_ARGS__))
#define LOG_ERROR(fmt, ...)		Engine::SpdLogger::Error(std::format(fmt, __VA_ARGS__))
#define LOG_CRITICAL(fmt, ...)	Engine::SpdLogger::Critical(std::format(fmt, __VA_ARGS__))
#else
// constexpr로 컴파일 타임 완전 제거
#define LOG_INIT()				do { if constexpr (false) {} } while(0)
#define LOG_SHUTDOWN()			do { if constexpr (false) {} } while(0)
#define LOG_TRACE(fmt, ...)		do { if constexpr (false) { (void)(fmt); } } while(0)
#define LOG_DEBUG(fmt, ...)		do { if constexpr (false) { (void)(fmt); } } while(0)
#define LOG_INFO(fmt, ...)		do { if constexpr (false) { (void)(fmt); } } while(0)
#define LOG_WARN(fmt, ...)		do { if constexpr (false) { (void)(fmt); } } while(0)
#define LOG_ERROR(fmt, ...)		do { if constexpr (false) { (void)(fmt); } } while(0)
#define LOG_CRITICAL(fmt, ...)	do { if constexpr (false) { (void)(fmt); } } while(0)
#endif