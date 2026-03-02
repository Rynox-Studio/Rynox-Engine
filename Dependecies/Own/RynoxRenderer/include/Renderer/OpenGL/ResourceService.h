#pragma once

#include <vector>
#include "Renderer/OpenGL/OpenGLMesh.h"
#include "Renderer/RenderTypes.h"

namespace Rynox::Renderer::OpenGL
{
	class ResourceService
	{
	public:
		MeshHandle AddMesh(OpenGLMesh mesh);
		OpenGLMesh GetMesh(MeshHandle handle);
	private:
		std::vector<OpenGLMesh> m_meshes;
	};
}