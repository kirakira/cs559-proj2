#include "Model.h"
#include <set>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

using namespace std;

unique_ptr<Mesh> ModelLoader::load(const string &filename) {
	ifstream fin(filename);
	if (!fin.good())
		return nullptr;

	string command;
	while (fin >> command) {
		if (command == "name") {
		}
	}

	fin.close();
}