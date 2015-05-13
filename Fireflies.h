#pragma once

#include "GL/glew.h"
#include <vector>
#include "Utilities\Pnt3f.h"

class Fireflies
{
	std::vector<Pnt3f> positions;

	float randFloat() const;
	Pnt3f randDirection() const;
	Pnt3f randPosition() const;

	const float xmin = -100, xmax = 100, zmin = -100, zmax = 100, ymin = 0, ymax = 50;
	const float stepLength = .5f;

	GLuint shader;

public:
	Fireflies(int n);
	virtual ~Fireflies() = default;

	void randomMove();
	void draw() const;
	std::vector<Pnt3f> getPositions() const;
};

