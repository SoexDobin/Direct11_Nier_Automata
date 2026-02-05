#include "SpdLogger.h"

#ifdef _DEBUG

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/sinks/rotating_file_sink.h"
#include "String_Helper.h"
#include <Windows.h>
#include <memory>

NS_BEGIN(Engine)

static Shared<spdlog::logger> g_Logger = { nullptr };

void SpdLogger::Initialize()
{
	try
	{
		// 컬러 콘솔 싱크
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_level(spdlog::level::trace);
		console_sink->set_pattern("[%H:%M:%S.%e] [%^%l%$] %v");

		// 로테이션 파일 싱크 (10MB, 최대 3개)
		auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
			"logs/engine.log",
			1024 * 1024 * 10,
			3
		);
		file_sink->set_level(spdlog::level::trace);
		file_sink->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%l] [%t] %v");

		// 멀티 싱크 로거
		spdlog::sinks_init_list sink_list = { console_sink, file_sink };
		g_Logger = std::make_shared<spdlog::logger>("EngineLogger", sink_list.begin(), sink_list.end());
		g_Logger->set_level(spdlog::level::trace);
		g_Logger->flush_on(spdlog::level::warn);

		spdlog::set_default_logger(g_Logger);
		spdlog::info("SpdLogger Initialized Successfully");
	}
	catch (const spdlog::spdlog_ex& ex)
	{
		std::string errorMsg = "SpdLogger Init Failed: " + std::string(ex.what());
		MessageBoxA(nullptr, errorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
	}
}

void SpdLogger::Shutdown()
{
	if (g_Logger)
	{
		spdlog::info("==========================");
		spdlog::info("SpdLogger Shutting Down...");
		g_Logger->flush();
		g_Logger.reset();
		spdlog::drop_all();
		spdlog::shutdown();
	}
}

void SpdLogger::Trace(const std::string& message)
{ if (g_Logger) g_Logger->trace(message); }
void SpdLogger::Debug(const std::string& message)
{ if (g_Logger) g_Logger->debug(message); } 
void SpdLogger::Info(const std::string& message)
{ if (g_Logger) g_Logger->info(message); }
void SpdLogger::Warn(const std::string& message)
{ if (g_Logger) g_Logger->warn(message); }
void SpdLogger::Error(const std::string& message)
{ if (g_Logger) g_Logger->error(message); }
void SpdLogger::Critical(const std::string& message)
{ if (g_Logger) g_Logger->critical(message); }

void SpdLogger::Trace(const std::wstring& message)
{ Trace(Helper::To_String(message)); }
void SpdLogger::Debug(const std::wstring& message)
{ Debug(Helper::To_String(message)); }
void SpdLogger::Info(const std::wstring& message)
{ Info(Helper::To_String(message)); }
void SpdLogger::Warn(const std::wstring& message)
{ Warn(Helper::To_String(message)); }
void SpdLogger::Error(const std::wstring& message)
{ Error(Helper::To_String(message)); }
void SpdLogger::Critical(const std::wstring& message)
{ Critical(Helper::To_String(message)); }

NS_END

#else

// Release 빌드: 빈 함수
NS_BEGIN(Engine)

void SpdLogger::Initialize() {}
void SpdLogger::Shutdown() {}

void SpdLogger::Trace(const std::string& message) {}
void SpdLogger::Debug(const std::string& message) {}
void SpdLogger::Info(const std::string& message) {}
void SpdLogger::Warn(const std::string& message) {}
void SpdLogger::Error(const std::string& message) {}
void SpdLogger::Critical(const std::string& message) {}

void SpdLogger::Trace(const std::wstring& message) {}
void SpdLogger::Debug(const std::wstring& message) {}
void SpdLogger::Info(const std::wstring& message) {}
void SpdLogger::Warn(const std::wstring& message) {}
void SpdLogger::Error(const std::wstring& message) {}
void SpdLogger::Critical(const std::wstring& message) {}

NS_END

#endif
