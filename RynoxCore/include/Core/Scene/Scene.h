#pragma once

#include <Flies/World.h>
#include <string>

namespace Rynox
{
    class Scene
    {
    public:
        Flies::World& GetWorld() { return m_world; }

        void SetActiveCamera(Flies::Entity entity) { m_activeCamera = entity; }
        Flies::Entity GetActiveCamera() const { return m_activeCamera; }

		void SetName(const std::string& name) { m_name = name; }
		std::string GetName() const { return m_name; }
    private:
        Flies::World m_world;
        Flies::Entity m_activeCamera;
        std::string m_name;
    };
}