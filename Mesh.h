#pragma once

#include "FL/gl.h"
#include <vector>
#include <tuple>
#include <set>
#include <map>
#include "Utilities/Pnt3f.h"

class Mesh
{
	std::vector<Pnt3f> vertices;
	std::vector<std::tuple<int, int, int>> faces;
	std::vector<std::set<int>> vertexNeighbours; // indices of faces adjacent to a given vertex

	GLuint vao, vbo;

	void associateFace(const std::tuple<int, int, int> &face, int fi,
		std::vector<std::set<int>> &vertexNeighbours) const;
	std::vector<int> adjacentVertices(int v) const;

	Pnt3f subdivideEdge(int u, int v, const std::vector<std::vector<int>> &adj,
		const std::map<std::pair<int, int>, std::vector<int>> &commonNeighbours) const;
	Pnt3f semiregularSubdivideEdge(int regular, int irregular, const std::vector<std::vector<int>> &adj) const;

	void cleanup();
	void initVertexArray();

public:
	Mesh(std::vector<Pnt3f> vertices, std::vector<std::tuple<int, int, int>> faces);
	virtual ~Mesh();

	void modifiedButterfly();
	void draw(GLuint shader);
};
