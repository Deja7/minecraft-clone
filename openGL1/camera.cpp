#include "camera.h"

glm::mat4 Camera::getViewMatrix() {
	front = glm::normalize(glm::vec3(
		cos(glm::radians(horizontal)) * cos(glm::radians(vertical)),
		sin(glm::radians(vertical)),
		sin(glm::radians(horizontal)) * cos(glm::radians(vertical))
	));
	return glm::lookAt(pos, pos + front, up);
}

Camera::Camera() {
	up = glm::vec3(0.0, 1.0, 0.0);
	pos = glm::vec3(0.0, 0.0, 0.0);
	vertical = 0;
	horizontal = 0;
}