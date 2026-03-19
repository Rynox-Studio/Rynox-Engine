#pragma once

#include <DirectX12.h>

namespace Rynox::DirectX12
{
    enum class ShaderStage
    {
        Vertex,
        Pixel,
        Compute,
    };

    class Dx12Shader
    {
    public:
        Dx12Shader() = default;
        Dx12Shader(
            const char* source,
            ShaderStage stage);
        ~Dx12Shader() = default;

        Dx12Shader(const Dx12Shader&) = delete;
        Dx12Shader& operator=(const Dx12Shader&) = delete;

        Dx12Shader(Dx12Shader&& other) noexcept;
        Dx12Shader& operator=(Dx12Shader&& other) noexcept;

        // Returns the bytecode desc you pass directly into PSO creation
        D3D12_SHADER_BYTECODE GetBytecode() const;

        bool IsValid() const { return m_Blob != nullptr; }

    private:
        static const wchar_t* GetTargetProfile(ShaderStage stage);

        ComPtr<IDxcBlob> m_Blob;
    };
}