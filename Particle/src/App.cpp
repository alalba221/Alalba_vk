#include <Alalba.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

class ParticleApp : public Alalba::Application
{
public:
	ParticleApp()
	{
		m_particleSys.reset(new Alalba::ParticalSystem());
	}
	~ParticleApp()
	{
		
	}

	virtual void OnUpdate() override
	{

		//m_renderer->DrawFrame(*m_scene);
	}

	virtual void OnInit() override
	{
		ALALBA_INFO("Hello from Particle OnInit");


	}

	virtual void OnShutdown() override
	{
		// should shutdown renderer first, as mesh and textures are being used by renderer
		Alalba::Application::Get().GetDevice().WaitIdle();

		m_particleSys->ShutDown();

		Alalba::Application::OnShutdown();
	}

private:

	std::unique_ptr < Alalba::ParticalSystem> m_particleSys;
};	


Alalba::Application* Alalba::CreateApplication()
{
	return new ParticleApp();
}