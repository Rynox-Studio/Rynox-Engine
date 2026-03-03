#pragma once

#include <ostream>
#include <string_view>
#include <mutex>

#include "LogLevel.h"

namespace Rynox::Common
{
	class Logger
	{
	public:
		Logger(std::ostream& os, LogLevel level);

		static Logger& Get();

		void Log(LogLevel level, std::string_view message);
		
		template<typename... Args>
		void Log(LogLevel level, std::string_view message_fmt, Args&&... args)
		{
			if (VerifyLevel(level))
			{
				Log(level, std::vformat(message_fmt, std::make_format_args(args...)));
			}
		}

		const std::ostream& GetOutputStream() const { return m_OutStream; }
		LogLevel GetLogLevel() const { return m_Level; }

		void SetLogLevel(LogLevel level) { std::lock_guard lock(m_Mutex); m_Level = level; }

	private:
		void InternalLog(std::string_view log);
		constexpr bool VerifyLevel(LogLevel level);

	private:
		std::ostream m_OutStream;
		LogLevel m_Level;
		std::mutex m_Mutex;
	};
}

#define RNX_LOG(level, ...) ::Rynox::Common::Logger::Get().Log(::Rynox::Common::LogLevel::level, __VA_ARGS__)

#define RNX_LOG_ERROR(...) RNX_LOG(Error, __VA_ARGS__)
#define RNX_LOG_WARNING(...) RNX_LOG(Warning, __VA_ARGS__)
#define RNX_LOG_INFO(...) RNX_LOG(Info, __VA_ARGS__)
#define RNX_LOG_DEBUG(...) RNX_LOG(Debug, __VA_ARGS__)
#define RNX_LOG_TRACE(...) RNX_LOG(Trace, __VA_ARGS__)
