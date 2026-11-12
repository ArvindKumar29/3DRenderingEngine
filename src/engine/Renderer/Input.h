#pragma once
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <map>

namespace VKRENDERER
{
	struct Input
	{
	public:
		void Init(GLFWwindow* window);
		bool isKeyDown(int keycode) ;
		bool isKeyHeld(int keycode) ;
		bool isMouseButtonDown(int button) ;
		bool isMouseButtonHeld(int button) ;
		void resetMousePosition();

		void updateKeyStates();

		glm::vec2 getMousePosition();
		glm::vec2 isMouseMoved();
		
		bool firstMouseMovemenet = true;
		std::map<int, bool> currentKeyStates;
		std::map<int, bool> currentMouseStates;
		std::map<int, bool> previousKeyStates;
		std::map<int, bool> previousMouseStates;
		GLFWwindow* m_window;
		double mouseX, mouseY;
		glm::vec2 lastMousePos, currentMousePos;
	};
}