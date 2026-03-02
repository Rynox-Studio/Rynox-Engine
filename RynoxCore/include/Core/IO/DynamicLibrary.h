#pragma once

#include <string>

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
using LibHandle = HMODULE;
#else
#include <dlfcn.h>
using LibHandle = void*;
#endif

namespace Rynox::Core::IO
{
    class DynamicLibrary {
    public:
        static LibHandle Load(const std::string& path);
        static void UnLoad(LibHandle handle);
        static void* GetSymbol(LibHandle handle, const std::string& name);
    };
}