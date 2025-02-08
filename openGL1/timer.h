#pragma once
#include <GLFW/glfw3.h>
class Timer {
private: 
	float prev;
	float delta;
public:
	Timer() {
		prev = glfwGetTime();
		delta = 0;
	}
	void use() {
		float current = glfwGetTime();
		delta = current - prev;
		prev = current;
	}
	float deltaS() { return delta; }
	float deltaMS() { return delta * 0.001; }
	float getElapsed() { return glfwGetTime() - prev; }
};