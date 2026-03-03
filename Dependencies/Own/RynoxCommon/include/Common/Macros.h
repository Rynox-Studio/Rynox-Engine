#pragma once

#ifdef _DEBUG
#	define RNX_DEBUG
#	define RNX_ENABLE_ASSERTS
#else
#	define RNX_RELEASE
#endif

#if defined(_WIN32) || defined(_WIN64)
#  define RNX_WINDOWS
#endif

#if defined(RNX_WINDOWS)
#  if defined(RNX_RENDERER_EXPORTS)
#    define RNX_RENDERER_API __declspec(dllexport)
#  else
#    define RNX_RENDERER_API __declspec(dllimport)
#  endif
#else
#  define RNX_RENDERER_API
#endif

#define RNX_DEBUGBREAK() __debugbreak()
#define RNX_BIT(x) (1 << x)
#define RNX_STRINGIFY(x) #x
#define RNX_EXPAND_MACRO(x) x
