#include "Core/IO/DynamicLibrary.h"

#if defined(_WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif

namespace Rynox::Core::IO
{
    LibHandle DynamicLibrary::Load(const std::string& path)
    {
#if defined(_WIN32)
        return LoadLibraryA(path.c_str());
#else
        return dlopen(path.c_str(), RTLD_NOW);
#endif
    }

    bool DynamicLibrary::UnLoad(LibHandle handle)
    {
#if defined(_WIN32)
        return FreeLibrary((HMODULE)handle);
#else
        dlclose(handle);
#endif
    }

    void* DynamicLibrary::GetSymbol(LibHandle handle, const std::string& name)
    {
#if defined(_WIN32)
        return reinterpret_cast<void*>(GetProcAddress((HMODULE)handle, name.c_str()));
#else
        return dlsym(handle, name.c_str());
#endif
    }
}
