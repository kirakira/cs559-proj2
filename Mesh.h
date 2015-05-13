#pragma once

#include "FL/gl.h"
#include <vector>
#include <tuple>
#include <set>
#include <map>
#include "Utilities/Pnt3f.h"
#include "ControlPoint.H"

struct Vertex {
	Pnt3f position;
	Pnt3f normal;

	Vertex() = default;
	Vertex(const glm::vec3 &pos, const glm::vec3 &norm)
		: position(pos)
		, normal(norm) {}
	Vertex(const Pnt3f &pos, const Pnt3f &norm)
		: position(pos.toGLM())
		, normal(norm.toGLM()) {}

	void writeToBuffer(float *buffer) const {
		position.writeToBuffer(buffer);
		normal.writeToBuffer(buffer + 3);
	}

	operator ControlPoint() const {
		return ControlPoint(position, normal);
	}
};

class Mesh
{
	std::vector<Vertex> vertices;
	std::vector<std::tuple<int, int, int>> faces;

	std::vector<std::set<int>> vertexNeighbours; // indices of faces adjacent to a given vertex
	int sidePointsCount;

	GLuint vao[3], vbo[3];

	void associateFace(const std::tuple<int, int, int> &face, int fi,
		std::vector<std::set<int>> &vertexNeighbours) const;
	std::vector<int> adjacentVertices(int v) const;

	Pnt3f subdivideEdge(int u, int v, const std::vector<std::vector<int>> &adj,
		const std::map<std::pair<int, int>, std::vector<int>> &commonNeighbours) const;
	Pnt3f semiregularSubdivideEdge(int regular, int irregular, const std::vector<std::vector<int>> &adj) const;

	void cleanup();
	void initVertexArray();

	void copy(const Mesh& m);
	void move(Mesh&& m);

	const bool drawNormals = false;

public:
	Mesh(std::vector<Vertex> vertices, std::vector<std::tuple<int, int, int>> faces);
	Mesh(const Mesh &m);
	virtual ~Mesh();

	Mesh& operator=(const Mesh &m);

	void modifiedButterfly();
	void draw(GLuint shader, float time, const Pnt3f &light,
		const std::vector<Pnt3f> &localLights, const Pnt3f &color, bool grid);
};
