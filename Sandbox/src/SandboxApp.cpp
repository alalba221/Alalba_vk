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
		m_renderer->DrawFrame(*m_mesh.get());
	}

	virtual void OnInit() override
	{
		ALALBA_INFO("Hello from sandbox OnInit");

		m_Camera.reset(new Alalba::Camera(glm::perspectiveFov(glm::radians(40.0f), 1280.0f, 720.0f, 0.1f, 3000.0f)));

		m_mesh.reset(new Alalba::Mesh("models/room.obj"));
		m_testTexture.reset(new Alalba::Texture("textures/room.png"));
		
		m_renderer.reset(new vk::VulkanRenderer(Alalba::Application::Get().GetDevice()));
		m_renderer->Init(*m_testTexture.get());
		
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
	std::unique_ptr<Alalba::Texture> m_testTexture;
	std::unique_ptr<Alalba::Mesh> m_mesh;
};	


Alalba::Application* Alalba::CreateApplication()
{
	return new Sandbox();
}