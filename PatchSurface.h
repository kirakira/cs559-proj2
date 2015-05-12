#pragma once

#include <vector>
#include <memory>
#include "Utilities/Pnt3f.h"
#include "Mesh.h"

class PatchSurface
{
	// s ranges from [0, n - 3)
	// t ranges from [0, m - 3)
	static Pnt3f pointAt(const std::vector<std::vector<Pnt3f>> &contronPoints,
		float s, float t);

	PatchSurface() = delete;

public:
	static std::unique_ptr<Mesh> generate(const Pnt3f *control_points, int n, int m);
	~PatchSurface() = default;
};
