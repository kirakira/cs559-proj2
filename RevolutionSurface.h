#pragma once

#include "Utilities\Pnt3f.h"
#include <vector>
#include "Mesh.h"
#include <memory>

class RevolutionSurface
{
public:
	RevolutionSurface() = delete;
	virtual ~RevolutionSurface() = default;

	static std::unique_ptr<Mesh> generate(std::vector<Pnt3f> controlPoints);
};

