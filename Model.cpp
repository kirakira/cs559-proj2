#include "Model.h"
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

vector<string> split(const string &s, char delim) {
	string line;
	vector<string> ans;
	istringstream ss(s);
	while (getline(ss, line, delim))
		ans.emplace_back(line);
	return ans;
}

int parseInt(const string &s) {
	istringstream ss(s);
	int i;
	ss >> i;
	return i;
}

unique_ptr<Mesh> ModelLoader::load(const string &filename) {
	ifstream fin(filename);
	if (!fin.good())
		return nullptr;

	vector<Pnt3f> vertices;
	vector<Pnt3f> normals;
	map<int, int> vertexToNormal;
	vector<tuple<int, int, int>> faces;

	string line;
	while (getline(fin, line)) {
		istringstream ss(line);
		string command;
		ss >> command;
		if (command.empty() || command[0] == '#')
			continue;
		if (command == "v") {
			float x, y, z;
			ss >> x >> y >> z;
			vertices.emplace_back(Pnt3f(x, y, z));
		}
		else if (command == "vn") {
			float x, y, z;
			ss >> x >> y >> z;
			normals.emplace_back(Pnt3f(x, y, z));
		}
		else if (command == "f") {
			vector<int> fi;
			for (int i = 0; i < 3; ++i) {
				string face;
				ss >> face;
				auto components = split(face, '/');
				if (components.empty())
					cerr << "Bad model file: " << line << endl;
				else {
					int x = parseInt(components[0]);
					if (x > (int) vertices.size() || x <= 0)
						cerr << "Index out of range (only has " << vertices.size() << " vertices): " << line << endl;
					fi.emplace_back(x - 1);

					if (components.size() >= 3) {
						int z = parseInt(components[2]);
						if (z > (int)normals.size() || z <= 0)
							cerr << "Index out of range (only has " << normals.size() << " normals): " << line << endl;
						vertexToNormal[x - 1] = z - 1;
					}
				}
			}
			if (fi.size() < 3)
				cerr << "Bad model file: " << line << endl;
			else
				faces.emplace_back(make_tuple(fi[0], fi[1], fi[2]));
		}
	}

	fin.close();

	vector<Vertex> v;
	for (int i = 0; i < (int) vertices.size(); ++i) {
		v.emplace_back(vertices[i], Pnt3f(0, 0, 0));
		if (vertexToNormal.count(i) > 0)
			v.back().normal = normals[vertexToNormal[i]];
	}

	return make_unique<Mesh>(std::move(v), std::move(faces));
}