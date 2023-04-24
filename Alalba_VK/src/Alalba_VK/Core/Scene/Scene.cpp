#include "pch.h"
#include "Scene.h"
#include "Entity.h"
// test
#include "glm/gtx/string_cast.hpp"
namespace Alalba
{
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>();
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::OnUpdate()
	{
		// update camera
		auto view = this->GetAllEntitiesWith<CamComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& camera = entity.GetComponent<CamComponent>();
			camera.m_Camera.Update();
		}
		// update other entities

	}
}
