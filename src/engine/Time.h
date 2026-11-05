#pragma once
#include <GLFW/glfw3.h>

namespace VKRENDERER
{
	class Time
	{
		static float deltaTime;
		static float lastFrame;
		static float currentFrame;
		static float m_Time;

	public:
		static void updateDeltaTime()
		{
			currentFrame = static_cast<float>(glfwGetTime());
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;
		}

		static float getDeltaTime()
		{
			return deltaTime;
		}

		static float getLastFrame()
		{
			return lastFrame;
		}
		
		static float getSeconds() 
		{
			return m_Time;
		}
		static float getMilliseconds()
		{
			return m_Time * 1000.0f;
		}
	};
}