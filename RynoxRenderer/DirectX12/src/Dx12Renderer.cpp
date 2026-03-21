#include <Dx12Renderer.h>

#include <Common/Assert.h>
#include <Math/Mat4.h>

namespace Rynox::DirectX12
{
	Dx12Renderer::~Dx12Renderer()
	{
		if (m_Initialized)
		{
			Shutdown();
			m_Initialized = false;
		}
	}

	bool Dx12Renderer::Initialize(const RendererDesc& desc)
	{
		RNX_ASSERT(!m_Initialized, "Renderer already initialized. (Re-initializing...)");
		if (m_Initialized) Shutdown();
		m_Initialized = false;

		m_Desc = desc;
		{
			RECT rc;
			GetClientRect(reinterpret_cast<HWND>(m_Desc.nWindow), &rc);
			LONG width = rc.right - rc.left;
			LONG height = rc.bottom - rc.top;

			if (m_Desc.outputWidth == 0 || m_Desc.outputHeight == 0)
			{
				m_Desc.outputWidth = width;
				m_Desc.outputHeight = height;
			}

			if (m_Desc.viewport.width == 0 || m_Desc.viewport.height == 0)
			{
				m_Desc.viewport.width = width;
				m_Desc.viewport.height = height;
			}
		}

		m_Scissor = { 0, 0, (LONG)m_Desc.outputWidth, (LONG)m_Desc.outputHeight };
		m_Viewport.Width = (float)m_Desc.viewport.width;
		m_Viewport.Height = (float)m_Desc.viewport.height;

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

		if (!m_Device.Initialize()) return false;
		if (!m_Queue.Initialize(m_Device)) return false;
		if (!m_SwapChain.Initialize(
			m_Device, m_Device,
			m_Queue,
			reinterpret_cast<HWND>(m_Desc.nWindow),
			m_Desc.outputWidth, m_Desc.outputHeight)) return false;

		if (!m_Upload.Initialize(m_Device)) return false;
		if (!m_CmdList.Initialize(m_Device)) return false;

		// Create Root Signature
		{
			D3D12_ROOT_PARAMETER1 params[1] = {};

			// MVP
			params[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
			params[0].Constants.ShaderRegister = 0;
			params[0].Constants.RegisterSpace = 0;
			params[0].Constants.Num32BitValues = 16;
			params[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

			D3D12_VERSIONED_ROOT_SIGNATURE_DESC rsDesc{};
			rsDesc.Version = D3D_ROOT_SIGNATURE_VERSION_1_1;
			rsDesc.Desc_1_1.NumParameters = 1;
			rsDesc.Desc_1_1.pParameters = params;
			rsDesc.Desc_1_1.Flags =
				D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
				D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

			ComPtr<ID3DBlob> serialized, errors;
			HRESULT hr = D3D12SerializeVersionedRootSignature(&rsDesc, &serialized, &errors);
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to serialize Root Signature.");
				return false;
			}

			hr = m_Device->CreateRootSignature(
				0,
				serialized->GetBufferPointer(),
				serialized->GetBufferSize(),
				IID_PPV_ARGS(&m_RootSignature)
			);
			if (FAILED(hr))
			{
				RNX_LOG_ERROR("[DirectX12] Failed to create Root Signature.");
				return false;
			}
		}

		RNX_LOG_INFO("[DirectX12] Initialized successfully.");
		m_Initialized = true;
		return true;
	}

	void Dx12Renderer::Shutdown()
	{
		m_Queue.Flush();

		m_RootSignature.Reset();

		m_CmdList.Shutdown();
		m_Upload.Shutdown();
		m_SwapChain.Shutdown();
		m_Queue.Shutdown();
		m_Device.Shutdown();

		for (UINT i = 0; i < BUFFER_COUNT; i++)
		{
			m_FrameValues[i] = 0;
		}
		m_Initialized = false;
	}

	const RendererDesc& Dx12Renderer::GetDesc() const
	{
		return m_Desc;
	}

	void Dx12Renderer::BeginFrame(const Graphics::FrameData& data)
	{
		if (!m_Initialized) return;

		m_Upload.Flush(m_Queue);
		m_Queue.WaitForValue(m_FrameValues[m_SwapChain.FrameIndex]);

		m_CmdList.Reset(m_SwapChain.FrameIndex);

		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Transition.pResource = m_SwapChain.GetCurrentBackBuffer();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		m_CmdList->ResourceBarrier(1, &barrier);

		auto rtv = m_SwapChain.GetCurrentRTV();
		auto dsv = m_SwapChain.GetDSV();

		m_CmdList->OMSetRenderTargets(1, &rtv, FALSE, &dsv);
		m_CmdList->ClearRenderTargetView(rtv, m_ClearColor.data, 0, nullptr);
		m_CmdList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
		m_CmdList->RSSetViewports(1, &m_Viewport);
		m_CmdList->RSSetScissorRects(1, &m_Scissor);
	}

	void Dx12Renderer::Submit(const Graphics::DrawCommand& command)
	{
	}

	void Dx12Renderer::EndFrame()
	{
		if (!m_Initialized) return;

		// Transition back buffer Present → RenderTarget
		D3D12_RESOURCE_BARRIER barrier{};
		barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier.Transition.pResource = m_SwapChain.GetCurrentBackBuffer();
		barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
		barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

		m_CmdList->ResourceBarrier(1, &barrier);

		m_Queue.Execute(m_CmdList);

		m_SwapChain.SwapChain->Present(0, 0);
		m_FrameValues[m_SwapChain.FrameIndex] = m_Queue.Signal();

		m_SwapChain.FrameIndex = m_SwapChain.SwapChain->GetCurrentBackBufferIndex();
	}

	bool Dx12Renderer::SetOutputSize(uint32_t width, uint32_t height)
	{
		if (!m_Initialized) return false;
		if (width == 0 || height == 0) return false;
		if (m_Desc.outputWidth == width && m_Desc.outputHeight == height) return true;

		m_Queue.Flush();

		if (!m_SwapChain.Resize(m_Device.Device.Get(), width, height))
			return false;

		m_Desc.outputWidth = width;
		m_Desc.outputHeight = height;
		m_Scissor = { 0, 0, (LONG)width, (LONG)height };

		return true;
	}

	void Dx12Renderer::SetViewport(const Viewport& viewport)
	{
		if (!m_Initialized) return;
		if (viewport.width == 0 || viewport.height == 0) return;

		m_Viewport.TopLeftX = (float)viewport.x;
		m_Viewport.TopLeftY = (float)viewport.y;
		m_Viewport.Width = (float)viewport.width;
		m_Viewport.Height = (float)viewport.height;
		m_Desc.viewport = viewport;
	}

	void Dx12Renderer::SetClearColor(const Math::Vec4& color)
	{
		if (!m_Initialized) return;
		m_ClearColor = color;
	}

	Graphics::MeshHandle Dx12Renderer::LoadMesh(const Graphics::MeshData& mesh)
	{
		if (!m_Initialized) return {};

		uint32_t id;
		if (!m_FreeMeshes.empty())
		{
			id = m_FreeMeshes.back();
			m_FreeMeshes.pop_back();
		}
		else
		{
			id = (uint32_t)m_Meshes.size();
			m_Meshes.push_back({});
			m_Meshes[id].gen = 0;
		}

		m_Meshes[id].VertexBuffer = Dx12VertexBuffer(
			m_Device.Device.Get(), m_Upload.List.Get(),
			mesh.vertices, mesh.vertexSize, mesh.layout.stride
		);
		m_Meshes[id].IndexBuffer = Dx12IndexBuffer(
			m_Device.Device.Get(), m_Upload.List.Get(),
			mesh.indices, mesh.indexCount * sizeof(uint32_t)
		);

		for (auto& element : mesh.layout.attributes)
		{
			D3D12_INPUT_ELEMENT_DESC desc{};
			desc.SemanticName = element.name.c_str();
			desc.SemanticIndex = 0;
			desc.Format = ToDXGI(element.type);
			desc.InputSlot = 0;
			desc.AlignedByteOffset = element.offset;
			desc.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA;
			desc.InstanceDataStepRate = 0;

			m_Meshes[id].Layout.push_back(desc);
		}

		m_Upload.Pending = true;

		return { id, m_Meshes[id].gen };
	}

	Graphics::ShaderHandle Dx12Renderer::LoadShader(const Graphics::ShaderData& shader)
	{
		if (!m_Initialized) return {};

		uint32_t id;
		
		if (!m_FreePrograms.empty())
		{
			id = m_FreePrograms.back();
			m_FreePrograms.pop_back();
		}
		else
		{
			id = (uint32_t)m_ShaderPrograms.size();
			m_ShaderPrograms.push_back({});
			m_ShaderPrograms[id].gen = 0;
		}

		Dx12ShaderProgram& program = m_ShaderPrograms[id];
		program.VertexShader = Dx12Shader(shader.vertCode, ShaderStage::Vertex);
		program.PixelShader = Dx12Shader(shader.fragCode, ShaderStage::Pixel);
		return { id, m_ShaderPrograms[id].gen };
	}

	//void Dx12Renderer::DrawMesh(const Graphics::MeshHandle mesh, const Graphics::ShaderHandle shader)
	//{
	//	if (!m_Initialized) return;

	//	if (mesh.id >= m_Meshes.size() || m_Meshes[mesh.id].gen != mesh.gen) return;
	//	if (shader.id >= m_ShaderPrograms.size() || m_ShaderPrograms[shader.id].gen != shader.gen) return;

	//	Dx12Mesh& dxMesh = m_Meshes[mesh.id];
	//	Dx12ShaderProgram& dxShader = m_ShaderPrograms[shader.id];

	//	if (!dxShader.Pipeline)
	//	{
	//		// Rasterizer
	//		D3D12_RASTERIZER_DESC rDesc{};
	//		rDesc.FillMode = D3D12_FILL_MODE_WIREFRAME;
	//		rDesc.CullMode = D3D12_CULL_MODE_BACK;
	//		rDesc.FrontCounterClockwise = FALSE;
	//		rDesc.DepthBias = 0;
	//		rDesc.DepthClipEnable = TRUE;
	//		rDesc.MultisampleEnable = FALSE;
	//		rDesc.AntialiasedLineEnable = FALSE;

	//		// Blend
	//		D3D12_BLEND_DESC bDesc{};
	//		bDesc.AlphaToCoverageEnable = FALSE;
	//		bDesc.IndependentBlendEnable = FALSE;
	//		bDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	//		// Depth Stencil
	//		D3D12_DEPTH_STENCIL_DESC dsDesc{};
	//		dsDesc.DepthEnable = TRUE;
	//		dsDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	//		dsDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	//		dsDesc.StencilEnable = FALSE;

	//		// PSO
	//		D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc{};
	//		psoDesc.pRootSignature = m_RootSignature.Get();

	//		psoDesc.VS = dxShader.VertexShader.GetBytecode();
	//		psoDesc.PS = dxShader.PixelShader.GetBytecode();

	//		psoDesc.InputLayout = { dxMesh.Layout.data(), (UINT)dxMesh.Layout.size() };

	//		psoDesc.RasterizerState = rDesc;

	//		psoDesc.BlendState = bDesc;
	//		psoDesc.SampleMask = UINT_MAX;

	//		psoDesc.DepthStencilState = dsDesc;

	//		psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//		psoDesc.NumRenderTargets = 1;
	//		psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	//		psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	//		psoDesc.SampleDesc.Count = 1;
	//		psoDesc.SampleDesc.Quality = 0;

	//		HRESULT hr = m_Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&dxShader.Pipeline));
	//		if (FAILED(hr))
	//		{
	//			RNX_LOG_ERROR("[DirectX12] Failed to create Pipeline State Object.");
	//		}
	//	}

	//	m_CmdList->SetPipelineState(dxShader.Pipeline.Get());
	//	m_CmdList->SetGraphicsRootSignature(m_RootSignature.Get());

	//	Math::Mat4 mvp = Math::Mat4::Identity();
	//	m_CmdList->SetGraphicsRoot32BitConstants(0, 16, &mvp, 0);
	//	m_CmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//	dxMesh.Bind(m_CmdList);
	//	m_CmdList->DrawIndexedInstanced(dxMesh.IndexBuffer.GetIndexCount(), 1, 0, 0, 0);
	//}
}
