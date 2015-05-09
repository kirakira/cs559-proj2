#include "GL/glew.h"
#include "PatchSurface.h"
#include "CurvePiece.h"
#include "FL/gl.h"
#include "glm/glm.hpp"
#include <cmath>

using namespace std;
using namespace glm;

PatchSurface::PatchSurface(const Pnt3f *control_points, int n, int m, GLuint shader)
	: shaderProgram(shader)
{
	controlPoints = vector< vector<Pnt3f> >(n, vector<Pnt3f>(m));
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < m; ++j)
			controlPoints[i][j] = control_points[i * m + j];
	initVertexArray();
}

PatchSurface::~PatchSurface()
{
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

Pnt3f PatchSurface::pointAt(float s, float t) const {
	int i_start = (int) floor(s), j_start = (int) floor(t);
	vector<Pnt3f> vp;
	for (int i = i_start; i < i_start + 4; ++i) {
		CardinalPiece cp(controlPoints[i][j_start + 1], controlPoints[i][j_start + 2],
			controlPoints[i][j_start], controlPoints[i][j_start + 3], .5);
		vp.emplace_back(cp.positionAt(t - (int) t));
	}
	CardinalPiece cp(vp[1], vp[2], vp[0], vp[3], .5);
	return cp.positionAt(s - (int) s);
}

void PatchSurface::initVertexArray() {
	vector< vector<Pnt3f> > mesh;
	float delta = .1;
	for (float s = 0; s < controlPoints.size() - 3; s += delta) {
		mesh.push_back(vector<Pnt3f>());
		for (double t = 0; t < controlPoints.size() - 3; t += delta)
			mesh.back().push_back(pointAt(s, t));
	}
	vector<Pnt3f> flatterned;
	flatterned.push_back(mesh[0][0]);
	for (int i = 0; i + 1 < mesh.size(); ++i) {
		for (int j = 0; j < mesh[i].size(); ++j) {
			int sj = j;
			if (i % 2)
				sj = mesh[i].size() - j - 1;
			if (j > 0)
				flatterned.push_back(mesh[i][sj]);
			flatterned.push_back(mesh[i + 1][sj]);
		}
	}
	renderedVertexCount = flatterned.size();

	size_t buffer_size = flatterned.size() * sizeof(float) * 3;
	float *buffer = new float[buffer_size];
	for (int i = 0; i < (int)flatterned.size(); ++i) {
		buffer[i * 3] = flatterned[i].x;
		buffer[i * 3 + 1] = flatterned[i].y;
		buffer[i * 3 + 2] = flatterned[i].z;
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, buffer_size, buffer, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] buffer;
}

void PatchSurface::draw() const {
	glUseProgram(shaderProgram);

	mat4 projM = mat4(0.0f);
	mat4 viewM = mat4(0.0f);

	glGetFloatv(GL_PROJECTION_MATRIX, &projM[0][0]);
	glGetFloatv(GL_MODELVIEW_MATRIX, &viewM[0][0]);
	GLuint projectID = glGetUniformLocation(shaderProgram, "projectMatrix");
	GLuint modelViewID = glGetUniformLocation(shaderProgram, "modelViewMatrix");

	glUniformMatrix4fv(projectID, 1, GL_FALSE, &projM[0][0]);
	glUniformMatrix4fv(modelViewID, 1, GL_FALSE, &viewM[0][0]);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, renderedVertexCount);
	glBindVertexArray(0);
	
	glUseProgram(0);
}