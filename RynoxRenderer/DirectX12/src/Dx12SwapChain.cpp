#include <Internal/Dx12SwapChain.h>

namespace Rynox::DirectX12
{
    bool Dx12SwapChain::Initialize(
        IDXGIFactory7* factory, ID3D12Device14* device,
        ID3D12CommandQueue* queue, HWND hWnd,
        uint32_t width, uint32_t height)
    {
        HRESULT hr = S_OK;

        // SwapChain
        {
            DXGI_SWAP_CHAIN_DESC1 desc{};
            desc.Width = width;
            desc.Height = height;
            desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
            desc.Stereo = FALSE;
            desc.SampleDesc = { 1, 0 };
            desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
            desc.BufferCount = BUFFER_COUNT;
            desc.Scaling = DXGI_SCALING_STRETCH;
            desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
            desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |
                DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

            ComPtr<IDXGISwapChain1> swapchain;
            hr = factory->CreateSwapChainForHwnd(
                queue,
                hWnd,
                &desc,
                nullptr, nullptr,
                &swapchain
            );
            if (FAILED(hr))
            {
                RNX_LOG_ERROR("[DirectX12] Failed to create DXGI SwapChain.");
                return false;
            }

            swapchain.As(&SwapChain);
            FrameIndex = SwapChain->GetCurrentBackBufferIndex();
        }

        // RTV Heap
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc{};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
            desc.NumDescriptors = BUFFER_COUNT;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&RTVHeap));
            if (FAILED(hr))
            {
                RNX_LOG_ERROR("[DirectX12] Failed to create RTV Heap.");
                return false;
            }

            RTVDescSize = device->GetDescriptorHandleIncrementSize(
                D3D12_DESCRIPTOR_HEAP_TYPE_RTV
            );
        }

        // DSV Heap
        {
            D3D12_DESCRIPTOR_HEAP_DESC desc{};
            desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
            desc.NumDescriptors = 1;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

            hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&DSVHeap));
            if (FAILED(hr))
            {
                RNX_LOG_ERROR("[DirectX12] Failed to create DSV Heap.");
                return false;
            }
        }

        // RTVs + DSV — общий код с Resize
        if (!CreateSizeDependentResources(device, width, height))
            return false;

        return true;
    }

    bool Dx12SwapChain::Resize(ID3D12Device14* device, uint32_t width, uint32_t height)
    {
        for (UINT i = 0; i < BUFFER_COUNT; i++)
        {
            RTVs[i].Reset();
        }
        DepthStencilBuffer.Reset();

        HRESULT hr = SwapChain->ResizeBuffers(
            BUFFER_COUNT,
            width, height,
            DXGI_FORMAT_R8G8B8A8_UNORM,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH |
            DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING
        );
        if (FAILED(hr))
        {
            RNX_LOG_ERROR("[DirectX12] Failed to resize DXGI SwapChain.");
            return false;
        }

        FrameIndex = SwapChain->GetCurrentBackBufferIndex();
        return CreateSizeDependentResources(device, width, height);
    }

    void Dx12SwapChain::Shutdown()
    {
        for (UINT i = 0; i < BUFFER_COUNT; i++)
            RTVs[i].Reset();

        DepthStencilBuffer.Reset();
        DSVHeap.Reset();
        RTVHeap.Reset();
        SwapChain.Reset();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE Dx12SwapChain::GetCurrentRTV() const
    {
        D3D12_CPU_DESCRIPTOR_HANDLE handle =
            RTVHeap->GetCPUDescriptorHandleForHeapStart();
        handle.ptr += FrameIndex * RTVDescSize;
        return handle;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE Dx12SwapChain::GetDSV() const
    {
        return DSVHeap->GetCPUDescriptorHandleForHeapStart();
    }

    ID3D12Resource* Dx12SwapChain::GetCurrentBackBuffer() const
    {
        return RTVs[FrameIndex].Get();
    }

    bool Dx12SwapChain::CreateSizeDependentResources(ID3D12Device14* device,
        uint32_t width,
        uint32_t height)
    {
        HRESULT hr = S_OK;

        // RTVs
        {
            D3D12_CPU_DESCRIPTOR_HANDLE handle =
                RTVHeap->GetCPUDescriptorHandleForHeapStart();

            for (UINT i = 0; i < BUFFER_COUNT; i++)
            {
                hr = SwapChain->GetBuffer(i, IID_PPV_ARGS(&RTVs[i]));
                if (FAILED(hr))
                {
                    RNX_LOG_ERROR("[DirectX12] Failed to get SwapChain Buffer.");
                    return false;
                }

                device->CreateRenderTargetView(RTVs[i].Get(), nullptr, handle);
                handle.ptr += RTVDescSize;
            }
        }

        // Depth/Stencil
        {
            D3D12_RESOURCE_DESC desc{};
            desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
            desc.Width = width;
            desc.Height = height;
            desc.DepthOrArraySize = 1;
            desc.MipLevels = 1;
            desc.Format = DXGI_FORMAT_D32_FLOAT;
            desc.SampleDesc = { 1, 0 };
            desc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
            desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

            D3D12_CLEAR_VALUE clearValue{};
            clearValue.Format = DXGI_FORMAT_D32_FLOAT;
            clearValue.DepthStencil.Depth = 1.0f;
            clearValue.DepthStencil.Stencil = 0;

            D3D12_HEAP_PROPERTIES heapProps{};
            heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

            hr = device->CreateCommittedResource(
                &heapProps,
                D3D12_HEAP_FLAG_NONE,
                &desc,
                D3D12_RESOURCE_STATE_DEPTH_WRITE,
                &clearValue,
                IID_PPV_ARGS(&DepthStencilBuffer)
            );
            if (FAILED(hr))
            {
                RNX_LOG_ERROR("[DirectX12] Failed to create Depth/Stencil Buffer.");
                return false;
            }

            device->CreateDepthStencilView(
                DepthStencilBuffer.Get(),
                nullptr,
                DSVHeap->GetCPUDescriptorHandleForHeapStart()
            );
        }

        return true;
    }
}
