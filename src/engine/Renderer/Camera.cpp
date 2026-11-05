#include "Camera.h"

namespace VKRENDERER
{
	Camera::Camera()
		:position(glm::vec3(0.0f, 0.0f, 10.0f)),
		front(glm::vec3(0.0f, 0.0f, -1.0f)),
		up(glm::vec3(0.0f, 1.0f, 0.0f)),
		rotation(glm::vec3(0.0f, -90.0f, 0.0f)),
		viewMatrix(glm::lookAt(position, position + front, up)),
		projectionMatrix(glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f))
	{
		projectionMatrix[1][1] *= -1.0f;
	}

	void Camera::update()
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));
		direction.y = sin(glm::radians(rotation.x));
		direction.z = sin(glm::radians(rotation.y)) * cos(glm::radians(rotation.x));

		front = glm::normalize(direction);
		up = glm::normalize(glm::cross(glm::cross(front, WORLD_UP), front));

		viewMatrix = glm::lookAt(position, position + front, up);
	}

	Camera::Camera(glm::vec3 position, glm::vec3 front, glm::vec3 up, glm::vec3 rotation)
		: position(position), front(front), up(up), rotation(rotation), 
		viewMatrix(glm::lookAt(position, position + front, up)), 
		projectionMatrix(glm::perspective(glm::radians(45.0f), 1920.0f / 1080.0f, 0.1f, 100.0f))
	{}


}