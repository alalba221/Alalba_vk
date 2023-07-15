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
		auto viewlight = this->GetAllEntitiesWith<PointLightComponent>();
		for (auto e : viewlight)
		{
			Entity entity = { e, this };

			auto& position = entity.GetComponent<PointLightComponent>().LightPosition;
			auto& color = entity.GetComponent<PointLightComponent>().LightColor;

			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::rotate(transform, glm::radians(0.01f), glm::vec3(0.0f, 1.0f, 0.0f));
			position = transform * position;

			//// TODO: if light rotate fast there will be error, should synchronize between shadow system and gltf system
			//glm::mat4 rot = glm::mat4(1.0f);
			//rot = glm::rotate(rot, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			//position = rot * glm::vec4(1.0);

			entity.AddOrReplaceComponent<PointLightComponent>(position, color);
		}
	
	}
}
