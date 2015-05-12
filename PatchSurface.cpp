#include "GL/glew.h"
#include "PatchSurface.h"
#include "CurvePiece.h"
#include "FL/gl.h"
#include "glm/glm.hpp"
#include <cmath>
#include <tuple>

using namespace std;
using namespace glm;

unique_ptr<Mesh> PatchSurface::generate(const Pnt3f *control_points, int n, int m, float delta)
{
	auto controlPoints = vector< vector<Pnt3f> >(n, vector<Pnt3f>(m));
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < m; ++j)
			controlPoints[i][j] = control_points[i * m + j];

	vector< vector<Pnt3f> > mesh;
	for (float s = 0; s < controlPoints.size() - 3; s += delta) {
		mesh.push_back(vector<Pnt3f>());
		for (float t = 0; t < controlPoints.size() - 3; t += delta)
			mesh.back().push_back(pointAt(controlPoints, s, t));
	}

	vector<Pnt3f> vertices;
	vector<vector<int>> meshIndex;
	for (int i = 0; i < mesh.size(); ++i) {
		meshIndex.emplace_back(mesh[i].size());
		for (int j = 0; j < mesh[i].size(); ++j) {
			meshIndex[i][j] = vertices.size();
			vertices.emplace_back(mesh[i][j]);
		}
	}

	vector<tuple<int, int, int>> triangles;
	for (int i = 0; i + 1 < (int) mesh.size(); ++i)
		for (int j = 0; j + 1 < (int)mesh[i].size() && j + 1 < (int)mesh[i + 1].size(); ++j) {
			triangles.emplace_back(make_tuple(meshIndex[i][j], meshIndex[i + 1][j],
				meshIndex[i + 1][j + 1]));
			triangles.emplace_back(make_tuple(meshIndex[i][j], meshIndex[i][j + 1],
				meshIndex[i + 1][j + 1]));
		}

	return make_unique<Mesh>(std::move(vertices), std::move(triangles));
}

Pnt3f PatchSurface::pointAt(const vector<vector<Pnt3f>> &controlPoints, float s, float t) {
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
