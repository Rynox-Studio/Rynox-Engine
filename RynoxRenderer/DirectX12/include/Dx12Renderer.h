#pragma once

#include <vector>

#include <DirectX12.h>
#include <Dx12DataTypes.h>

#include <Internal/Dx12Device.h>
#include <Internal/Dx12CommandQueue.h>
#include <Internal/Dx12SwapChain.h>
#include <Internal/Dx12UploadContext.h>
#include <Internal/Dx12CommandList.h>

#include <Core/Graphics/IRenderer.h>

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

		void BeginFrame(const Graphics::FrameData& data) override;
		void Submit(const Graphics::DrawCommand& command) override;
		void EndFrame() override;

		bool SetOutputSize(uint32_t width, uint32_t height) override;
		void SetViewport(const Viewport& viewport) override;
		void SetClearColor(const Math::Vec4& color) override;

		Graphics::MeshHandle LoadMesh(const Graphics::MeshData& mesh) override;
		Graphics::ShaderHandle LoadShader(const Graphics::ShaderData& shader) override;

	private:
		RendererDesc m_Desc;
		bool m_Initialized = false;

		Dx12Device m_Device;
		Dx12CommandQueue m_Queue;
		Dx12SwapChain m_SwapChain;
		Dx12UploadContext m_Upload;
		Dx12CommandList m_CmdList;

		UINT64 m_FrameValues[BUFFER_COUNT]{};

		// Pipeline
		ComPtr<ID3D12RootSignature> m_RootSignature;

		// Render state
		D3D12_VIEWPORT m_Viewport{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f };
		D3D12_RECT m_Scissor{};
		Math::Vec4 m_ClearColor{ 0.0f, 0.0f, 0.0f, 1.0f };

		// Resource storage
		std::vector<Dx12Mesh> m_Meshes;
		std::vector<uint32_t> m_FreeMeshes;

		std::vector<Dx12ShaderProgram> m_ShaderPrograms;
		std::vector<uint32_t> m_FreePrograms;
	};

	extern "C"
	{
		RNX_RENDERER_API IRenderer* CreateRenderer();
		RNX_RENDERER_API void DestroyRenderer(IRenderer* renderer);
	}
}