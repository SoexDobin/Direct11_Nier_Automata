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
#define LOG_TRACE(msg)		Engine::SpdLogger::Trace(msg)
#define LOG_DEBUG(msg)		Engine::SpdLogger::Debug(msg)
#define LOG_INFO(msg)		Engine::SpdLogger::Info(msg)
#define LOG_WARN(msg)		Engine::SpdLogger::Warn(msg)
#define LOG_ERROR(msg)		Engine::SpdLogger::Error(msg)
#define LOG_CRITICAL(msg)	Engine::SpdLogger::Critical(msg)
#else
	// constexpr로 컴파일 타임 완전 제거
#define LOG_INIT()			do { if constexpr (false) {} } while(0)
#define LOG_SHUTDOWN()		do { if constexpr (false) {} } while(0)
#define LOG_TRACE(msg)		do { if constexpr (false) { (void)(msg); } } while(0)
#define LOG_DEBUG(msg)		do { if constexpr (false) { (void)(msg); } } while(0)
#define LOG_INFO(msg)		do { if constexpr (false) { (void)(msg); } } while(0)
#define LOG_WARN(msg)		do { if constexpr (false) { (void)(msg); } } while(0)
#define LOG_ERROR(msg)		do { if constexpr (false) { (void)(msg); } } while(0)
#define LOG_CRITICAL(msg)	do { if constexpr (false) { (void)(msg); } } while(0)
#endif