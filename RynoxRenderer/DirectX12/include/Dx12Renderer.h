#pragma once

#include <vector>

#include <Rendering/IRenderer.h>
#include <DirectX12.h>

#include "ResourceStorage.h"

namespace Rynox::DirectX12
{
	class Dx12Renderer final : public IRenderer
	{
	public:
		Dx12Renderer() = default;
		~Dx12Renderer();

		bool Initialize(const RendererDesc& desc) override;
		void Shutdown();

		const RendererDesc& GetDesc() const override;

		void BeginFrame(const FrameData& data) override;
		void Submit(const DrawCommand& command) override;
		void EndFrame() override;

		bool SetOutputSize(uint32_t width, uint32_t height) override;
		void SetViewport(const Viewport& viewport) override;
		void SetClearColor(const Math::Vec4& color) override;

		GeometryHandle LoadMesh(const MeshData& mesh) override;
		ShaderHandle LoadShader(const ShaderData& shader) override;

	private:
		bool EnableDebugLayer(bool enable);

		bool CreateRenderTargets(uint32_t width, uint32_t height);
		bool CreateDepthBuffer(uint32_t width, uint32_t height);

		void FlushCopyQueue();
		bool UploadBuffer(const void *data, uint64_t size, D3D12_RESOURCE_STATES finalSate, ID3D12Resource **outResource);

	private:
		RendererDesc m_Desc;
		bool m_Initialized = false;

		ComPtr<IDXGIFactory7> m_Factory;
		ComPtr<IDXGIAdapter4> m_Adapter;
		ComPtr<ID3D12Device14> m_Device;

		ComPtr<ID3D12CommandQueue> m_DirectQueue;
		ComPtr<IDXGISwapChain4> m_SwapChain;
		
		ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
		ComPtr<ID3D12Resource> m_RenderTargets[FRAME_COUNT];
		ComPtr<ID3D12CommandAllocator> m_Allocators[FRAME_COUNT];
		ComPtr<ID3D12GraphicsCommandList> m_List;
		
		ComPtr<ID3D12Resource> m_DepthBuffer;
		ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
		
		ComPtr<ID3D12Fence> m_Fence;
		UINT64 m_FenceValue = 1;
		UINT64 m_FenceValues[FRAME_COUNT] = {};
		HANDLE m_FenceEvent = nullptr;

		ComPtr<ID3D12CommandQueue> m_CopyQueue;
		ComPtr<ID3D12CommandAllocator> m_CopyAllocator;
 		ComPtr<ID3D12GraphicsCommandList> m_CopyList;

		ComPtr<ID3D12Fence> m_CopyFence;
		UINT64 m_CopyFenceValue = 0;
		HANDLE m_CopyFenceEvent = nullptr;
		std::vector<ComPtr<ID3D12Resource>> m_StagingBuffers;
 
		UINT m_RTVSize;
		UINT m_DSVSize;
		UINT m_FrameIndex = 0;
		D3D12_VIEWPORT m_Viewport = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
		D3D12_RECT m_Scissor = {};
		Math::Vec4 m_ClearColor = { 0.0f, 0.0f, 0.0f, 1.0f };

		struct MeshResource
		{
			ComPtr<ID3D12Resource> VertexBuffer;
			ComPtr<ID3D12Resource> IndexBuffer;
			UINT IndexCount;
		};

		struct ShaderResource
		{

		};

		ResourceStorage<MeshResource, GeometryTag> m_MeshStorage;
		ResourceStorage<ShaderResource, ShaderTag> m_ShaderStorage;
	};

	extern "C"
	{
		RNX_RENDERER_API Rynox::IRenderer* CreateRenderer();
		RNX_RENDERER_API void DestroyRenderer(Rynox::IRenderer* renderer);
	}
}