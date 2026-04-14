#pragma once

#if defined(_WIN32) || defined(_WIN64)
#	define RNX_PLATFORM_WINDOWS
#elif defined(__linux__)
#	define RNX_PLATFORM_LINUX
#else
#	error "Unsupported platform!"
#endif
