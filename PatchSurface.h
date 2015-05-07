#pragma once

#include <vector>
#include "Utilities/Pnt3f.h"

class PatchSurface
{
	std::vector< std::vector<Pnt3f> > controlPoints;
	GLuint vao, vbo;
	int renderedVertexCount;

	void initVertexArray();

public:
	PatchSurface(const Pnt3f *control_points, int n, int m);
	~PatchSurface();

	// s ranges from [0, n - 3)
	// t ranges from [0, m - 3)
	Pnt3f pointAt(float s, float t) const;

	void draw() const;
};
