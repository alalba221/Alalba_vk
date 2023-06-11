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
		//m_gltfSys->LoadModel("models/glTF/buster_drone/busterDrone.gltf");
		m_gltfLoader->LoadModel("models/glTF/FlightHelmet/FlightHelmet.gltf")
			.LoadModel("models/glTF/buster_drone/busterDrone.gltf");


		auto teapot = m_scene->CreateEntity("teapot");
		teapot.AddComponent<MeshComponent>(m_meshSys->GetMesh("teapot"));
		teapot.AddComponent<TextureComponent>(m_textureSys->GetTexture("lion"));
		teapot.AddOrReplaceComponent<TransformComponent>(glm::mat4(1.0f));

		auto teapot2 = m_scene->CreateEntity("teapot2");
		teapot2.AddComponent<MeshComponent>(m_meshSys->GetMesh("teapot"));
		teapot2.AddComponent<TextureComponent>(m_textureSys->GetTexture("awesomeface"));
		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(1.5f, -1.5f, 2.0f));
		teapot2.AddOrReplaceComponent<TransformComponent>(trans);

		auto drone = m_scene->CreateEntity("busterDrone");
		drone.AddComponent<GLTFComponent>(m_gltfLoader->GetModel("busterDrone"));
		drone.AddOrReplaceComponent<TransformComponent>(glm::mat4(1.0f));

		auto helmet = m_scene->CreateEntity("FlightHelmet");
		helmet.AddComponent<GLTFComponent>(m_gltfLoader->GetModel("FlightHelmet"));
		helmet.AddOrReplaceComponent<TransformComponent>(trans);
	
		auto cam = m_scene->CreateEntity("camera");
		cam.AddComponent<CamComponent>(glm::perspective(glm::radians(45.0f), 1024.0f / 720.f, 0.1f, 10.0f));

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