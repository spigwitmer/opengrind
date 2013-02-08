#include "timer.h"
#include <GL/glfw3.h>
#include <sstream>

Timer::Timer() : m_start(glfwGetTime()) {}

void Timer::Touch()
{
	m_start = glfwGetTime();
}

double Timer::Ago() const
{
	return glfwGetTime() - m_start;
}
