#pragma once

#include <vector>

#include <Renderer/DirectX12.h>
#include <Renderer/Dx12DataTypes.h>

#include <Core/Graphics/IRenderer.h>

namespace Rynox::Renderer
{
	class Dx12Renderer final : public IRenderer
	{
	public:
		Dx12Renderer() = default;
		~Dx12Renderer();

		bool Initialize(const RendererDesc& desc);
		const RendererDesc& GetDesc() const override;

		void BeginFrame() override;
		void EndFrame() override;

		bool SetOutputSize(uint32_t width, uint32_t height) override;
		void SetViewport(Viewport viewport) override;
		void SetClearColor(Math::Vec4 color) override;

		Graphics::MeshHandle LoadMesh(const Graphics::MeshData& mesh) override;
		Graphics::ShaderHandle LoadShader(const Graphics::ShaderData& shader) override;

		void DrawMesh(const Graphics::MeshHandle mesh, const Graphics::ShaderHandle shader);

	private:
		void Cleanup();
		void FlushGPU();

	private:
		RendererDesc m_Desc;
		bool m_Initialized = false;

		ComPtr<IDXGIFactory7> m_Factory;
		ComPtr<ID3D12Device14> m_Device;

		ComPtr<IDXGIAdapter4> m_Adapter;
		ComPtr<ID3D12CommandQueue> m_DirectQueue;
		ComPtr<IDXGISwapChain4> m_SwapChain;
		ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
		ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
		ComPtr<ID3D12Resource> m_RTVs[BUFFER_COUNT];
		ComPtr<ID3D12Resource> m_DepthStencilBuffer;
		ComPtr<ID3D12CommandAllocator> m_CommandAllocator[BUFFER_COUNT];
		ComPtr<ID3D12GraphicsCommandList> m_CommandList;
		ComPtr<ID3D12Fence> m_Fence;

		UINT m_RTVDescSize{};
		UINT m_FrameIndex{};
		UINT64 m_FrameValues[BUFFER_COUNT]{};
		HANDLE m_FenceEvent = nullptr;
		D3D12_VIEWPORT m_Viewport{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f};
		Math::Vec4 m_ClearColor{ 0, 0, 0, 1 };

		std::vector<Dx12Mesh> m_Meshes;
		std::vector<uint32_t> m_FreeMeshes;
	};

	extern "C"
	{
		RNX_RENDERER_API IRenderer* CreateRenderer();
		RNX_RENDERER_API void DestroyRenderer(IRenderer* renderer);
	}
}