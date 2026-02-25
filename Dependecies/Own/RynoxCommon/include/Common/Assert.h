#pragma once

#include "Macros.h"
#include "Logger.h"

#ifdef RNX_ENABLE_ASSERTS

#	define RNX_INTERNAL_ASSERT_IMPL(cond, ...) if (!(cond)) { RNX_LOG_ERROR(__VA_ARGS__); RNX_DEBUGBREAK(); }

#	define RNX_INTERNAL_ASSERT_WITH_MSG(cond, message) RNX_INTERNAL_ASSERT_IMPL(cond, "Assertion failed: {0}", message)
#	define RNX_INTERNAL_ASSERT_NO_MSG(cond) RNX_INTERNAL_ASSERT_IMPL(cond, "Assertion '{0}' failed at {1}:{2}", RNX_STRINGIFY(cond), __FILE__, __LINE__)

#	define RNX_INTERNAL_ASSERT_GET_MACRO_NAME(_1, _2, NAME, ...) NAME
#	define RNX_INTERNAL_ASSERT_GET_MACRO(...) RNX_EXPAND_MACRO( RNX_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, RNX_INTERNAL_ASSERT_WITH_MSG, RNX_INTERNAL_ASSERT_NO_MSG) )

#	define RNX_ASSERT(...) RNX_EXPAND_MACRO( RNX_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(__VA_ARGS__) )
#else
#	define RNX_ASSERT(...)
#endif
