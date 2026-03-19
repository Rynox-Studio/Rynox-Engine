#include <Dx12Shader.h>

#include <Common/Assert.h>
#include <vector>

namespace Rynox::DirectX12
{
    static const wchar_t* GetEntryPoint(ShaderStage stage)
    {
        switch (stage)
        {
        case ShaderStage::Vertex:  return L"VSMain";
        case ShaderStage::Pixel:   return L"PSMain";
        case ShaderStage::Compute: return L"CSMain";
        default:                   return L"VSMain";
        }
    }

    const wchar_t* Dx12Shader::GetTargetProfile(ShaderStage stage)
    {
        switch (stage)
        {
        case ShaderStage::Vertex:  return L"vs_6_0";
        case ShaderStage::Pixel:   return L"ps_6_0";
        case ShaderStage::Compute: return L"cs_6_0";
        default:                   return L"vs_6_0";
        }
    }

    Dx12Shader::Dx12Shader(const char* source, ShaderStage stage)
    {
        RNX_ASSERT(source, "Source is null");

        // ----------------------------------------------------------------
        // 1. Create DXC utils + compiler
        // ----------------------------------------------------------------
        ComPtr<IDxcUtils>    utils;
        ComPtr<IDxcCompiler3> compiler;

        DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf()));
        DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf()));

        // ----------------------------------------------------------------
        // 2. Wrap the raw source string in a DXC blob — no copy, no file
        // ----------------------------------------------------------------
        ComPtr<IDxcBlobEncoding> sourceBlob;
        utils->CreateBlobFromPinned(
            source,
            static_cast<UINT32>(strlen(source)),
            DXC_CP_UTF8,
            sourceBlob.GetAddressOf()
        );

        DxcBuffer sourceBuffer = {};
        sourceBuffer.Ptr = sourceBlob->GetBufferPointer();
        sourceBuffer.Size = sourceBlob->GetBufferSize();
        sourceBuffer.Encoding = DXC_CP_UTF8;

        // ----------------------------------------------------------------
        // 3. Compile arguments
        // ----------------------------------------------------------------
        const wchar_t* entryPoint = GetEntryPoint(stage);
        const wchar_t* profile = GetTargetProfile(stage);

        std::vector<const wchar_t*> args =
        {
            L"shader",              // virtual filename for error messages
            L"-E", entryPoint,
            L"-T", profile,
            L"-Zpr",                // row-major matrices
    #ifdef _DEBUG
            L"-Zi",                 // debug info
            L"-Od",                 // no optimization
    #else
            L"-O3",
    #endif
        };

        // ----------------------------------------------------------------
        // 4. Compile
        // ----------------------------------------------------------------
        ComPtr<IDxcResult> result;
        HRESULT hr = compiler->Compile(
            &sourceBuffer,
            args.data(),
            static_cast<UINT32>(args.size()),
            nullptr,
            IID_PPV_ARGS(result.GetAddressOf())
        );

        if (FAILED(hr))
        {
            RNX_LOG_ERROR("[DirectX12] DXC compiler call failed.");
            return;
        }

        // ----------------------------------------------------------------
        // 5. Errors / warnings
        // ----------------------------------------------------------------
        ComPtr<IDxcBlobUtf8> errors;
        result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(errors.GetAddressOf()), nullptr);
        if (errors && errors->GetStringLength() > 0)
        {
            RNX_LOG_DEBUG(errors->GetStringPointer());
        }

        HRESULT compileStatus = S_OK;
        result->GetStatus(&compileStatus);
        if (FAILED(compileStatus))
        {
            RNX_LOG_ERROR("[DirectX12] Compilation failed.");
            return;
        }

        // ----------------------------------------------------------------
        // 6. Extract bytecode
        // ----------------------------------------------------------------
        result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(m_Blob.GetAddressOf()), nullptr);
    }

    Dx12Shader::Dx12Shader(Dx12Shader&& other) noexcept
        : m_Blob(std::move(other.m_Blob))
    {
    }

    Dx12Shader& Dx12Shader::operator=(Dx12Shader&& other) noexcept
    {
        if (this != &other)
        {
            m_Blob = std::move(other.m_Blob);
        }
        return *this;
    }

    D3D12_SHADER_BYTECODE Dx12Shader::GetBytecode() const
    {
        return
        {
            m_Blob->GetBufferPointer(),
            m_Blob->GetBufferSize()
        };
    }

}