#include <Dx12Renderer.h>

namespace Rynox::DirectX12
{
	Dx12Renderer::~Dx12Renderer()
	{
		Shutdown();
	}

	bool Dx12Renderer::Initialize(const RendererDesc& desc)
	{
		if (m_Initialized) Shutdown();

		HWND hWnd = reinterpret_cast<HWND>(desc.nWindow);
		UINT width = desc.outputWidth;
		UINT height = desc.outputHeight;

		if (width == 0 || height == 0)
		{
			RECT rc = {};
			GetClientRect(hWnd, &rc);

			width = rc.right;
			height = rc.bottom;
		}

#ifdef _DEBUG
		EnableDebugLayer(true);
#endif

		HRESULT hr = S_OK;
		{
			UINT flags = 0;
#ifdef _DEBUG
			flags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

			hr = CreateDXGIFactory2(flags, IID_PPV_ARGS(&m_Factory));
			if (FAILED(hr)) return false;
		}

		for (UINT i = 0; 
			m_Factory->EnumAdapterByGpuPreference(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&m_Adapter)) != DXGI_ERROR_NOT_FOUND;
			i++)
		{
			DXGI_ADAPTER_DESC1 aDesc;
			m_Adapter->GetDesc1(&aDesc);

			if (aDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
			
			hr = D3D12CreateDevice(m_Adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device));
			if (SUCCEEDED(hr)) break;
		}
		if (!m_Device) return false;

		{
			D3D12_COMMAND_QUEUE_DESC cqDesc = {};
			cqDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			cqDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
			cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			cqDesc.NodeMask = 0;

			hr = m_Device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_DirectQueue));
			if (FAILED(hr)) return false;
		}

		{
			ComPtr<IDXGISwapChain1> swapchain;
			DXGI_SWAP_CHAIN_DESC1 scDesc = {};
			scDesc.Width = width;
			scDesc.Height = height;
			scDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			scDesc.Stereo = FALSE;
			scDesc.SampleDesc.Count = 1;
			scDesc.SampleDesc.Quality = 0;
			scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
			scDesc.BufferCount = FRAME_COUNT;
			scDesc.Scaling = DXGI_SCALING_STRETCH;
			scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
			scDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
			scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

			hr = m_Factory->CreateSwapChainForHwnd(
				m_DirectQueue.Get(),
				hWnd,
				&scDesc, nullptr, nullptr, &swapchain);
			if (FAILED(hr)) return false;

			hr = swapchain->QueryInterface(IID_PPV_ARGS(&m_SwapChain));
			if (FAILED(hr)) return false;

			hr = m_Factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER);
			if (FAILED(hr)) return false;
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC dhDesc = {};
			dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			dhDesc.NumDescriptors = FRAME_COUNT;
			dhDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			dhDesc.NodeMask = 0;

			hr = m_Device->CreateDescriptorHeap(&dhDesc, IID_PPV_ARGS(&m_RTVHeap));
			if (FAILED(hr)) return false;

			m_RTVSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			if (!CreateRenderTargets(width, height)) return false;
		}

		{
			D3D12_DESCRIPTOR_HEAP_DESC dhDesc = {};
    		dhDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    		dhDesc.NumDescriptors = 1;
    		dhDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    		dhDesc.NodeMask = 0;

			hr = m_Device->CreateDescriptorHeap(&dhDesc, IID_PPV_ARGS(&m_DSVHeap));
			if (FAILED(hr)) return false;

			m_DSVSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
			if (!CreateDepthBuffer(width, height)) return false;
		}

		for (UINT i = 0; i < FRAME_COUNT; i++)
		{
			hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_Allocators[i]));
			if (FAILED(hr)) return false;
		}
		hr = m_Device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT, 
			m_Allocators[0].Get(),
			nullptr,
			IID_PPV_ARGS(&m_List)
		);
		if (FAILED(hr)) return false;
		m_List->Close();

		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence));
		if (FAILED(hr)) return false;

		{
			D3D12_COMMAND_QUEUE_DESC cqDesc = {};
			cqDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
			cqDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
			cqDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			cqDesc.NodeMask = 0;

			hr = m_Device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&m_CopyQueue));
			if (FAILED(hr)) return false;

			hr = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&m_CopyAllocator));
			if (FAILED(hr)) return false;

			hr = m_Device->CreateCommandList(
				0,
				D3D12_COMMAND_LIST_TYPE_COPY, 
				m_CopyAllocator.Get(),
				nullptr,
				IID_PPV_ARGS(&m_CopyList)
			);
			if (FAILED(hr)) return false;
			
			m_CopyFenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
			hr = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_CopyFence));
			if (FAILED(hr)) return false;
		}

		m_Desc.nWindow = desc.nWindow;
		m_Desc.nDisplay = desc.nDisplay;
		m_Desc.outputWidth = width;
		m_Desc.outputHeight = height;
		m_Desc.viewport = desc.viewport;

		m_Viewport.TopLeftX = desc.viewport.x;
		m_Viewport.TopLeftY = desc.viewport.y;
		m_Viewport.Width = desc.viewport.width;
		m_Viewport.Height = desc.viewport.height;

		m_Initialized = true;
		return true;
	}

	void Dx12Renderer::Shutdown()
	{
		if (m_CopyQueue && m_CopyFence && m_CopyFenceEvent)
		{
		    const UINT64 value = ++m_CopyFenceValue;
		    m_CopyQueue->Signal(m_CopyFence.Get(), value);
		
		    if (m_CopyFence->GetCompletedValue() < value)
		    {
		        m_CopyFence->SetEventOnCompletion(value, m_CopyFenceEvent);
		        WaitForSingleObject(m_CopyFenceEvent, INFINITE);
		    }
		}

		if (m_CopyFenceEvent)
		{
		    CloseHandle(m_CopyFenceEvent);
		    m_CopyFenceEvent = nullptr;
		}

		if (m_DirectQueue && m_Fence && m_FenceEvent)
		{
			const UINT64 value = ++m_FenceValue;
			m_DirectQueue->Signal(m_Fence.Get(), value);

			if (m_Fence->GetCompletedValue() < value)
			{
				m_Fence->SetEventOnCompletion(value, m_FenceEvent);
				WaitForSingleObject(m_FenceEvent, INFINITE);
			}
		}

    	if (m_FenceEvent)
    	{
    	    CloseHandle(m_FenceEvent);
    	    m_FenceEvent = nullptr;
    	}

    	m_Fence.Reset();
    	m_List.Reset();

    	for (UINT i = 0; i < FRAME_COUNT; i++)
    	    m_Allocators[i].Reset();

    	m_DepthBuffer.Reset();
    	m_DSVHeap.Reset();

    	for (UINT i = 0; i < FRAME_COUNT; i++)
    	    m_RenderTargets[i].Reset();

		m_StagingBuffers.clear();
		m_CopyFence.Reset();
		m_CopyList.Reset();
		m_CopyAllocator.Reset();
		m_CopyQueue.Reset();

    	m_RTVHeap.Reset();
    	m_SwapChain.Reset();
    	m_DirectQueue.Reset();
    	m_Device.Reset();
    	m_Adapter.Reset();
    	m_Factory.Reset();

    	m_Initialized = false;
	}

	const RendererDesc& Dx12Renderer::GetDesc() const
	{
		return m_Desc;
	}

	void Dx12Renderer::BeginFrame(const FrameData& data)
	{
		HRESULT hr = S_OK;
		const UINT64 completedValue = m_Fence->GetCompletedValue();
		if (completedValue < m_FenceValues[m_FrameIndex])
		{
			hr = m_Fence->SetEventOnCompletion(m_FenceValues[m_FrameIndex], m_FenceEvent);
			//if (FAILED(hr)) return;

			WaitForSingleObject(m_FenceEvent, INFINITE);
		}

		hr = m_Allocators[m_FrameIndex]->Reset();
		//if (FAILED(hr)) return;

		hr = m_List->Reset(m_Allocators[m_FrameIndex].Get(), nullptr);
		//if (FAILED(hr)) return;

		D3D12_RESOURCE_BARRIER barrier = {};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Transition.pResource = m_RenderTargets[m_FrameIndex].Get();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		m_List->ResourceBarrier(1, &barrier);

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
		rtvHandle.ptr += m_FrameIndex * m_RTVSize;

		m_List->ClearRenderTargetView(rtvHandle, m_ClearColor.data, 0, nullptr);

		D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
		m_List->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		m_List->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);

		m_List->RSSetViewports(1, &m_Viewport);
		m_List->RSSetScissorRects(1, &m_Scissor);
	}

	void Dx12Renderer::Submit(const DrawCommand& command)
	{
	}

	void Dx12Renderer::EndFrame()
	{
		HRESULT hr = S_OK;
    	D3D12_RESOURCE_BARRIER barrier = {};
    	barrier.Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
    	barrier.Transition.pResource   = m_RenderTargets[m_FrameIndex].Get();
    	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
    	barrier.Transition.StateAfter  = D3D12_RESOURCE_STATE_PRESENT;
    	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
    	m_List->ResourceBarrier(1, &barrier);
		
		m_List->Close();
		m_DirectQueue->ExecuteCommandLists(1, (ID3D12CommandList**)(m_List.GetAddressOf()));

		m_FenceValue++;
		m_FenceValues[m_FrameIndex] = m_FenceValue;
		hr = m_DirectQueue->Signal(m_Fence.Get(), m_FenceValue);
		//if (FAILED(hr)) return;

		hr = m_SwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING);
		//if (FAILED(hr)) return;

		m_FrameIndex = (m_FrameIndex + 1) % FRAME_COUNT;
	}

bool Dx12Renderer::SetOutputSize(uint32_t width, uint32_t height)
{
    if (width == m_Desc.outputWidth && height == m_Desc.outputHeight)
        return true;

	HRESULT hr = S_OK;
    const UINT64 value = ++m_FenceValue;

	hr = m_DirectQueue->Signal(m_Fence.Get(), value);
	if (FAILED(hr)) return false;

    if (m_Fence->GetCompletedValue() < value)
    {
		hr = m_Fence->SetEventOnCompletion(value, m_FenceEvent);
        if (FAILED(hr)) return false;

        WaitForSingleObject(m_FenceEvent, INFINITE);
    }

    for (int i = 0; i < FRAME_COUNT; i++)
    {
        m_RenderTargets[i].Reset();
        m_FenceValues[i] = value;
    }

    DXGI_SWAP_CHAIN_DESC desc = {};
    m_SwapChain->GetDesc(&desc);
	hr = m_SwapChain->ResizeBuffers(FRAME_COUNT, width, height, desc.BufferDesc.Format, desc.Flags);
    if (FAILED(hr)) return false;

	if (!CreateRenderTargets(width, height))
		return false;

	if (!CreateDepthBuffer(width, height))
		return false;

    m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
	m_Desc.outputWidth = width;
	m_Desc.outputHeight = height;
    m_Scissor = { 0, 0, (LONG)width,  (LONG)height };

    return true;
}

	void Dx12Renderer::SetViewport(const Viewport& viewport)
	{
		m_Desc.viewport = viewport;

		m_Viewport.TopLeftX = viewport.x;
		m_Viewport.TopLeftY = viewport.y;
		m_Viewport.Width = viewport.width;
		m_Viewport.Height = viewport.height;
	}

	void Dx12Renderer::SetClearColor(const Math::Vec4& color)
	{
		m_ClearColor = color;
	}

	MeshHandle Dx12Renderer::LoadMesh(const MeshData& mesh)
	{
		MeshResource resource = {};

	    uint64_t vbSize = (uint64_t)mesh.vertexSize;
	    uint64_t ibSize = (uint64_t)mesh.indexCount * sizeof(uint32_t);

	    if (!UploadBuffer(mesh.vertices, vbSize, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &resource.VertexBuffer))
	        return MeshHandle();

	    if (!UploadBuffer(mesh.indices, ibSize, D3D12_RESOURCE_STATE_INDEX_BUFFER, &resource.IndexBuffer))
	        return MeshHandle();

	    FlushCopyQueue();

	    resource.IndexCount = mesh.indexCount;

	    return m_MeshStorage.Add(std::move(resource));
	}

	ShaderHandle Dx12Renderer::LoadShader(const ShaderData& shader)
	{
		return ShaderHandle();
	}

    bool Dx12Renderer::EnableDebugLayer(bool enable)
    {
		HRESULT hr = S_OK;
		ComPtr<ID3D12Debug6> debug;
		if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debug))))
		{
			enable ? debug->EnableDebugLayer() : debug->DisableDebugLayer();
			return true;
		}
        return false;
    }

    bool Dx12Renderer::CreateRenderTargets(uint32_t width, uint32_t height)
    {
		for (UINT i = 0; i < FRAME_COUNT; i++)
			m_RenderTargets[i].Reset();

		D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
		for (UINT i = 0; i < FRAME_COUNT; i++)
    	{
    	    HRESULT hr = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i]));
    	    if (FAILED(hr)) return false;
		
    	    m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, rtvHandle);
    	    rtvHandle.ptr += m_RTVSize;
    	}

        return true;
    }

    bool Dx12Renderer::CreateDepthBuffer(uint32_t width, uint32_t height)
    {
    	m_DepthBuffer.Reset();
	
    	D3D12_RESOURCE_DESC desc = {};
    	desc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    	desc.Width = width;
    	desc.Height = height;
    	desc.DepthOrArraySize = 1;
    	desc.MipLevels = 1;
    	desc.Format = DXGI_FORMAT_D32_FLOAT;
    	desc.SampleDesc.Count = 1;
    	desc.SampleDesc.Quality = 0;
    	desc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
	
    	D3D12_CLEAR_VALUE clearVal = {};
    	clearVal.Format = DXGI_FORMAT_D32_FLOAT;
    	clearVal.DepthStencil.Depth = 1.0f;
    	clearVal.DepthStencil.Stencil = 0;
	
		D3D12_HEAP_PROPERTIES heapProps = {};
		heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapProps.CreationNodeMask = 1;
		heapProps.VisibleNodeMask = 1;
	
    	HRESULT hr = m_Device->CreateCommittedResource(
    	    &heapProps,
    	    D3D12_HEAP_FLAG_NONE,
    	    &desc,
    	    D3D12_RESOURCE_STATE_DEPTH_WRITE,
    	    &clearVal,
    	    IID_PPV_ARGS(&m_DepthBuffer)
    	);
    	if (FAILED(hr)) return false;
	
    	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    	dsvDesc.Format        = DXGI_FORMAT_D32_FLOAT;
    	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    	dsvDesc.Flags         = D3D12_DSV_FLAG_NONE;
	
    	m_Device->CreateDepthStencilView(
    	    m_DepthBuffer.Get(),
    	    &dsvDesc,
    	    m_DSVHeap->GetCPUDescriptorHandleForHeapStart()
    	);
	
    	return true;
	}

    void Dx12Renderer::FlushCopyQueue()
    {
		m_CopyList->Close();
    	m_CopyQueue->ExecuteCommandLists(1, (ID3D12CommandList**)m_CopyList.GetAddressOf());

    	const UINT64 value = ++m_CopyFenceValue;
    	m_CopyQueue->Signal(m_CopyFence.Get(), value);
    	m_CopyFence->SetEventOnCompletion(value, m_CopyFenceEvent);
    	WaitForSingleObject(m_CopyFenceEvent, INFINITE);

    	m_StagingBuffers.clear();

    	m_CopyAllocator->Reset();
    	m_CopyList->Reset(m_CopyAllocator.Get(), nullptr);
    }

    bool Dx12Renderer::UploadBuffer(const void *data, uint64_t size, D3D12_RESOURCE_STATES finalSate, ID3D12Resource **outResource)
    {
		D3D12_HEAP_PROPERTIES defaultHeap = {};
    	defaultHeap.Type = D3D12_HEAP_TYPE_DEFAULT;
    	defaultHeap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    	defaultHeap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    	defaultHeap.CreationNodeMask = 1;
    	defaultHeap.VisibleNodeMask = 1;

    	D3D12_RESOURCE_DESC bufDesc = {};
    	bufDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
    	bufDesc.Width = size;
    	bufDesc.Height = 1;
    	bufDesc.DepthOrArraySize = 1;
    	bufDesc.MipLevels = 1;
    	bufDesc.Format = DXGI_FORMAT_UNKNOWN;
    	bufDesc.SampleDesc.Count = 1;
    	bufDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
    	bufDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

    	ComPtr<ID3D12Resource> gpuBuffer;
    	HRESULT hr = m_Device->CreateCommittedResource(
    	    &defaultHeap, D3D12_HEAP_FLAG_NONE,
    	    &bufDesc, D3D12_RESOURCE_STATE_COMMON,
    	    nullptr, IID_PPV_ARGS(&gpuBuffer)
    	);
    	if (FAILED(hr)) return false;

    	// Staging буфер
    	D3D12_HEAP_PROPERTIES uploadHeap = {};
    	uploadHeap.Type = D3D12_HEAP_TYPE_UPLOAD;
    	uploadHeap.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
    	uploadHeap.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
    	uploadHeap.CreationNodeMask = 1;
    	uploadHeap.VisibleNodeMask = 1;

    	ComPtr<ID3D12Resource> uploadBuffer;
    	hr = m_Device->CreateCommittedResource(
    	    &uploadHeap, D3D12_HEAP_FLAG_NONE,
    	    &bufDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
    	    nullptr, IID_PPV_ARGS(&uploadBuffer)
    	);
    	if (FAILED(hr)) return false;

    	void* mapped = nullptr;
    	uploadBuffer->Map(0, nullptr, &mapped);
    	memcpy(mapped, data, size);
    	uploadBuffer->Unmap(0, nullptr);

    	m_CopyList->CopyResource(gpuBuffer.Get(), uploadBuffer.Get());

    	m_StagingBuffers.push_back(uploadBuffer);

    	*outResource = gpuBuffer.Detach();
    	return true;
    }

    Rynox::IRenderer* CreateRenderer()
    {
        return new Dx12Renderer();
    }

    void DestroyRenderer(Rynox::IRenderer *renderer)
    {
        if (renderer) delete renderer;
    }
}