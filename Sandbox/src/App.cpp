#include <Alalba.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

class DPA8090 : public Alalba::Application
{
public:
	DPA8090()
	{
		
	}
	~DPA8090()
	{
		
	}

	virtual void OnUpdate() override
	{

		m_scene->OnUpdate();
		m_renderer->DrawFrame(*m_scene);

		// TEST LIGHT POSITION
		auto viewlight = m_scene->GetAllEntitiesWith<PointLightComponent>();
		for (auto e : viewlight)
		{
			Entity entity = { e, m_scene.get()};

			auto& position = entity.GetComponent<PointLightComponent>().LightPosition;
			auto& color = entity.GetComponent<PointLightComponent>().LightColor;

			glm::mat4 transform = glm::mat4(1.0f);
			transform = glm::rotate(transform, glm::radians(0.01f) , glm::vec3(0.0f, 1.0f, 0.0f));
			position = transform * position;

			//// TODO: if light rotate fast there will be error, should synchronize between shadow system and gltf system
			//glm::mat4 rot = glm::mat4(1.0f);
			//rot = glm::rotate(rot, (float)glfwGetTime() * glm::radians(50.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			//position = rot * glm::vec4(1.0);

			entity.AddOrReplaceComponent<PointLightComponent>(position, color);
		}
	}

	virtual void OnInit() override
	{
		ALALBA_INFO("Hello from dpa8090 OnInit");

		m_meshSys.reset(new Alalba::MeshSys());
		m_scene.reset(new Alalba::Scene());
		m_textureSys.reset(new Alalba::TextureSys());
		m_gltfLoader.reset(new Alalba::GLTFLoader());

		m_meshSys->LoadMesh("models/room.obj").LoadMesh("models/teapot.obj").LoadMesh("models/cube.obj").LoadMesh("models/quad.obj");;
		m_textureSys->LoadTexture("textures/awesomeface.png").LoadTexture("textures/room.png")
			.LoadTexture("textures/lion.png").LoadTexture("textures/white.png");
		m_gltfLoader->LoadModel("models/glTF/buster_drone/busterDrone.gltf");
	/*	m_gltfLoader->LoadModel("models/glTF/buster_drone/busterDrone.gltf")
			.LoadModel("models/glTF/samplescene.gltf");*/

		auto teapot = m_scene->CreateEntity("teapot");
		teapot.AddComponent<MeshComponent>(m_meshSys->GetMesh("teapot"));
		teapot.AddComponent<TextureComponent>(m_textureSys->GetTexture("lion"));
		teapot.AddOrReplaceComponent<TransformComponent>(glm::mat4(1.0f));

		auto teapot2 = m_scene->CreateEntity("teapot2");
		teapot2.AddComponent<MeshComponent>(m_meshSys->GetMesh("teapot"));
		teapot2.AddComponent<TextureComponent>(m_textureSys->GetTexture("awesomeface"));
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(3.0f, 0.0f, 0.0f));
		teapot2.AddOrReplaceComponent<TransformComponent>(trans);

		auto drone = m_scene->CreateEntity("busterDrone");
		drone.AddComponent<GLTFComponent>(m_gltfLoader->GetModel("busterDrone"));
		drone.AddOrReplaceComponent<TransformComponent>(glm::mat4(1.0f));

		//auto drone2 = m_scene->CreateEntity("busterDrone2");
		//drone2.AddComponent<GLTFComponent>(m_gltfLoader->GetModel("busterDrone"));
		//drone2.AddOrReplaceComponent<TransformComponent>(trans);
	
		glm::vec4 lightpos = glm::vec4(3.0, 3.0, 3.0, 1.0);
		glm::vec4 lightcolor = glm::vec4(1.0, 1.0, 1.0, 1.0);
		auto pointLight = m_scene->CreateEntity("pointLight0");
		pointLight.AddComponent<PointLightComponent>(lightpos, lightcolor);
	
		auto cam = m_scene->CreateEntity("camera");
		cam.AddComponent<CamComponent>(glm::perspective(glm::radians(45.0f), 1024.0f / 720.f, 0.1f, 100.0f));

		// Render creation should be after scene
		m_renderer.reset(new Alalba::Renderer(*m_scene));
		/// record command buffer
		m_renderer->PrepareCommandBuffer(*m_scene);

		//m_UI.reset(new Alalba::UI(*m_renderer, Alalba::Application::Get().GetWindow()));
	}

	virtual void OnShutdown() override
	{
		// should shutdown renderer first, as mesh and textures are being used by renderer
		Alalba::Application::Get().GetDevice().WaitIdle();

		m_meshSys->Clean();
		m_textureSys->Clean();
		

		m_renderer->Shutdown();
		m_gltfLoader->Clean();

		Alalba::Application::OnShutdown();
	}

private:

	std::unique_ptr<Alalba::MeshSys> m_meshSys;
	std::unique_ptr<Alalba::TextureSys> m_textureSys;
	std::unique_ptr<Alalba::GLTFLoader> m_gltfLoader;

	std::unique_ptr<Alalba::Scene> m_scene;

	std::unique_ptr<Alalba::Renderer> m_renderer;

};	


Alalba::Application* Alalba::CreateApplication()
{
	return new DPA8090();
}