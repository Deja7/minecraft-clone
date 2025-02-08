#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera{
public:
	glm::vec3 pos;
	glm::vec3 front;
	glm::vec3 up;
	float vertical;
	float horizontal;
	Camera();
	glm::mat4 getViewMatrix();
	void move() {
		//horizontal += 0.01;
		//pos.x -= 0.001;
	}
};

