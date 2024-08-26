#include <Alalba.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

class Sandbox : public Alalba::Application
{
public:
	Sandbox()
	{
		
	}
	~Sandbox()
	{
		
	}

	virtual void OnUpdate() override
	{
		m_scene->OnUpdate();
		m_renderer->DrawFrame(*m_scene);
	}

	virtual void OnInit() override
	{
		LOG_INFO("Hello from Alalba OnInit");

		//m_meshSys.reset(new Alalba::MeshSys());
		m_scene.reset(new Alalba::Scene());
		//m_textureSys.reset(new Alalba::TextureSys());
		m_gltfLoader.reset(new Alalba::GLTFLoader());

		m_gltfLoader->LoadModel("models/glTF/buster_drone/busterDrone.gltf")
			.LoadModel("models/glTF/vulkanscene_shadow.gltf");

		//m_meshSys->LoadMesh("models/room.obj").LoadMesh("models/teapot.obj").LoadMesh("models/cube.obj").LoadMesh("models/quad.obj");;
		//m_textureSys->LoadTexture("textures/awesomeface.png").LoadTexture("textures/room.png")
		//	.LoadTexture("textures/lion.png").LoadTexture("textures/white.png");

		//auto teapot = m_scene->CreateEntity("teapot");
		//teapot.AddComponent<MeshComponent>(m_meshSys->GetMesh("teapot"));
		//teapot.AddComponent<TextureComponent>(m_textureSys->GetTexture("lion"));
		//teapot.AddOrReplaceComponent<TransformComponent>(glm::mat4(1.0f));

		//auto teapot2 = m_scene->CreateEntity("teapot2");
		//teapot2.AddComponent<MeshComponent>(m_meshSys->GetMesh("teapot"));
		//teapot2.AddComponent<TextureComponent>(m_textureSys->GetTexture("awesomeface"));
		//glm::mat4 trans = glm::mat4(1.0f);
		//trans = glm::translate(trans, glm::vec3(3.0f, 0.0f, 0.0f));
		//teapot2.AddOrReplaceComponent<TransformComponent>(trans);

		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(3.0f, 0.0f, 0.0f));
		auto drone = m_scene->CreateEntity("busterDrone");
		drone.AddComponent<GLTFComponent>(m_gltfLoader->GetModel("busterDrone"));
		drone.AddOrReplaceComponent<TransformComponent>(glm::mat4(1.0f));

		//auto drone2 = m_scene->CreateEntity("busterDrone2");
		//drone2.AddComponent<GLTFComponent>(m_gltfLoader->GetModel("busterDrone"));
		//drone2.AddOrReplaceComponent<TransformComponent>(trans);
		
		//auto vulkanscene_shadow = m_scene->CreateEntity("vulkanscene_shadow");
		//vulkanscene_shadow.AddComponent<GLTFComponent>(m_gltfLoader->GetModel("vulkanscene_shadow"));
		//vulkanscene_shadow.AddOrReplaceComponent<TransformComponent>(glm::mat4(1.0f));


	
		glm::vec4 lightpos = glm::vec4(4.0, 5.0, 2, 1.0);
		glm::vec4 lightcolor = glm::vec4(1.0, 1.0, 1.0, 1.0);
		auto pointLight = m_scene->CreateEntity("pointLight0");
		pointLight.AddComponent<PointLightComponent>(lightpos, lightcolor);
	
		auto cam = m_scene->CreateEntity("camera");
		cam.AddComponent<CamComponent>(glm::perspective(glm::radians(45.0f), 1024.0f / 720.f, 0.1f, 100.0f));

		// Render creation should be after scene
		m_renderer.reset(new Alalba::Renderer(*m_scene));
		/// record command buffer
		//m_renderer->PrepareCommandBuffers();

		//m_UI.reset(new Alalba::UI(*m_renderer, Alalba::Application::Get().GetWindow()));
	}

	virtual void OnShutdown() override
	{
		// should shutdown renderer first, as mesh and textures are being used by renderer
		Alalba::Application::Get().GetDevice().WaitIdle();

		//m_meshSys->Clean();
		//m_textureSys->Clean();
		

		m_renderer->Shutdown();
		m_gltfLoader->Clean();

		Alalba::Application::OnShutdown();
	}

	virtual void DesignUI(UIOverlay* overlay) override
	{
		ImGui::ShowDemoWindow();
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
	return new Sandbox();
}