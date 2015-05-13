#include "GL/glew.h"
#include "FL/gl.h"
#include "glm/glm.hpp"
#include "BillBoardTree.h"
#include "Utilities/libtarga.h"
#include "TrainView.H"

const GLfloat points[] = {

	25.0f, 0.0f, 100.0f,
	25.0f, 5.0f, 100.0f,
	20.0f, 5.0f, 100.0f,

	20.0f, 5.0f, 100.0f,
	20.0f, 0.0f, 100.0f,
	25.0f, 0.0f, 100.0f,
};

const GLfloat texCoords[] = {

	1.0f, 0.0f,
	1.0f, 1.0f,
	0.0f, 1.0f,

	0.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
};

BillBoardTree::BillBoardTree() {}
BillBoardTree::~BillBoardTree() {}
BillBoardTree::BillBoardTree(GLuint billboard_program) {
	billboardProgram = billboard_program;
	loadTexture(&tex);
}

void BillBoardTree::loadTexture(GLuint *tex) {
	char* filename = "pictures/tree.tga";
	int W, H;
	unsigned char* data = (unsigned char*)tga_load(filename, &W, &H, TGA_TRUECOLOR_32);

	glActiveTexture(GL_TEXTURE0);
	glGenTextures(1, tex);
	glBindTexture(GL_TEXTURE_2D, *tex);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, W, H, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glBindTexture(GL_TEXTURE_2D, 0);
}

void BillBoardTree::draw() {

	GLuint vbo[2];
	glGenBuffers(2, vbo);

	GLuint positionHandle = vbo[0];
	GLuint normalHandle = vbo[1];

	glBindBuffer(GL_ARRAY_BUFFER, positionHandle);
	glBufferData(GL_ARRAY_BUFFER, 3 * 6 * sizeof(GLfloat), &points, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, normalHandle);
	glBufferData(GL_ARRAY_BUFFER, 2 * 6 * sizeof(GLfloat), &texCoords, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, positionHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, normalHandle);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

	glUseProgram(billboardProgram);

		glm::mat4 ModelViewMatrix = TrainView::getModelViewMatrix();
		TrainView::bindUniformMatrix(billboardProgram, "ModelViewMatrix", &ModelViewMatrix[0][0]);

		glm::mat4 ProjectionMatrix = TrainView::getProjectionMatrix();
		TrainView::bindUniformMatrix(billboardProgram, "ProjectionMatrix", &ProjectionMatrix[0][0]);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

}
