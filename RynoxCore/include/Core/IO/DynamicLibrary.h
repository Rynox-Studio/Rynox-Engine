#pragma once

#include <string>

using LibHandle = void*;

namespace Rynox::Core::IO
{
    class DynamicLibrary {
    public:
        static LibHandle Load(const std::string& path);
        static bool UnLoad(LibHandle handle);
        static void* GetSymbol(LibHandle handle, const std::string& name);
    };
}