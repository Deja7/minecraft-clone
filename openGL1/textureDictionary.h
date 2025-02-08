#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
using namespace std;
class TextureDictionary {
public:
	vector<string>names;
	unordered_map<string, unsigned int>ids;
	vector<vector<int>>faces;
	void loadnames() {
		fstream file;
		file.open("assets/loadlist.txt");
		while (!file.eof()) {
			string tmp;
			file >> tmp;
			names.push_back(tmp);
			ids[tmp] = names.size() - 1;
		}
		file.close();
	}

	void loadfaces() {
		fstream file;
		file.open("assets/faces.txt");
		while (!file.eof()) {
			string tmp;
			file >> tmp;
			if (tmp == "#") {
				file >> tmp;
				int ID = ids[tmp];
				faces.push_back({ID, ID, ID, ID, ID, ID});
			}
			else {
				vector<int>ID(6,0);
				ID[0] = ids[tmp];
				for (int i = 1; i <= 5; i++) { 
					file >> tmp;
					ID[i] = ids[tmp]; 
				}
				faces.push_back(ID);
			}
		}
		file.close();

		//for (auto list : faces) {
		//	for (int i = 0; i < 6; i++) cout << list[i];
		//	cout << "\n";
		//}
	}

	int getTextureID(int block, int dir) {
		if (block >= faces.size()) return 0;
		return faces[block - 1][dir];
	}

};

//TextureDictionary TexDict;