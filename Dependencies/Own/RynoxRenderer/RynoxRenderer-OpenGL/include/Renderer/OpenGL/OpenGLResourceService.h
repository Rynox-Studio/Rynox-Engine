#pragma once

#include <vector>

#include "Core/Graphics/RenderTypes.h"
#include "Core/Graphics/GraphicsData.h"

#include <Common/Logger.h>

#include "Renderer/OpenGL/OpenGLResourceSlot.h"
#include "Renderer/OpenGL/OpenGLResources.h"
#include "Renderer/OpenGL/OpenGLDevice.h"

using namespace Rynox::Graphics;

namespace Rynox::Renderer::OpenGL
{
	class OpenGLResourceService
	{
	public:
		template<typename Slot, typename Data, typename Handle>
		Handle QueueResource(std::vector<Slot>& slots, const Data& data)
		{
			uint32_t index = FindFreeSlot(slots);

			if (index >= slots.size())
				slots.emplace_back();

			auto& slot = slots[index];

			slot.cpuData = data;
			slot.active = false;
			slot.queued = true;
			slot.generation++;

			return Handle(index, slot.generation);
		}

		MeshHandle QueueMesh(const MeshData& meshData)
		{
			MeshHandle handle = QueueResource<
				OpenGLResourceSlot<OpenGLMesh, MeshData>,
				MeshData,
				MeshHandle
			>(m_meshes, meshData);

			m_meshQueue.push_back(handle.index);
			return handle;
		}
		ShaderHandle QueueShader(const ShaderData& shaderData)
		{
			ShaderHandle handle = QueueResource<
				OpenGLResourceSlot<OpenGLShader, ShaderData>,
				ShaderData,
				ShaderHandle
			>(m_shaders, shaderData);

			m_shaderQueue.push_back(handle.index);
			return handle;
		}
		template<typename T>
		uint32_t FindFreeSlot(const std::vector<T>& slots)
		{
			for (size_t i = 0; i < slots.size(); i++)
			{
				const T& slot = slots[i];
				if (!slot.active && !slot.queued)
					return i;
			}
			return slots.size();
		}
		void ProcessMeshes() 
		{
			for (uint32_t index : m_meshQueue)
			{
				auto& slot = m_meshes[index];
				if (!slot.cpuData.vertices)
				{
					RNX_LOG_WARNING("fail load mesh, vertices is nullptr");
					slot.queued = false;
					continue;
				}
				else if (!slot.cpuData.indices)
				{
					RNX_LOG_WARNING("fail load mesh, indices is nullptr");
					slot.queued = false;
					continue;
				}

				auto vBuffer = OpenGLDevice::CreateVertexBuffer(
					slot.cpuData.vertices,
					slot.cpuData.vertexSize,
					slot.cpuData.layout
				);
				auto iBuffer = OpenGLDevice::CreateIndexBuffer(
					slot.cpuData.indices,
					slot.cpuData.indexCount
				);
				auto aBuffer = OpenGLDevice::CreateVertexArray(vBuffer, iBuffer);

				slot.resource = OpenGLMesh{ aBuffer, vBuffer, iBuffer, slot.cpuData.indexCount };
				slot.active = true;
				slot.queued = false;
			}
			m_meshQueue.clear();
		}
		void ProcessShaders()
		{
			for (uint32_t index : m_shaderQueue)
			{
				auto& slot = m_shaders[index];
				if (!slot.cpuData.fragCode)
				{
					RNX_LOG_WARNING("fail load shader, fragCode is nullptr");
					slot.queued = false;
					continue;
				}
				else if (!slot.cpuData.vertCode)
				{
					RNX_LOG_WARNING("fail load shader, vertCode is nullptr");
					slot.queued = false;
					continue;
				}

				OpenGLShader newShader = OpenGLDevice::CreateShader(
					slot.cpuData.vertCode,
					slot.cpuData.fragCode
				);

				if (newShader.id == OpenGL::INVALID_ID)
				{
					RNX_LOG_WARNING("fail create shader Handle (index: {}, generation: {})", index, slot.generation);
					slot.queued = false;
					continue;
				}

				slot.resource = newShader;
				slot.active = true;
				slot.queued = false;
			}
			m_shaderQueue.clear();
		}
		void ProcessQueues()
		{
			if (!m_meshQueue.empty())
				ProcessMeshes();

			if(!m_shaderQueue.empty())
				ProcessShaders();
		}
	private:
		std::vector<OpenGLResourceSlot<OpenGLMesh, MeshData>> m_meshes;
		std::vector<OpenGLResourceSlot<OpenGLShader, ShaderData>> m_shaders;

		std::vector<uint32_t> m_meshQueue;
		std::vector<uint32_t> m_shaderQueue;
	};
}