#pragma once

#include <format>

namespace Rynox::Common
{
	enum class LogLevel
	{
		Error,
		Warning,
		Info,
		Debug,
		Trace
	};
}

namespace std
{
	using namespace Rynox::Common;

	template<>
	struct formatter<LogLevel>
	{
		constexpr auto parse(format_parse_context& ctx)
		{
			return ctx.begin();
		}

		auto format(LogLevel log_level, format_context& ctx) const
		{
			switch (log_level)
			{
			case LogLevel::Error: format_to(ctx.out(), "{}", "Error"); break;
			case LogLevel::Warning: format_to(ctx.out(), "{}", "Warning"); break;
			case LogLevel::Info: format_to(ctx.out(), "{}", "Info"); break;
			case LogLevel::Debug: format_to(ctx.out(), "{}", "Debug"); break;
			case LogLevel::Trace: format_to(ctx.out(), "{}", "Trace"); break;
			}

			return ctx.out();
		}
	};
}
