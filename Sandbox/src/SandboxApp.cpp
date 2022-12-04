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
	virtual void OnInit() override
	{
		ALALBA_INFO("Hello from sandbox OnInit");

		m_Camera.reset(new Alalba::Camera(glm::perspectiveFov(glm::radians(40.0f), 1280.0f, 720.0f, 0.1f, 3000.0f)));


		// TODO: As a seperate system, the renderer should not be initialized in the application's OnInit () function
		Alalba::Application::OnInit();
		m_mesh.reset(new Alalba::Mesh());
	}

	virtual void OnShutdown() override
	{
		m_testTexture->Clean();
		Alalba::Texture::Allocator()->Clean();
		Alalba::Application::OnShutdown();
	}

private:

};	


Alalba::Application* Alalba::CreateApplication()
{
	return new Sandbox();
}