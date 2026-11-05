#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Time.h>

namespace VKRENDERER
{
	const glm::vec3 WORLD_UP(0.0f, 1.0f, 0.0f);

	class Camera
	{
	public:
		Camera();
		Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 rotation);

		void update();
		glm::vec3 getPosition() { return position; }
		glm::vec3 getRotation() { return rotation; }
		glm::vec3 getFront() { return front; }
		glm::mat4 getViewMatrix() { return viewMatrix; }
		glm::mat4 getProjectionMatrix() { return projectionMatrix; }
		void setViewMatrix(const glm::mat4& view) { viewMatrix = view; }
		void setViewMatrix(glm::vec3 position, glm::vec3 front, glm::vec3 up) { viewMatrix = glm::lookAt(position, position + front, up); }
		void setProjectionMatrix(const glm::mat4& projection) { projectionMatrix = projection; }
		void setProjectionMatrix(float fov, float aspectRatio, float nearPlane, float farPlane) { projectionMatrix = glm::perspective(fov, aspectRatio, nearPlane, farPlane); projectionMatrix[1][1] *= -1.0f; }
		void setPosition(const glm::vec3& pos) { position = pos; }
		void setFront(const glm::vec3& f) { front = f; }
		void setUp(const glm::vec3& u) { up = u; }
		void setRotation(const glm::vec3& rot) { rotation = rot; }

	private:
		glm::mat4 viewMatrix;
		glm::mat4 projectionMatrix;
		glm::vec3 position;
		glm::vec3 front;
		glm::vec3 up;
		glm::vec3 rotation; // rotation.x(-89.0f, 89.0f) = pitch, rotation.y = yaw, rotation.z = roll

	};
}