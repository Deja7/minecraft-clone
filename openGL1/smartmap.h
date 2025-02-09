#pragma once
#include <iostream>
#include <unordered_map>
#include <glm/glm.hpp>

inline unsigned int posHash(glm::ivec3 pos) {
	unsigned int hash = 0;
	hash ^= (pos.x * 73856093);
	hash ^= (pos.y * 19349663);
	hash ^= (pos.z * 83492791);

	hash ^= (hash >> 13);
	hash *= 0x5bd1e995;
	hash ^= (hash >> 15);

	return hash;
}

template <typename T> 
class SmartMap {
private:
	T* data;
	int volume;
	unsigned int* hashes;
	int radius = 15;
	int miny = -6;
	int maxy = 6;
	int lim = 2 * radius + 1;
	int limy = maxy - miny + 1;
	void limit(glm::ivec3& vec) {
		if (vec.x < 0) vec.x = (lim - ((-vec.x) % lim)) % lim;
		else if (vec.x > 0) vec.x = vec.x % lim;

		if (vec.y < 0) vec.y = (limy - ((-vec.y) % limy)) % limy;
		else if (vec.y > 0) vec.y = vec.y % limy;

		if (vec.z < 0) vec.z = (lim - ((-vec.z) % lim)) % lim;
		else if (vec.z > 0) vec.z = vec.z % lim;
	}

public:
	void init(unsigned int isize) {
		if (data) delete[] data;
		data = new T[isize];
		hashes = new unsigned int[isize];
		volume = isize;
		for (int i = 0; i < volume; i++) {
			data[i] = T();
			hashes[i] = UINT_MAX;
		}
	}
	~SmartMap() {
		delete[] data;
	}

	int getI(glm::ivec3 pos) {
		pos += glm::ivec3(radius, -miny, radius);
		limit(pos);
		return pos.x + pos.z * (radius * 2 + 1) + pos.y * (radius * 2 + 1) * (radius * 2 + 1);
	}

	T& getAt(glm::ivec3 pos) {
		pos += glm::ivec3(radius, -miny, radius);
		limit(pos);
		int i = pos.x + pos.z * (radius * 2 + 1) + pos.y * (radius * 2 + 1) * (radius * 2 + 1);
		return data[i];
	}

	T& operator[](int i) {
		return data[i];
	}

	unsigned int& getHashAt(glm::ivec3 pos) {
		pos += glm::ivec3(radius, -miny, radius);
		limit(pos);
		int i = pos.x + pos.z * (radius * 2 + 1) + pos.y * (radius * 2 + 1) * (radius * 2 + 1);
		return hashes[i];
	}

	void updateHash(glm::ivec3 pos) {
		glm::ivec3 tmp = pos;
		pos += glm::ivec3(radius, -miny, radius);
		limit(pos);
		int i = pos.x + pos.z * (radius * 2 + 1) + pos.y * (radius * 2 + 1) * (radius * 2 + 1);
		hashes[i] = posHash(tmp);
	}

	bool isInRange(glm::ivec3 pos) {
		pos += glm::ivec3(radius, -miny, radius);
		glm::ivec3 sec = pos;
		limit(sec);
		return pos == sec;
	}

	int size() {
		return volume;
	}
	
};

