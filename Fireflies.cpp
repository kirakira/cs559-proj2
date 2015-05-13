#include "Fireflies.h"
#include <cstdlib>
#include <ctime>
#include <glm/matrix.hpp>
#include "Utilities\ShaderTools.H"
#include <iostream>

using namespace std;

Fireflies::Fireflies(int n)
{
	srand(time(0));
	for (int i = 0; i < n; ++i)
		positions.emplace_back(randPosition());

	char *err;
	shader = loadShader("shaders/firefly.vert", "shaders/firefly.frag", err);
	if (shader == 0)
		cerr << err << endl;
}

float Fireflies::randFloat() const {
	return (float)rand() / (float)RAND_MAX;
}

vector<Pnt3f> Fireflies::getPositions() const {
	return positions;
}

Pnt3f Fireflies::randDirection() const {
	float x, y, z, n;
	do {
		x = randFloat();
		y = randFloat();
		z = randFloat();
		n = x * x + y * y + z * z;
	} while (n >= 1 || (feq(x, .5) && feq(y, .5) && feq(z, .5)));
	auto ans = Pnt3f(x, y, z) - Pnt3f(.5, .5, .5);
	ans.normalize();
	return ans;
}

Pnt3f Fireflies::randPosition() const {
	return Pnt3f(randFloat() * (xmax - xmin) + xmin,
		randFloat() * (ymax - ymin) + ymin, randFloat() * (zmax - zmin) + zmin);
}

void Fireflies::randomMove() {
	for (int i = 0; i < (int)positions.size(); ++i) {
		Pnt3f np = positions[i] + randDirection() * stepLength;
		if (np.x < xmin || np.x > xmax || np.y < ymin || np.y > ymax || np.z < zmin || np.z > zmax)
			np = randPosition();
		positions[i] = np;
	}
}

void Fireflies::draw() const {
	if (positions.size() == 0)
		return;

	GLuint vao, vbo;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, positions.size() * 3 * 6 * sizeof(float), &positions[0].x, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(shader);

	glm::mat4 projM = glm::mat4(0.0f);
	glm::mat4 viewM = glm::mat4(0.0f);

	glGetFloatv(GL_PROJECTION_MATRIX, &projM[0][0]);
	glGetFloatv(GL_MODELVIEW_MATRIX, &viewM[0][0]);
	GLuint projectID = glGetUniformLocation(shader, "projectMatrix");
	GLuint modelViewID = glGetUniformLocation(shader, "modelViewMatrix");
	GLuint localLightsID = glGetUniformLocation(shader, "localLights");
	GLuint localLightsCountID = glGetUniformLocation(shader, "localLightsCount");

	glUniformMatrix4fv(projectID, 1, GL_FALSE, &projM[0][0]);
	glUniformMatrix4fv(modelViewID, 1, GL_FALSE, &viewM[0][0]);
	glUniform3fv(localLightsID, positions.size(), &positions[0].x);
	glUniform1i(localLightsCountID, positions.size());


	glBindVertexArray(vao);
	glDrawArrays(GL_POINTS, 0, positions.size());
	glBindVertexArray(0);
	glUseProgram(0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}