#pragma once
#include "entt.hpp"
#include "Components.h"

namespace Alalba
{
	class Entity;
	class Scene
	{
	public:
		Scene();
		~Scene();
		
		entt::registry& Reg() { return m_Registry; }
		Entity CreateEntity(const std::string& name = std::string());
		void OnUpdate();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}

	private:
		entt::registry m_Registry;

		friend class Entity;
	};
}
