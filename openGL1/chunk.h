#pragma once
#include <time.h>
#include <vector>
#include <unordered_map>
#include <bitset>
#include <iostream>
#include <cstdlib>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <queue>
#include <thread>
#include "textureDictionary.h"
#include "Perlin.h"
#include "timer.h"
#include "smartmap.h"
#define MINY -6
#define MAXY 6
using namespace std;

class RenderChunk {
public:
	glm::vec3 offset;
	vector<unsigned int>faceData;
	float slod;
	bool ready;
	RenderChunk() {
		ready = 0;
		genBuffers();
	}
	void genBuffers() {
		float faceVertices[] = {
			1.f, 0.f, 0.f, 1.f, 0.f,
			1.f, 0.f, 1.f, 1.f, 1.f,
			0.f, 0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f, 1.f
		};
	}
	
};

class Chunk{
	int bpc() {//blocks per chunk
		const int P[] = { 4096, 512, 64, 8, 1 };
		return P[LOD];
	}
	int bpl() {//blocks per layer
		const int P[] = { 256, 64, 16, 4, 1 };
		return P[LOD];
	}
	int bps() {//blocks per slice
		const int P[] = { 16, 8, 4, 2, 1 };
		return P[LOD];
	}
	int bsc() {
		const int P[] = { 1, 2, 4, 8, 16 };
		return P[LOD];
	}
	void itoxyz(int i, int& x, int& y, int& z) {
		y = i / bpl();
		x = (i % bpl()) / bps();
		z = i % bps();
	}

	unsigned int getAt(int x, int y, int z) {
		if (x >= 0 && x < bps() && y >= 0 && y < bps() && z >= 0 && z < bps()) return data[((int)y) * bpl() + ((int)x) * bps() + ((int)z)];
		return 0;
	}
	void insert(int x, int y, int z, int dir, int texture, RenderChunk& render, int sX, int sY) {

		sX = (sX + 1) * bsc() - 1;
		sY = (sY + 1) * bsc() - 1;
		x *= bsc();
		y *= bsc();
		z *= bsc();
		//std::cout << x << " " << y << " " << z << " " << dir << " " << sX << " " << sY << " " << texture << "\n";
		//if (sX < 0 || x < 0 || y < 0 || sY < 0 || z < 0) {
		//	if (dir == 0) std::cout << x << " " << y << " " << z << " " << dir << " " << sX << " " << sY << " " << texture << "\n";
		//	return;
		//}

		unsigned int D = 0;
		D = (D | x) << 4;
		D = (D | y) << 4;
		D = (D | z) << 3;
		D = (D | dir) << 4;
		D = (D | sX) << 4;
		D = (D | sY) << 9;
		D = D | (texture);
		
		//std::cout << render.faceData.size() << "*\n";
		render.faceData.push_back(D);
	}

public:
	//unsigned char *data = nullptr;
	vector<unsigned char> data;
	short LOD = 0;
	Chunk() {}
	Chunk(int lod) {
		LOD = lod;
		if (LOD > 4) LOD = 4;
		else if (LOD < 0) LOD = 0;

		//data = (unsigned char*)malloc(bpc() * sizeof(unsigned char));
		// 
		data.resize(bpc());
		for (int i = 0; i < bpc(); i++) data[i] = 0;

	}
	void init(int lod) {
		LOD = lod;
		if (LOD > 4) LOD = 4;
		else if (LOD < 0) LOD = 0;
		data.resize(bpc());
		////data = (unsigned char*)malloc(bpc() * sizeof(unsigned char));
		//for (int i = 0; i < bpc(); i++) data[i] = 0;
	}
	~Chunk() {
		//free(data);
	}

	void genBall(int block, float r) {
		//float r = 5.f;
		for (int i = 0; i < 4096; i++) {
			int x, y, z;
			itoxyz(i, x, y, z);

			float dist = sqrtf((x - 7.5) * (x - 7.5) + (y - 7.5) * (y - 7.5) + (z - 7.5) * (z - 7.5));

			if (dist < r && y >= 8 ) data[i] = block;
			else data[i] = 0;
		}
	}

	void genChunk(int cX, int cY, int cZ) {
		//cout << cX << " " << cY << " " << cZ << "^\n";
		Perlin perlin;
		float flatVal = 1000.f;
		for (int i = 0; i < bpl(); i++) {
			float x, z;
			x = (i % bpl()) / bps();
			z = i % bps();
			size_t octaves = 8;
			//float pVal, perl = (perlin.noise((16 * cX + x) / flatVal, (16 * cZ + z) / flatVal));
			float pVal = 0.f, perl = (perlin.fractal(octaves, (16 * cX + bsc() * x) / flatVal, (16 * cZ + bsc() * z) / flatVal));
			if (perl >= 0) pVal = pow(sin(perl * 3.14 / 2.1), 3.f);
			else pVal = sin(perl) / 1.5f;
			pVal = 2 * (pVal + 1) + 1;
			pVal = pow(pVal, 3);
			for (int y = bps()-1; y >= 0; y--) {
				int I = y * bpl() + (int)x * bps() + (int)z;
				int Y = cY * 16 + y * bsc();
				if (Y <= pVal) { 
					if (fabs(Y - pVal) <= 1.f * bsc()) { 
						if (Y >= 27) data[I] = 2;
						else data[I] = 4;
					}
					else if (fabs(Y - pVal) <= 3.f) data[I] = 1;
					else data[I] = 3;
				}
				else data[I] = 0;
			}
		}



	}

	void mesh2(TextureDictionary& texDict, RenderChunk& render, Chunk* adjacent[6]) {
		int layers[6][16][256];
		render.faceData.clear();
		//cout<<render.faceData.size()<<"&\n";
		//X+
		for (int L = 0; L < bps(); L++) {
			for (int i = 0; i < bpl(); i++) {
				int z = i % bps(), y = i / bps();
				if (!getAt(L + 1, y, z)) layers[1][L][i] = getAt(L, y, z);
				else layers[1][L][i] = 0;
				if (L == 15 && (adjacent[1] == nullptr || adjacent[1]->getAt(0, y, z))) layers[1][L][i] = 0;
			}
		}

		//Y+
		for (int L = 0; L < bps(); L++) {
			for (int i = 0; i < bpl(); i++) {
				int x = i % bps(), z = i / bps();
				if (!getAt(x, L+1, z)) layers[5][L][i] = getAt(x, L, z);
				else layers[5][L][i] = 0;
				if (L == 15 && (adjacent[5] == nullptr || adjacent[5]->getAt(x, 0, z))) layers[5][L][i] = 0;
			}
		}

		//Z+
		for (int L = 0; L < bps(); L++) {
			for (int i = 0; i < bpl(); i++) {
				int x = i % bps(), y = i / bps();
				if (!getAt(x, y, L + 1)) layers[2][L][i] = getAt(x, y, L);
				else layers[2][L][i] = 0;
				if (L == 15 && (adjacent[2] == nullptr || adjacent[2]->getAt(x, y, 0))) layers[2][L][i] = 0;
			}
		}

		//X-
		for (int L = 0; L < bps(); L++) {
			for (int i = 0; i < bpl(); i++) {
				int z = i % bps(), y = i / bps();
				if (!getAt(L - 1, y, z)) layers[3][L][i] = getAt(L, y, z);
				else layers[3][L][i] = 0;
				if (L == 0 && (adjacent[3] == nullptr || adjacent[3]->getAt(15, y, z))) layers[3][L][i] = 0;
			}
		}

		//Y-
		for (int L = 0; L < bps(); L++) {
			for (int i = 0; i < bpl(); i++) {
				int x = i % bps(), z = i / bps();
				if (!getAt(x, L - 1, z)) layers[4][L][i] = getAt(x, L, z);
				else layers[4][L][i] = 0;
				if (L == 0 && (adjacent[4] == nullptr || adjacent[4]->getAt(x, 15, z))) layers[4][L][i] = 0;
			}
		}

		//Z-
		for (int L = 0; L < bps(); L++) {
			for (int i = 0; i < bpl(); i++) {
				int x = i % bps(), y = i / bps();
				if (!getAt(x, y, L - 1)) layers[0][L][i] = getAt(x, y, L);
				else layers[0][L][i] = 0;
				if (L == 0 && (adjacent[0] == nullptr || adjacent[0]->getAt(x, y, 15))) layers[0][L][i] = 0;
			}
		}

		
		for (int D = 0; D < 6; D++) {
			for (int L = 0; L < bps(); L++) {
				vector<bool>vis(bpl(), 0);
				for (int i = 0; i < bpl(); i++) {
					if (vis[i]) continue;
					int ref = layers[D][L][i];
					if (ref == 0) continue;
					int x = i % bps(), y = i / bps();
					int xI = x + 1;

					while (xI < bps() && layers[D][L][i + xI - x] == ref && vis[i + xI - x] == 0) {
						vis[i + xI - x] = 1;
						xI++;
					}

					int yI = y + 1;
					bool fine = 1;
					while (yI < bps() && fine) {
						for (int vx = x; vx < xI; vx++) {
							if (layers[D][L][i + vx - x + bps() * (yI - y)] != ref) fine = 0;
						}

						if (fine) {
							for (int vx = x; vx < xI; vx++) {
								vis[i + vx - x + bps() * (yI - y)] = 1;
							}
							yI++;
						}
					}
					xI--;
					yI--;

					//std::cout << ref << " " << D << "\n";
					ref = texDict.getTextureID(ref, D);

					switch (D) {
					case 0: insert(x + (xI - x), y, L, 0, ref, render, xI - x, yI - y); break;
					case 1: insert(L, y, x + (xI - x), 1, ref, render, xI - x, yI - y); break;
					case 2: insert(x, y, L, 2, ref, render, xI - x, yI - y); break;
					case 3: insert(L, y, x, 3, ref, render, xI - x, yI - y); break;
					case 4: insert(x, L, y, 4, ref, render, xI - x, yI - y); break;
					case 5: insert(x + (xI - x), L, y, 5, ref, render, xI - x, yI - y); break;
					}

				}
			}
		}
	}
};



	class World {
	public:
		queue<glm::ivec3>qGen;
		queue<glm::ivec3>qMesh;

		int radius;
		SmartMap<Chunk>wData;
		SmartMap<RenderChunk>wRender;
		//vector<unsigned int> ids;
		unsigned int wVBO, wfVBO, wVAO;
		glm::ivec3 prevPos;
		int VOLUME;
		int lodLevel;
		World() {
			lodLevel = 2;
			radius = 15;
			VOLUME = pow(radius * 2 + 1, 2) * (MAXY - MINY + 1);
			cout << "Volume: " << VOLUME << "\n";
			wData.init(VOLUME);
			wRender.init(VOLUME);
			for (int i = 0; i < VOLUME; i++) {
				wData[i].init(0);
			}
			prevPos = glm::ivec3(0, 0, 0);

			glGenBuffers(1, &wVBO);
			glBindBuffer(GL_ARRAY_BUFFER, wVBO);
			glBufferData(GL_ARRAY_BUFFER, 12288 * VOLUME * sizeof(int), nullptr, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			float faceVertices[] = {
			1.f, 0.f, 0.f, 1.f, 0.f,
			1.f, 0.f, 1.f, 1.f, 1.f,
			0.f, 0.f, 0.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f, 1.f
			};

			glGenVertexArrays(1, &wVAO);
			glGenBuffers(1, &wfVBO);
			glBindVertexArray(wVAO);
			glBindBuffer(GL_ARRAY_BUFFER, wfVBO);
			glBufferData(GL_ARRAY_BUFFER, sizeof(faceVertices), faceVertices, GL_STATIC_DRAW);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
			glEnableVertexAttribArray(1);

			glEnableVertexAttribArray(2);
			glBindBuffer(GL_ARRAY_BUFFER, wVBO);
			glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, sizeof(unsigned int), (void*)0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glVertexAttribDivisor(2, 1);
			glBindVertexArray(0);
		}
		~World(){
			//chunks.clear();
		}

		bool isInRange(const glm::ivec3& a, const glm::ivec3& b) {
			glm::ivec3 c = a - b;
			return !(abs(c.x) > radius || abs(c.z) > radius || c.y > MAXY || c.y < MINY);
		}

		Chunk* getCondition(glm::ivec3 pos) {
			if (!isInRange(pos, prevPos)) return nullptr;
			if (posHash(pos) != wData.getHashAt(pos)) {
				wData.getAt(pos).genChunk(pos.x, pos.y, pos.z);
				wData.updateHash(pos);
			}
			return &wData.getAt(pos);
		}

		void lazyGen(glm::ivec3 pos, TextureDictionary& texDict) {
			Chunk* pts[6];
			int hash = posHash(pos);
			pts[0] = getCondition(glm::ivec3(pos.x, pos.y, pos.z - 1));
			pts[1] = getCondition(glm::ivec3(pos.x + 1, pos.y, pos.z));
			pts[2] = getCondition(glm::ivec3(pos.x, pos.y, pos.z + 1));
			pts[3] = getCondition(glm::ivec3(pos.x - 1, pos.y, pos.z));
			pts[4] = getCondition(glm::ivec3(pos.x, pos.y - 1, pos.z));
			pts[5] = getCondition(glm::ivec3(pos.x, pos.y + 1, pos.z));
			if (hash != wData.getHashAt(pos)) {
				wData.getAt(pos).genChunk(pos.x, pos.y, pos.z);
				wData.updateHash(pos);
			}
			//qMesh.push(pos);
			wData.getAt(pos).mesh2(texDict, wRender.getAt(pos), pts);
			wRender.getAt(pos).offset = pos;
			wRender.getAt(pos).slod = pow(2, 0);	//LOD
			wRender.updateHash(pos);

			glBindBuffer(GL_ARRAY_BUFFER, wVBO);
			glBufferSubData(GL_ARRAY_BUFFER, wRender.getI(pos) * 12288 * sizeof(int),
				wRender.getAt(pos).faceData.size() * sizeof(int), wRender.getAt(pos).faceData.data());
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			wRender.getAt(pos).ready = 1;
		}

		void manageThreads(TextureDictionary& texDict) {
			if (qGen.empty()) return;
			Timer timer;
			while (timer.getElapsed() < 0.01) {
				if (qGen.empty()) return;
				lazyGen(qGen.front(), texDict);
				//thread genThread(lazyGen, qGen.front(), texDict);
				//genThread.detach();
				qGen.pop();
			}
		}

		RenderChunk& getRenderAt(glm::ivec3 pos, TextureDictionary& texDict) {
			int hash = posHash(pos);
			if (hash == wRender.getHashAt(pos)) return wRender.getAt(pos);
			else {
				if (wRender.getHashAt(pos) != 0) {
					qGen.push(pos);
					wRender.getHashAt(pos) = 0;
					wRender.getAt(pos).ready = 0;
				}
				return wRender.getAt(pos);
			}
		}

		void testgen(TextureDictionary& texDict) {
			for (int x = -radius; x <= radius; x++) {
				for (int y = MINY; y <= MAXY; y++) {
					for (int z = -radius; z <= radius; z++) {
						wData.getAt(glm::ivec3(x, y, z)).genChunk(x, y, z);
						int LOD = 0;
						if (LOD > 4) LOD = 4;
						else if (LOD < 0) LOD = 0;

							//gen chunk
							//Chunk tmp(LOD);
							//int block = rand() % 4 + 1;
							//chunks[ID] = tmp;
							//chunks[ID].genBall(block, 7.f);// (float)(rand() % 70) / 10.f + 2.f);
							//chunks[ID].genChunk(x, y, z);
							//std::cout << x << " " << y << " " << z << "\n";
							//for (int i = 0; i < 4096; i++) std::cout << (int)chunks[ID].data[i] << " ";
							//std::cout << "\n";
							 
							
						//wData.getAt(glm::ivec3(x, y, z)).mesh2(texDict, wRender.getAt(glm::ivec3(x, y, z)));
						wRender.getAt(glm::ivec3(x, y, z)).offset = glm::vec3(x, y, z);
						wRender.getAt(glm::ivec3(x, y, z)).genBuffers();
						wRender.getAt(glm::ivec3(x, y, z)).slod = pow(2, LOD);


					}
				}
			}


		}
};

