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
		 //m_mesh->test_UpdateModelMatrix();

		 m_renderer->DrawFrame(*m_mesh.get(), *m_testTexture.get(), *m_camera.get());

	}

	virtual void OnInit() override
	{
		ALALBA_INFO("Hello from dpa8090 OnInit");

		 m_camera.reset(new Alalba::Camera(glm::perspective(glm::radians(45.0f), 1024.0f /720.f , 0.1f, 10.0f)));

		 m_mesh.reset(Alalba::Model::Create("models/room.obj"));
		 m_testTexture.reset(new Alalba::Texture("textures/white.png"));
		
		 m_renderer.reset(new vk::VulkanRenderer(Alalba::Application::Get().GetDevice()));
		 m_renderer->Init("Shaders/vert.spv", "Shaders/frag.spv",false);

	}

	virtual void OnShutdown() override
	{
		// should shutdown renderer first, as mesh and textures are being used by renderer
		Alalba::Application::Get().GetDevice().WaitIdle();
		m_testTexture->Clean();
		m_mesh->Clean();
		m_renderer->Shutdown();
		Alalba::Application::OnShutdown();
	}

private:
	std::unique_ptr<vk::VulkanRenderer> m_renderer;
	std::unique_ptr<vk::VulkanComputer> m_computer;

	std::unique_ptr<Alalba::Texture> m_testTexture;
	std::unique_ptr<Alalba::Model> m_mesh;
	std::unique_ptr<Alalba::Camera> m_camera;
};	


Alalba::Application* Alalba::CreateApplication()
{
	return new DPA8090();
}