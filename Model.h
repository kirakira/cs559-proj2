#pragma once

#include <string>
#include <vector>
#include <tuple>

#include "Utilities/Pnt3f.h"

class Model
{
	std::vector<std::tuple<int, int, int>> convexHull(const std::vector<Pnt3f> &points);

public:
	Model(const std::string &filename);
	virtual ~Model() = default;
};

class Models
{
public:
	// returns the number of models loaded
	int load(const std::string &filename);
};