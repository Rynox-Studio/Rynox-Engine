#include <Internal/Dx12UploadContext.h>

namespace Rynox::DirectX12
{
    bool Dx12UploadContext::Initialize(ID3D12Device14* device)
    {
        HRESULT hr = S_OK;

        hr = device->CreateCommandAllocator(
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            IID_PPV_ARGS(&Allocator)
        );
        if (FAILED(hr))
        {
            RNX_LOG_ERROR("[DirectX12] Failed to create Upload Allocator.");
            return false;
        }

        hr = device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            Allocator.Get(),
            nullptr,
            IID_PPV_ARGS(&List)
        );
        if (FAILED(hr))
        {
            RNX_LOG_ERROR("[DirectX12] Failed to create Upload Command List.");
            return false;
        }
        List->Close();

        return true;
    }

    void Dx12UploadContext::Shutdown()
    {
        List.Reset();
        Allocator.Reset();
        Pending = false;
    }

    void Dx12UploadContext::Flush(Dx12CommandQueue& queue)
    {
        if (!Pending) return;

        queue.Execute(List.Get());
        queue.Flush();

        Reset();
        Pending = false;
    }

    void Dx12UploadContext::Reset()
    {
        Allocator->Reset();
        List->Reset(Allocator.Get(), nullptr);
    }
}
