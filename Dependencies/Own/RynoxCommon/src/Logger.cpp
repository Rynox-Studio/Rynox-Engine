#include "Common/Logger.h"

#include <iostream>

namespace Rynox::Common
{
	Logger::Logger(std::ostream& os, LogLevel level)
		: m_OutStream(os.rdbuf())
		, m_Level(level)
	{
	}

	Logger& Logger::Get()
	{
		static Logger s_Instance{ std::cerr, LogLevel::Trace };
		return s_Instance;
	}

	void Logger::Log(LogLevel level, std::string_view message)
	{
		if (VerifyLevel(level))
		{
			InternalLog(std::format("[{}] {}", level, message));
		}
	}

	void Logger::InternalLog(std::string_view log)
	{
		std::lock_guard lock(m_Mutex);
		m_OutStream << log << std::endl;
	}

	constexpr bool Logger::VerifyLevel(LogLevel level)
	{
		return m_Level >= level;
	}
}
