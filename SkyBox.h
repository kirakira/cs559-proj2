#pragma once

#include "FL/gl.h"

class SkyBox
{
public:
	SkyBox() {};
	SkyBox(GLuint shader_program);
	~SkyBox();
	void draw();
	
	void cubeMapLoad(GLuint* tex);
	void loadCubeFace(GLenum side, char* filename);

private:
	GLuint shaderProgram;
	GLuint tex;

};

