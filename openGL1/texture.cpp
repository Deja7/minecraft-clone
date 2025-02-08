#include "texture.h"
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
Texture::Texture(string path, int off){
	unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
	if (!data) cout << "Failed to load texture: " << path << "\n";
	glGenTextures(1, &ID);
	glBindTexture(GL_TEXTURE_2D, ID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, 
		GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);
}

TextureArray::TextureArray(string prefix, string suffix, vector<string> paths) {
	glGenTextures(1, &ID);

	glBindTexture(GL_TEXTURE_2D_ARRAY, ID);

	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	width = 16, height = 16, layers = 512;
	glTexImage3D(
		GL_TEXTURE_2D_ARRAY,
		0,                          // Poziom mipmapy
		GL_RGB,                   // Wewnêtrzny format
		width,                      // Szerokoœæ tekstury
		height,                     // Wysokoœæ tekstury
		layers,                     // Liczba warstw
		0,                          // Brak obramowania
		GL_RGB,                    // Format danych
		GL_UNSIGNED_BYTE,           // Typ danych
		nullptr                     // Na razie nie przesy³amy danych
	);

	for (int i = 0; i < paths.size(); i++) {
		string path = prefix + paths[i] + suffix;
		unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		//std::cout << channels << "\n";
		if (channels == 1) {
			unsigned char* nData = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 3);
			for (int j = 0; j < width * height; j++) {
				unsigned char c = data[j];
				glm::vec3 cFilter = glm::vec3(0.4f, 0.9f, 0.4f);
				nData[3 * j] = c * cFilter.x;
				nData[3 * j + 1] = c * cFilter.y;
				nData[3 * j + 2] = c * cFilter.z;
			}
			data = (unsigned char*)malloc(sizeof(unsigned char) * width * height * 3);
			data = nData;
		}
		if (!data) { cout << "Failed to load texture: " << path << "\n"; continue; }
		glTexSubImage3D(
			GL_TEXTURE_2D_ARRAY,
			0,               // Poziom mipmapy
			0, 0, i,         // Offset w osi X, Y i warstwa Z
			width, height, 1, // Rozmiar warstwy
			GL_RGB,         // Format danych
			GL_UNSIGNED_BYTE,// Typ danych
			data             // Dane tekstury
		);
		stbi_image_free(data);
	}
	glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}
