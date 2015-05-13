#pragma once

#include "FL/gl.h"

class Projector
{
public:
	Projector(GLuint projector_program);
	Projector();
	~Projector();

	void loadTexture(GLuint *tex);
	void draw();

	GLuint projectorProgram;
	GLuint tex;
};

