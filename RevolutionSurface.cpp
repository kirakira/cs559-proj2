#include "RevolutionSurface.h"
#include "CurvePiece.h"
#include <glm/gtx/transform.hpp>
#include <iostream>

using namespace std;

unique_ptr<Mesh> RevolutionSurface::generate(vector<Pnt3f> controlPoints) {
	float delta = (float) .1;
	vector<vector<pair<glm::vec3, glm::vec3>>> grid;
	for (float t = 0; t < 1; t += delta) {
		grid.emplace_back();
		auto mat = glm::rotate(t * 360, glm::vec3(0, 0, 1));
		for (float s = 0; s < controlPoints.size() - 3; s += delta) {
			int i = (int)s;
			CardinalPiece cp(controlPoints[i + 1], controlPoints[i + 2],
				controlPoints[i], controlPoints[i + 3], .5);
			auto p = cp.positionAt(s - i);
			auto position = glm::vec3(mat * glm::vec4(p.x, p.y, p.z, 1));
			auto tan = mat * glm::vec4(cp.tangentAt(s - i).toGLM(), 1);
			auto normal = glm::cross(glm::vec3(-position.y, position.x, 0), glm::vec3(tan));
			grid.back().emplace_back(make_pair(position, normal));
		}
	}

	vector<Vertex> vertices;
	vector<vector<int>> grid_index;
	for (int i = 0; i < (int)grid.size(); ++i) {
		grid_index.emplace_back(grid[i].size());
		for (int j = 0; j < (int)grid[i].size(); ++j) {
			grid_index[i][j] = vertices.size();
			vertices.emplace_back(grid[i][j].first, grid[i][j].second);
		}
	}

	vector<tuple<int, int, int>> triangles;
	for (int i = 0; i < (int) grid.size(); ++i) {
		int ni = (i + 1) % grid.size();
		for (int j = 0; j + 1 < (int) grid[i].size() && j + 1 < (int) grid[ni].size(); ++j) {
			triangles.emplace_back(make_tuple(grid_index[i][j], grid_index[ni][j], grid_index[ni][j + 1]));
			triangles.emplace_back(make_tuple(grid_index[i][j], grid_index[i][j + 1], grid_index[ni][j + 1]));
		}
	}

	return make_unique<Mesh>(std::move(vertices), std::move(triangles));
}