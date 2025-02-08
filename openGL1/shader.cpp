#include "shader.h"
#include <vector>
#define DIR "shaders/"
Shader::Shader(string vertexPath, string fragmentPath){
	string vCode, fCode;
	fstream vFile, fFile;
	stringstream vStream, fStream;

	vFile.open(DIR + vertexPath);
	vStream << vFile.rdbuf();
	vFile.close();
	vCode = vStream.str();
	const char* vcCode = vCode.c_str();
	
	fFile.open(DIR + fragmentPath);
	fStream << fFile.rdbuf();
	fFile.close();
	fCode = fStream.str();
	const char* fcCode = fCode.c_str();

	unsigned int vertex, fragment;
	int success;

	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vcCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) { 
		std::cout << "Vertex shader compilation error\n"; 
		GLint logLength;
		glGetShaderiv(vertex, GL_INFO_LOG_LENGTH, &logLength);

		// Alokacja bufora na log
		std::vector<GLchar> log(logLength);
		glGetShaderInfoLog(vertex, logLength, nullptr, log.data());

		// Wypisanie logu b³êdu
		std::cerr << "B³¹d kompilacji shadera:\n" << log.data() << std::endl;
	}

	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fcCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) std::cout << "Fragment shader compilation error\n";

	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);

	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if (!success) std::cout << "Shader program linking error\n";
	
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::use() {
	glUseProgram(ID);
}

void Shader::setBool(const string& name, bool value) {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void Shader::setInt(const string& name, int value) {
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setFloat(const string& name, float value) {
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void Shader::setMat4(const string& name, glm::mat4 value){
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setVec3(const string& name, glm::vec3 value) {
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, glm::value_ptr(value));
}
