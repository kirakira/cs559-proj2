#include "GL/glew.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "FL/gl.h"
#include "TrainView.H"
#include "Projector.h"
#include "Utilities/libtarga.h"

#define SIZE 50.0f
const GLfloat points[] = {
	
	-SIZE, 0.0f, -SIZE,
	-SIZE, 0.0f, SIZE,
	SIZE, 0.0f, SIZE,

	SIZE, 0.0f, SIZE,
	SIZE, 0.0f, -SIZE,
	-SIZE, 0.0f, -SIZE,
};

const GLfloat point_normals[] = {

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,

	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
};

Projector::Projector(GLuint projector_program) {
	projectorProgram = projector_program;
}

Projector::Projector() { }
Projector::~Projector() { }

void Projector::loadTexture(GLuint *tex) {
	char* filename = "pictures/smiley.tga";
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

void Projector::draw() {

	loadTexture(&tex);

	GLuint vbo[2];
	glGenBuffers(2, vbo);

	GLuint positionHandle = vbo[0];
	GLuint normalHandle = vbo[1];

	glBindBuffer(GL_ARRAY_BUFFER, positionHandle);
	glBufferData(GL_ARRAY_BUFFER, 3 * 6 * sizeof(GLfloat), &points, GL_STATIC_DRAW);
	
	glBindBuffer(GL_ARRAY_BUFFER, normalHandle);
	glBufferData(GL_ARRAY_BUFFER, 3 * 6 * sizeof(GLfloat), &point_normals, GL_STATIC_DRAW);

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	glBindBuffer(GL_ARRAY_BUFFER, positionHandle);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glBindBuffer(GL_ARRAY_BUFFER, normalHandle);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glUseProgram(projectorProgram);

	glm::vec3 projPos = glm::vec3(2.0f, 5.0f, 5.0f);
	glm::vec3 projAt = glm::vec3(-2.0f, -4.0f, 0.0f);
	glm::vec3 projUp = glm::vec3(0.0f, 1.0f, 0.0f);
	
	glm::mat4 projView = glm::lookAt(projPos, projAt, projUp);
	glm::mat4 projProjection = glm::perspective(45.0f, 1.0f, 0.1f, 1000.0f);
	glm::mat4 projScaleTrans = glm::translate(glm::mat4(1.0f), glm::vec3(0.5f))	* glm::scale(glm::mat4(1.0f), glm::vec3(1.0f));

	glm::mat4 projectorMatrix = projScaleTrans * projProjection * projView;
	TrainView::bindUniformMatrix(projectorProgram, "ProjectorMatrix", &projectorMatrix[0][0]);

	glm::mat4 NormalMatrix = TrainView::getNormalMatrix();
	TrainView::bindUniformMatrix(projectorProgram, "NormalMatrix", &NormalMatrix[0][0]);

	glm::mat4 ModelViewMatrix = TrainView::getModelViewMatrix();
	TrainView::bindUniformMatrix(projectorProgram, "ModelViewMatrix", &ModelViewMatrix[0][0]);

	glm::mat4 ModelMatrix = glm::mat4(1.0f);
	TrainView::bindUniformMatrix(projectorProgram, "ModelMatrix", &ModelMatrix[0][0]);

	glm::mat4 ProjectionMatrix = TrainView::getProjectionMatrix();
	TrainView::bindUniformMatrix(projectorProgram, "ProjectionMatrix", &ProjectionMatrix[0][0]);

	glm::mat4 MVP = ProjectionMatrix * ModelViewMatrix;
	TrainView::bindUniformMatrix(projectorProgram, "MVP", &MVP[0][0]);
	
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, tex);
		glBindVertexArray(vao);
			glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, 0);

	glUseProgram(0);

}
