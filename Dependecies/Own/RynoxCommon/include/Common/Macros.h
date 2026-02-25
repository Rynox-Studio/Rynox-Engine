#pragma once

#ifdef _DEBUG
#	define RNX_DEBUG
#	define RNX_ENABLE_ASSERTS
#else
#	define RNX_RELEASE
#endif

#define RNX_DEBUGBREAK() __debugbreak()
#define RNX_BIT(x) (1 << x)
#define RNX_STRINGIFY(x) #x
#define RNX_EXPAND_MACRO(x) x
