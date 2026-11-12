#include "Input.h"

namespace VKRENDERER
{
	void Input::updateKeyStates()
	{
		previousKeyStates = currentKeyStates;
		for (const auto& [key, state] : currentKeyStates)
		{
			currentKeyStates[key] = glfwGetKey(m_window, key) == GLFW_PRESS;
		}

		previousMouseStates = currentMouseStates;
		for (const auto& [button, state] : currentMouseStates)
		{
			currentMouseStates[button] = glfwGetMouseButton(m_window, button) == GLFW_PRESS;
		}
		
		currentMousePos = getMousePosition();
	}

	void Input::Init(GLFWwindow* window)
	{
		m_window = window;
	}

	bool Input::isKeyDown(int keycode) 
	{
		currentKeyStates[keycode] = glfwGetKey(m_window, keycode) == GLFW_PRESS;
		return ((currentKeyStates[keycode] == true) && (previousKeyStates[keycode] == false));
	}

	bool Input::isKeyHeld(int keycode)
	{
		currentKeyStates[keycode] = glfwGetKey(m_window, keycode) == GLFW_PRESS;
		return currentKeyStates[keycode];
	}


	bool Input::isMouseButtonHeld(int button) 
	{
		currentMouseStates[button] = glfwGetMouseButton(m_window, button) == GLFW_PRESS;
		return currentMouseStates[button];
	}

	bool Input::isMouseButtonDown(int button) 
	{
		currentMouseStates[button] = glfwGetMouseButton(m_window, button) == GLFW_PRESS;
		return (currentMouseStates[button] == true) && (previousMouseStates[button] == false);
	}

	glm::vec2 Input::getMousePosition()
	{
		glfwGetCursorPos(m_window, &mouseX, &mouseY);
		return glm::vec2(mouseX, mouseY);
	}

	glm::vec2 Input::isMouseMoved()
	{
		if(firstMouseMovemenet)
		{
			lastMousePos = getMousePosition();
			firstMouseMovemenet = false;
			return glm::vec2(0.0f, 0.0f);
		}
		if (lastMousePos != currentMousePos)
		{
			glm::vec2 diff = currentMousePos - lastMousePos;
			lastMousePos = currentMousePos;
			return diff;
		}
		return glm::vec2(0.0f, 0.0f);
	}

	void Input::resetMousePosition()
	{
		lastMousePos = getMousePosition();
	}
}
