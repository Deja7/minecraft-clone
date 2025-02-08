#pragma once
#include <glad/glad.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
class Shader{
public:
	unsigned int ID;

	Shader() {}
	Shader(string vertexPath, string fragmentPath);
	void use();
	void setBool(const string& name, bool value);
	void setInt(const string& name, int value);
	void setFloat(const string& name, float value);
	void setMat4(const string& name, glm::mat4 value);
	void setVec3(const string& name, glm::vec3 value);
};