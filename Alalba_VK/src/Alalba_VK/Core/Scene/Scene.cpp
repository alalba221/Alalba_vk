#include "pch.h"
#include "Scene.h"
#include "Entity.h"
// test
#include "glm/gtx/string_cast.hpp"
#include "GLFW/glfw3.h"
namespace Alalba
{
	float timer = 0.0f;
	Scene::Scene()
	{
	}

	Scene::~Scene()
	{
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = { m_Registry.create(), this };
		entity.AddComponent<TransformComponent>(glm::mat4(1.0f));
		auto& tag = entity.AddComponent<TagComponent>();
		tag.Tag = name.empty() ? "Entity" : name;
		return entity;
	}

	void Scene::OnUpdate()
	{
		timer++;
		// update camera
		auto view = this->GetAllEntitiesWith<CamComponent>();
		for (auto e : view)
		{
			Entity entity = { e, this };
			auto& camera = entity.GetComponent<CamComponent>();
			camera.m_Camera.Update();
		}
		// update other entities
			// light
		//auto lightview = this->GetAllEntitiesWith<PointLightComponent>();
		//for (auto e : lightview)
		//{
		//	Entity entity = { e, this };
		//	auto& lightPos = entity.GetComponent<PointLightComponent>().LightPosition;
		//	lightPos.x = cos(glm::radians((float)glfwGetTime() * 50.0f));
		//	lightPos.y = -50.0f + sin(glm::radians((float)glfwGetTime() * 50.0f)) ;
		//	lightPos.z = 25.0f + sin(glm::radians((float)glfwGetTime() * 50.0f)) ;
		//}
	
	}
}
