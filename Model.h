#pragma once

#include <map>
#include <string>
#include <vector>
#include <tuple>
#include <memory>

#include "Utilities/Pnt3f.h"
#include "FL/gl.h"
#include "Mesh.h"

class ModelLoader
{
public:
	// returns the names of successfully loaded models
	std::unique_ptr<Mesh> load(const std::string &filename);
};