#include "pch.h"
#include "Camera.h"

#include "Alalba_VK/Core/Input.h"

#include <glfw/glfw3.h>
#include <glm/gtc/quaternion.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#define M_PI 3.14159f

namespace Alalba {

	Camera::Camera(const glm::mat4& projectionMatrix)
		: m_ProjectionMatrix(projectionMatrix)
	{
		m_ProjectionMatrix[1][1] *= -1;
		
		m_PanSpeed = 0.0015f;
		m_RotationSpeed = 0.003f;
		m_ZoomSpeed = 1.0f;
		
		m_Position = { 0, 0, -2 };
		m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);

		m_FocalPoint = glm::vec3(0.0f);
		m_Distance = glm::distance(m_Position, m_FocalPoint);

		m_Yaw = 0.0f;
		m_Pitch = 0.0f;

	}

	Camera::Camera(const glm::vec3& from, const glm::vec3& at, const glm::vec3& up)
	{
		// Sensible defaults
		m_PanSpeed = 0.0015f;
		m_RotationSpeed = 0.002f;
		m_ZoomSpeed = 0.2f;


		m_Rotation = glm::vec3(0.0f, 0.0f, 0.0f);

		m_Position = from;
		m_FocalPoint = at;
		m_Up = up;
		m_Distance = glm::distance(m_Position, m_FocalPoint);

		m_Yaw = M_PI;
		m_Pitch = 0;
	}

	void Camera::Focus()
	{
	}

	void Camera::Update()
	{
		if (Input::IsKeyPressed(GLFW_KEY_LEFT_ALT))
		{
			m_Changed = true;
			const glm::vec2& mouse{ Input::GetMouseX(), Input::GetMouseY() };
			glm::vec2 delta = mouse - m_InitialMousePosition;
			m_InitialMousePosition = mouse;

			if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_MIDDLE))
				MousePan(delta);
			else if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_LEFT))
				MouseRotate(delta);
			else if (Input::IsMouseButtonPressed(GLFW_MOUSE_BUTTON_RIGHT))
				MouseZoom(delta.y);
		}

		m_Position = CalculatePosition();

		glm::quat orientation = GetOrientation();

		m_Rotation = glm::eulerAngles(orientation) * (180.0f / (float)M_PI);
		m_ViewMatrix = glm::toMat4(glm::conjugate(orientation)) * glm::translate(glm::mat4(1.0f), -m_Position);

		// T*R*S
		//m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
		//m_ViewMatrix = glm::inverse(m_ViewMatrix);

	}

	void Camera::MousePan(const glm::vec2& delta)
	{
		m_FocalPoint += -GetRightDirection() * delta.x * m_PanSpeed * m_Distance;
		m_FocalPoint += GetUpDirection() * delta.y * m_PanSpeed * m_Distance;
	}

	void Camera::MouseRotate(const glm::vec2& delta)
	{
		float yawSign = GetUpDirection().y < 0 ? -1.0f : 1.0f;
		m_Yaw += yawSign * delta.x * m_RotationSpeed;
		m_Pitch += delta.y * m_RotationSpeed;
	}

	void Camera::MouseZoom(float delta)
	{
		m_Distance -= delta * m_ZoomSpeed;
		if (m_Distance < 1.0f)
		{
			m_FocalPoint += GetForwardDirection();
			m_Distance = 1.0f;
		}
	}

	glm::vec3 Camera::GetUpDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	glm::vec3 Camera::GetRightDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
	}

	glm::vec3 Camera::GetForwardDirection()
	{
		return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
	}

	glm::vec3 Camera::CalculatePosition()
	{
		return m_FocalPoint - GetForwardDirection() * m_Distance;
	}

	glm::quat Camera::GetOrientation()
	{
		return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
	}

}


