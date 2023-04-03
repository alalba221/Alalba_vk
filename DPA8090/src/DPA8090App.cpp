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
		 m_camera->Update();
		 
		 //0. update global uniform (cam and light) from camera or UI
		 uint32_t size = sizeof(glm::vec4) + sizeof(glm::mat4) * 3;
		 Alalba::UniformBufferDeclaration globalUniform(size);
		 globalUniform.Push("ubo.meaningless", m_camera->GetPosition());
		 globalUniform.Push("meaningless", glm::mat4(1.0f));
		 globalUniform.Push("ubo.meaningless", m_camera->GetViewMatrix());
		 globalUniform.Push("ubo.meaningless", m_camera->GetProjectionMatrix());
		 
		 //1. Global uniform passed to render by scene
		 m_scene->UpdateGlobalUniform(globalUniform);

		 //2. model's individual uniforms passed to render by model
		 for (auto& mod : m_scene->GetModels())
		 {

		 }

		 m_renderer->DrawFrame(*m_scene.get(), *m_camera.get(), *m_UI.get());
		 
	}

	virtual void OnInit() override
	{
		ALALBA_INFO("Hello from dpa8090 OnInit");

		m_camera.reset(new Alalba::Camera(glm::perspective(glm::radians(45.0f), 1024.0f /720.f , 0.1f, 10.0f)));
		m_scene.reset(new Alalba::Scene());
		
		m_scene->AddMesh("models/cube.obj").AddMesh("models/teapot.obj").AddTexture("textures/awesomeface.png").AddTexture("textures/white.png");
		m_scene->AddModel("TEST", "models/cube.obj", "textures/white.png").AddModel("TEST1", "models/teapot.obj", "textures/awesomeface.png");

		m_renderer.reset(new vk::VulkanRenderer(Alalba::Application::Get().GetDevice()));
		m_renderer->Init("Shaders/vert.spv", "Shaders/frag.spv",false);

		m_UI.reset(new Alalba::UI(*m_renderer.get(), Alalba::Application::Get().GetWindow()));
	}

	virtual void OnShutdown() override
	{
		// should shutdown renderer first, as mesh and textures are being used by renderer
		Alalba::Application::Get().GetDevice().WaitIdle();

		m_scene->Clean();
		m_UI->Clean();
		m_renderer->Shutdown();
		Alalba::Application::OnShutdown();
	}

private:
	std::unique_ptr<vk::VulkanRenderer> m_renderer;
	std::unique_ptr<Alalba::UI> m_UI;
	std::unique_ptr<vk::VulkanComputer> m_computer;
	
	std::unique_ptr < Alalba::Scene > m_scene;
	std::unique_ptr<Alalba::Camera> m_camera;

};	


Alalba::Application* Alalba::CreateApplication()
{
	return new DPA8090();
}