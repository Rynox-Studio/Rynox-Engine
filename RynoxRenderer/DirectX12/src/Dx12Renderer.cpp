#include <Renderer/Dx12Renderer.h>

#include <Common/Assert.h>

namespace Rynox::Renderer
{
	Dx12Renderer::~Dx12Renderer()
	{
		Cleanup();
	}

	bool Dx12Renderer::Initialize(const RendererDesc& desc)
	{
		RNX_ASSERT(!m_Initialized, "Renderer already initialized. (Reinitializing...)");
		if (m_Initialized) Cleanup();
		m_Initialized = false;

		m_Desc = desc;
		{
			if (m_Desc.outputWidth == 0 || m_Desc.outputHeight == 0)
			{
				RECT rc;
				GetClientRect(reinterpret_cast<HWND>(m_Desc.nWindow), &rc);

				m_Desc.outputWidth = rc.right - rc.left;
				m_Desc.outputHeight = rc.bottom - rc.top;
			}
		}

		m_Viewport.Width = (float)m_Desc.outputWidth;
		m_Viewport.Height = (float)m_Desc.outputHeight;

		HRESULT hr = S_OK;

		// Enable Debug Layer
#ifndef NDEBUG
		{
			ComPtr<ID3D12Debug6> debug;
			hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debug));
			if (SUCCEEDED(hr))
			{
				debug->EnableDebugLayer();
			}
			else
			{
				RNX_LOG_WARNING("[DirectX12] Failed to enable debug layer.");
			}
		}
#endif

		// Create DXGI Factory
		{
			UINT Flags = 0;
#ifndef NDEBUG
			Flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif
			hr = CreateDXGIFactory2(Flags, IID_PPV_ARGS(&m_Factory));
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create DXGI Factory.");
				return false;
			}
		}

		// Select Hardware Adapter
		{
			ComPtr<IDXGIAdapter1> adapter;
			for (UINT i = 0;
				m_Factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&adapter)) != DXGI_ERROR_NOT_FOUND;
				i++)
			{
				DXGI_ADAPTER_DESC1 adapterDesc;
				adapter->GetDesc1(&adapterDesc);

				if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue; // Skip software adapters

				hr = D3D12CreateDevice(
					adapter.Get(),
					D3D_FEATURE_LEVEL_12_0,
					__uuidof(ID3D12Device),
					nullptr
				);
				if (SUCCEEDED(hr))
				{
					adapter.As(&m_Adapter);
					break;
				}
			}

			if (!m_Adapter)
			{
				RNX_LOG_ERROR("[DirectX12] Failed to select hardware adapter.");
				return false;
			}
		}

		// Create D3D12Device
		{
			hr = D3D12CreateDevice(
				m_Adapter.Get(),
				D3D_FEATURE_LEVEL_12_0,
				IID_PPV_ARGS(&m_Device)
			);
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create D3D12Device.");
				return false;
			}
		}

		// Create Direct Command Queue
		{
			D3D12_COMMAND_QUEUE_DESC queueDesc{};
			queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
			queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

			hr = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_DirectQueue));
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create Direct Command Queue.");
				return false;
			}
		}

		// Create SwapChain
		{
			DXGI_SWAP_CHAIN_DESC1 scDesc{};
			scDesc.Width = m_Desc.outputWidth;
			scDesc.Height = m_Desc.outputHeight;
			scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			scDesc.Stereo = FALSE;
			scDesc.SampleDesc.Count = 1;
			scDesc.SampleDesc.Quality = 0;
			scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			scDesc.BufferCount = BUFFER_COUNT;
			scDesc.Scaling = DXGI_SCALING_STRETCH;
			scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

			ComPtr<IDXGISwapChain1> swapchain;
			hr = m_Factory->CreateSwapChainForHwnd(
				m_DirectQueue.Get(),
				reinterpret_cast<HWND>(m_Desc.nWindow),
				&scDesc,
				nullptr,
				nullptr,
				&swapchain
			);
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create DXGI SwapChain.");
				return false;
			}

			hr = swapchain.As(&m_SwapChain);
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create DXGI SwapChain.");
				return false;
			}

			m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
		}

		// Create Descriptor Heap for RTV
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			heapDesc.NumDescriptors = BUFFER_COUNT;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_RTVHeap));
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create RTV Heap.");
				return false;
			}

			m_RTVDescSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		}

		// Create Descriptor Heap for DSV
		{
			D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
			heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
			heapDesc.NumDescriptors = 1;
			heapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

			hr = m_Device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&m_DSVHeap));
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create DSV Heap.");
				return false;
			}
		}

		// Create RTVs
		{
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();

			for (UINT i = 0; i < BUFFER_COUNT; i++)
			{
				hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RTVs[i]));
				if (FAILED(hr))
				{
					RNX_LOG_ERROR("[DirectX12] Failed to get Back Buffer.");
					return false;
				}

				m_Device->CreateRenderTargetView(
					m_RTVs[i].Get(),
					nullptr,
					rtvHandle
				);

				rtvHandle.ptr += m_RTVDescSize;
			}
		}

		// Create Depth/Stencil Buffer
		{
			D3D12_RESOURCE_DESC dsResDesc = {};
			dsResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			dsResDesc.Alignment = 0;
			dsResDesc.Width = m_Desc.outputWidth;
			dsResDesc.Height = m_Desc.outputHeight;
			dsResDesc.DepthOrArraySize = 1;
			dsResDesc.MipLevels = 1;
			dsResDesc.Format = DXGI_FORMAT_D32_FLOAT;
			dsResDesc.SampleDesc.Count = 1;
			dsResDesc.SampleDesc.Quality = 0;
			dsResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			dsResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			D3D12_CLEAR_VALUE clearValue = {};
			clearValue.Format = DXGI_FORMAT_D32_FLOAT;
			clearValue.DepthStencil.Depth = 1.0f;
			clearValue.DepthStencil.Stencil = 0;

			D3D12_HEAP_PROPERTIES heapProps = {};
			heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
			heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
			heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
			heapProps.CreationNodeMask = 1;
			heapProps.VisibleNodeMask = 1;

			hr = m_Device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&dsResDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&clearValue, IID_PPV_ARGS(&m_DepthStencilBuffer)
			);
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create Depth/Stencil Buffer.");
				return false;
			}

			m_Device->CreateDepthStencilView(
				m_DepthStencilBuffer.Get(),
				nullptr,
				m_DSVHeap->GetCPUDescriptorHandleForHeapStart()
			);
		}

		// Create Command Allocator & Command List
		{
			for (UINT i = 0; i < BUFFER_COUNT; i++)
			{
				hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator[i]));
				if (FAILED(hr))
				{
					RNX_LOG_ERROR("[DirectX12] Failed to create Command Allocator.");
					return false;
				}
			}

			hr = m_Device->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_DIRECT,
				m_CommandAllocator[0].Get(),
				nullptr,
				IID_PPV_ARGS(&m_CommandList)
			);
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create Command List.");
				return false;
			}
			m_CommandList->Close();
		}

		// Create Fence
		{
			m_FenceEvent = CreateEventW(nullptr, FALSE, FALSE, nullptr);
			if (!m_FenceEvent)
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create Fence Event.");
				return false;
			}

			hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create Fence.");
				return false;
			}
		}

		RNX_LOG_INFO("[DirectX12] Initialized successfully.");
		m_Initialized = true;
		return true;
	}

	const RendererDesc& Dx12Renderer::GetDesc() const
	{
		return m_Desc;
	}

	void Dx12Renderer::BeginFrame()
	{
		RNX_ASSERT(m_Initialized);
		if (!m_Initialized) return;

		// Wait GPU to finish
		if (m_Fence->GetCompletedValue() < m_FrameValues[m_FrameIndex])
		{
			m_Fence->SetEventOnCompletion(m_FrameValues[m_FrameIndex], m_FenceEvent);
			WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
		}

		// Reset allocator and command list
		m_CommandAllocator[m_FrameIndex]->Reset();
		m_CommandList->Reset(m_CommandAllocator[m_FrameIndex].Get(), nullptr);

		// Transition back buffer
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_RTVs[m_FrameIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		m_CommandList->ResourceBarrier(1, &barrier);

		// Bind render targets
		D3D12_CPU_DESCRIPTOR_HANDLE rtv = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
		rtv.ptr += m_FrameIndex * m_RTVDescSize;

		D3D12_CPU_DESCRIPTOR_HANDLE dsv = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();

		m_CommandList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);

		// Clear
		constexpr float clearColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		m_CommandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
		m_CommandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		// Set viewport
		m_CommandList->RSSetViewports(1, &m_Viewport);
	}

	void Dx12Renderer::EndFrame()
	{
		RNX_ASSERT(m_Initialized);
		if (!m_Initialized) return;

		// Transition back buffer
		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_RTVs[m_FrameIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		m_CommandList->ResourceBarrier(1, &barrier);

		// Close & Execute
		m_CommandList->Close();

		ID3D12CommandList* lists[] = { m_CommandList.Get() };
		m_DirectQueue->ExecuteCommandLists(1, lists);

		// Present
		m_SwapChain->Present(0, 0);

		// Signal Fence
		m_DirectQueue->Signal(m_Fence.Get(), ++m_FrameValues[m_FrameIndex]);

		// Advance to next frame index
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
	}

	bool Dx12Renderer::SetOutputSize(uint32_t width, uint32_t height)
	{
		if (!m_Initialized) return;
		if (m_Desc.outputWidth == width && m_Desc.outputHeight == height) return;

		FlushGPU();

		for (UINT i = 0; i < BUFFER_COUNT; i++)
		{
			m_RTVs[i].Reset();
		}
		m_DepthStencilBuffer.Reset();

		HRESULT hr = m_SwapChain->ResizeBuffers(
			BUFFER_COUNT,
			width, height,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
		);
		if (FAILED(hr))
		{
			RNX_LOG_ERROR("[DirectX12] Failed to resize Swapchain.");
			return false;
		}

		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		// Recreate RTVs
		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < BUFFER_COUNT; i++)
		{
			hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RTVs[i]));
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to get Back Buffer during resize.");
				return;
			}
			m_Device->CreateRenderTargetView(m_RTVs[i].Get(), nullptr, rtvHandle);
			rtvHandle.ptr += m_RTVDescSize;
		}

		// Recreate depth/stencil buffer
		{
			D3D12_RESOURCE_DESC dsResDesc{};
			dsResDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			dsResDesc.Width = width;
			dsResDesc.Height = height;
			dsResDesc.DepthOrArraySize = 1;
			dsResDesc.MipLevels = 1;
			dsResDesc.Format = DXGI_FORMAT_D32_FLOAT;
			dsResDesc.SampleDesc.Count = 1;
			dsResDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			dsResDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			D3D12_CLEAR_VALUE clearValue{};
			clearValue.Format = DXGI_FORMAT_D32_FLOAT;
			clearValue.DepthStencil.Depth = 1.0f;

			D3D12_HEAP_PROPERTIES heapProps{};
			heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

			hr = m_Device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&dsResDesc,
				D3D12_RESOURCE_STATE_DEPTH_WRITE,
				&clearValue,
				IID_PPV_ARGS(&m_DepthStencilBuffer)
			);
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to recreate Depth/Stencil Buffer during resize.");
				return;
			}

			m_Device->CreateDepthStencilView(
				m_DepthStencilBuffer.Get(),
				nullptr,
				m_DSVHeap->GetCPUDescriptorHandleForHeapStart()
			);
		}
	}

	void Dx12Renderer::SetViewport(Viewport viewport)
	{
		m_Viewport.TopLeftX = (float)viewport.x;
		m_Viewport.TopLeftY = (float)viewport.y;
		m_Viewport.Width = (float)viewport.width;
		m_Viewport.Height = (float)viewport.height;
	}

	void Dx12Renderer::SetClearColor(Math::Vec4 color)
	{
		m_ClearColor = color;
	}

	Graphics::MeshHandle Dx12Renderer::LoadMesh(const Graphics::MeshData& mesh)
	{
		return {};
	}

	Graphics::ShaderHandle Dx12Renderer::LoadShader(const Graphics::ShaderData& shader)
	{
		return Graphics::ShaderHandle();
	}

	void Dx12Renderer::DrawMesh(const Graphics::MeshHandle mesh, const Graphics::ShaderHandle shader)
	{
	}

	void Dx12Renderer::Cleanup()
	{
		// Release fence event handle
		if (m_FenceEvent)
		{
			CloseHandle(m_FenceEvent);
			m_FenceEvent = nullptr;
		}

		// Release all ComPtrs (order matters — reverse of creation)
		m_Fence.Reset();
		m_CommandList.Reset();

		for (UINT i = 0; i < BUFFER_COUNT; i++)
			m_CommandAllocator[i].Reset();

		m_DepthStencilBuffer.Reset();

		for (UINT i = 0; i < BUFFER_COUNT; i++)
			m_RTVs[i].Reset();

		m_DSVHeap.Reset();
		m_RTVHeap.Reset();
		m_SwapChain.Reset();
		m_DirectQueue.Reset();
		m_Adapter.Reset();
		m_Device.Reset();
		m_Factory.Reset();

		m_FrameIndex = 0;
		for (UINT i = 0; i < BUFFER_COUNT; i++)
			m_FrameValues[i] = 0;

		m_Initialized = false;
	}

	void Dx12Renderer::FlushGPU()
	{
		if (!m_Fence || !m_DirectQueue) return;

		UINT64 flushValue = 0;
		for (UINT i = 0; i < BUFFER_COUNT; i++)
		{
			flushValue = max(flushValue, m_FrameValues[i]);
		}

		flushValue++;
		m_DirectQueue->Signal(m_Fence.Get(), flushValue);

		if (m_Fence->GetCompletedValue() < flushValue)
		{
			m_Fence->SetEventOnCompletion(flushValue, m_FenceEvent);
			WaitForSingleObjectEx(m_FenceEvent, INFINITE, FALSE);
		}
	}
}
