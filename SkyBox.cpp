#include "GL/glew.h"
#include "glm/glm.hpp"
#include "FL/gl.h"
#include "SkyBox.h"
#include "Utilities/libtarga.h"

#include <cstdio>
#include <cmath>

using namespace std;
using namespace glm;

#define SIZE 100.0f

const GLfloat points[] = {
	-SIZE, SIZE, -SIZE,
	-SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE, SIZE, -SIZE,
	-SIZE, SIZE, -SIZE,

	-SIZE, -SIZE, SIZE,
	-SIZE, -SIZE, -SIZE,
	-SIZE, SIZE, -SIZE,
	-SIZE, SIZE, -SIZE,
	-SIZE, SIZE, SIZE,
	-SIZE, -SIZE, SIZE,

	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, SIZE,
	SIZE, SIZE, SIZE,
	SIZE, SIZE, SIZE,
	SIZE, SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,

	-SIZE, -SIZE, SIZE,
	-SIZE, SIZE, SIZE,
	SIZE, SIZE, SIZE,
	SIZE, SIZE, SIZE,
	SIZE, -SIZE, SIZE,
	-SIZE, -SIZE, SIZE,

	-SIZE, SIZE, -SIZE,
	SIZE, SIZE, -SIZE,
	SIZE, SIZE, SIZE,
	SIZE, SIZE, SIZE,
	-SIZE, SIZE, SIZE,
	-SIZE, SIZE, -SIZE,

	-SIZE, -SIZE, -SIZE,
	-SIZE, -SIZE, SIZE,
	SIZE, -SIZE, -SIZE,
	SIZE, -SIZE, -SIZE,
	-SIZE, -SIZE, SIZE,
	SIZE, -SIZE, SIZE
};

SkyBox::SkyBox(GLuint shader_program) { 
	this->shaderProgram = shader_program;

	cubeMapLoad(&tex);
}
SkyBox::~SkyBox() { }

void SkyBox::cubeMapLoad(GLuint *tex) {
	char 
		*frontf = "pictures/front.tga", 
		*backf = "pictures/back.tga", 
		*bottomf = "pictures/bottom.tga", 
		*topf = "pictures/top.tga", 
		*leftf = "pictures/left.tga", 
		*rightf = "pictures/right.tga";

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, tex);
	loadCubeFace(*tex, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, frontf);
	loadCubeFace(*tex, GL_TEXTURE_CUBE_MAP_POSITIVE_Z, backf);
	loadCubeFace(*tex, GL_TEXTURE_CUBE_MAP_POSITIVE_Y, topf);
	loadCubeFace(*tex, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, bottomf);
	loadCubeFace(*tex, GL_TEXTURE_CUBE_MAP_NEGATIVE_X, leftf);
	loadCubeFace(*tex, GL_TEXTURE_CUBE_MAP_POSITIVE_X, rightf);

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void SkyBox::loadCubeFace(GLuint tex, GLenum side, char* filename) {
	glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
	int W, H;
	unsigned char* data = (unsigned char*)tga_load(filename, &W, &H, TGA_TRUECOLOR_32);
	
	glTexImage2D(side, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}

void SkyBox::draw() {

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, 3 * 36 * sizeof(GLfloat), &points, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glPushMatrix();
	glTranslatef(0.0f, SIZE, 0.0f);

	glDepthMask(GL_FALSE);
	glUseProgram(shaderProgram);
	
		mat4 projM = mat4(0.0f);
		mat4 viewM = mat4(0.0f);

		glGetFloatv(GL_PROJECTION_MATRIX, &projM[0][0]);
		glGetFloatv(GL_MODELVIEW_MATRIX, &viewM[0][0]);
		GLuint projectID = glGetUniformLocation(shaderProgram, "Projection");
		GLuint modelViewID = glGetUniformLocation(shaderProgram, "ModelView");
	
		glUniformMatrix4fv(projectID, 1, GL_FALSE, &projM[0][0]);
		glUniformMatrix4fv(modelViewID, 1, GL_FALSE, &viewM[0][0]);
	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		
		glBindVertexArray(0);
		
	glUseProgram(0);
	glDepthMask(GL_TRUE);

	glPopMatrix();
}
