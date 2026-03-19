#pragma once

#include <cstdint>

#include <Dx12VertexBuffer.h>
#include <Dx12IndexBuffer.h>
#include <Dx12Shader.h>

namespace Rynox::DirectX12
{
	struct Dx12Mesh
	{
		Dx12VertexBuffer VertexBuffer;
		Dx12IndexBuffer IndexBuffer;
		std::vector<D3D12_INPUT_ELEMENT_DESC> Layout;
		uint32_t gen;

		void Bind(ID3D12GraphicsCommandList* cmdList, UINT slot = 0u) const
		{
			VertexBuffer.Bind(cmdList, slot);
			IndexBuffer.Bind(cmdList);
		}
	};

	struct Dx12ShaderProgram
	{
		Dx12Shader VertexShader;
		Dx12Shader PixelShader;
		ComPtr<ID3D12PipelineState> Pipeline;
		uint32_t gen;
	};
}
