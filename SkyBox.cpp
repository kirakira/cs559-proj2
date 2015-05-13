#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtx/string_cast.hpp"
#include "FL/gl.h"
#include "SkyBox.h"
#include "Utilities/libtarga.h"

#include <cstdio>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;
using namespace glm;

#define SIZE 1.0f

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
	vector<char*> filenames;
	filenames.push_back("pictures/right.tga"),
	filenames.push_back("pictures/left.tga"),
		filenames.push_back("pictures/top.tga"),
		filenames.push_back("pictures/bottom.tga"),
		filenames.push_back("pictures/front.tga"),
		filenames.push_back("pictures/back.tga"),
		
	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, tex);

	glBindTexture(GL_TEXTURE_CUBE_MAP, *tex);

	for (GLuint i = 0; i < filenames.size(); i++) {
		loadCubeFace(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, filenames[i]);
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void SkyBox::loadCubeFace(GLenum side, char* filename) {
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
