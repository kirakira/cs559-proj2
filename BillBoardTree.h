#pragma once

#include "FL/gl.h"

class BillBoardTree
{
public:
	BillBoardTree(GLuint billboard_program);
	BillBoardTree();
	~BillBoardTree();

	void loadTexture(GLuint *tex);
	void draw();

	GLuint billboardProgram;
	GLuint tex;
};

