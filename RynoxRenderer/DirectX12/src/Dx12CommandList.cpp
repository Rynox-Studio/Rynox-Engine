#include <Internal/Dx12CommandList.h>

namespace Rynox::DirectX12
{
    bool Dx12CommandList::Initialize(ID3D12Device14* device)
    {
        HRESULT hr = S_OK;

        for (UINT i = 0; i < BUFFER_COUNT; i++)
        {
            hr = device->CreateCommandAllocator(
                D3D12_COMMAND_LIST_TYPE_DIRECT,
                IID_PPV_ARGS(&Allocators[i])
            );
            if (FAILED(hr))
            {
                RNX_LOG_ERROR("[DirectX12] Failed to create Command Allocator.");
                return false;
            }
        }

        hr = device->CreateCommandList(
            0,
            D3D12_COMMAND_LIST_TYPE_DIRECT,
            Allocators[0].Get(),
            nullptr,
            IID_PPV_ARGS(&List)
        );
        if (FAILED(hr))
        {
            RNX_LOG_ERROR("[DirectX12] Failed to create Command List.");
            return false;
        }

        List->Close();
        return true;
    }

    void Dx12CommandList::Shutdown()
    {
        List.Reset();
        for (UINT i = 0; i < BUFFER_COUNT; i++)
        {
            Allocators[i].Reset();
        }
    }

    void Dx12CommandList::Reset(uint32_t frameIndex)
    {
        Allocators[frameIndex]->Reset();
        List->Reset(Allocators[frameIndex].Get(), nullptr);
    }
}
