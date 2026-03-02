#include "Renderer/OpenGL/ResourceService.h"

namespace Rynox::Renderer::OpenGL
{
	MeshHandle ResourceService::AddMesh(OpenGLMesh mesh)
	{
		m_meshes.push_back(mesh);
		return MeshHandle(m_meshes.size() - 1);
	}
	OpenGLMesh ResourceService::GetMesh(MeshHandle handle)
	{
		return m_meshes[handle.index];
	}
}