#pragma once
#include <string>
#include <vector>
#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"
using namespace std;
class Texture{
public:
	int width;
	int height;
	int channels;
	unsigned int ID;
	Texture() {}
	Texture(string path, int off);
};

class TextureArray {
public:
	int width;
	int height;
	int channels;
	int layers;
	unsigned int ID;
	TextureArray() {}
	TextureArray(string prefix, string suffix, vector<string>paths);
};
