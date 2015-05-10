#include "GL/glew.h"
#include "Mesh.h"
#include "glm/glm.hpp"
#include <map>
#include <cassert>
#include <algorithm>
#include <iostream>

using namespace std;

Mesh::Mesh(vector<Pnt3f> v, vector<tuple<int, int, int>> f)
	: vertices(std::move(v))
	, faces(std::move(f))
	, vertexNeighbours(vertices.size())
{
	for (int i = 0; i < (int)faces.size(); ++i)
		associateFace(faces[i], i, vertexNeighbours);
	initVertexArray();
}

Mesh::~Mesh() {
	cleanup();
}

void Mesh::cleanup() {
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}

void Mesh::associateFace(const tuple<int, int, int> &face, int index, vector<set<int>> &vertexNeighbours) const {
	vertexNeighbours[get<0>(face)].emplace(index);
	vertexNeighbours[get<1>(face)].emplace(index);
	vertexNeighbours[get<2>(face)].emplace(index);
}

void Mesh::modifiedButterfly() {
	vector<vector<int>> adj((int)vertices.size());
	for (int i = 0; i < (int)vertices.size(); ++i)
		adj[i] = adjacentVertices(i);
	
	map<pair<int, int>, vector<int>> commonNeighbours;
	for (const auto &face : faces) {
		int i = get<0>(face), j = get<1>(face), k = get<2>(face);
		commonNeighbours[make_pair(i, j)].emplace_back(k);
		commonNeighbours[make_pair(j, i)].emplace_back(k);
		commonNeighbours[make_pair(i, k)].emplace_back(j);
		commonNeighbours[make_pair(k, i)].emplace_back(j);
		commonNeighbours[make_pair(j, k)].emplace_back(i);
		commonNeighbours[make_pair(k, j)].emplace_back(i);
	}

	map<pair<int, int>, int> newVertices;
	for (const auto& face : faces) {
		int i = get<0>(face), j = get<1>(face), k = get<2>(face);
		if (newVertices.count(make_pair(i, j)) == 0) {
			auto v = subdivideEdge(i, j, adj, commonNeighbours);
			newVertices[make_pair(i, j)] = (int)vertices.size();
			newVertices[make_pair(j, i)] = (int)vertices.size();
			vertices.emplace_back(v);
		}
		if (newVertices.count(make_pair(i, k)) == 0) {
			auto v = subdivideEdge(i, k, adj, commonNeighbours);
			newVertices[make_pair(i, k)] = (int)vertices.size();
			newVertices[make_pair(k, i)] = (int)vertices.size();
			vertices.emplace_back(v);
		}
		if (newVertices.count(make_pair(k, j)) == 0) {
			auto v = subdivideEdge(k, j, adj, commonNeighbours);
			newVertices[make_pair(k, j)] = (int)vertices.size();
			newVertices[make_pair(j, k)] = (int)vertices.size();
			vertices.emplace_back(v);
		}
	}

	vector<tuple<int, int, int>> newFaces;
	vector<set<int>> newVertexNeighbours(vertices.size());
	for (const auto &f : faces) {
		int i = get<0>(f), j = get<1>(f), k = get<2>(f),
			ij = newVertices[make_pair(i, j)],
			ik = newVertices[make_pair(i, k)],
			kj = newVertices[make_pair(k, j)];
		newFaces.emplace_back(make_tuple(i, ij, ik ));
		associateFace(newFaces.back(), (int)newFaces.size() - 1, newVertexNeighbours);
		newFaces.emplace_back(make_tuple(ij, j, kj));
		associateFace(newFaces.back(), (int)newFaces.size() - 1, newVertexNeighbours);
		newFaces.emplace_back(make_tuple(ik, kj, k));
		associateFace(newFaces.back(), (int)newFaces.size() - 1, newVertexNeighbours);
		newFaces.emplace_back(make_tuple(ij, ik, kj));
		associateFace(newFaces.back(), (int)newFaces.size() - 1, newVertexNeighbours);
	}

	faces = std::move(newFaces);
	vertexNeighbours = std::move(newVertexNeighbours);

	cleanup();
	initVertexArray();
}

Pnt3f Mesh::semiregularSubdivideEdge(int regular, int irregular, const vector<vector<int>> &adj) const {
	int n = (int)adj[irregular].size();
	vector<float> w;
	if (n == 3)
		w = { 5.f / 12, -1.f / 12, -1.f / 12 };
	else if (n == 4)
		w = { 3.f / 8, 0, -1.f / 8, 0 };
	else {
		float PI = acos(-1);
		w.resize(n);
		for (int i = 0; i < n; ++i)
			w[i] = (.25 + cos(2 * PI * i / n) + .5 * cos(4 * PI * i / n)) / n;
	}
	float sum = 0;
	for (auto t : w)
		sum += t;
	assert(sum <= 1);

	int start = -1;
	for (int i = 0; i < n; ++i)
		if (adj[irregular][i] == regular)
			start = i;
	assert(start != -1);

	Pnt3f ans = (1 - sum) * vertices[irregular];
	for (int i = 0; i < n; ++i)
		ans = ans + w[i] * vertices[adj[irregular][(i + start) % n]];

	return ans;
}

Pnt3f Mesh::subdivideEdge(int u, int v, const vector<vector<int>> &adj,
	const map<pair<int, int>, vector<int>> &commonNeighbours) const {
	if (adj[u].size() == 6 && adj[v].size() == 6) {
		Pnt3f ans = (1.f / 2 + 1.f / 8) * (vertices[u] + vertices[v]);
		const auto &n1 = commonNeighbours.at(make_pair(u, v));
		assert(n1.size() == 2);
		ans = ans + 1. / 8 * (vertices[n1[0]] + vertices[n1[1]]);

		for (int n : n1) {
			{
				const auto &n2 = commonNeighbours.at(make_pair(u, n));
				assert(n2.size() == 2);
				for (int i = 0; i < 2; ++i)
					ans = ans - 1.f / 16 * vertices[n2[i]];
			}

			{
				const auto &n2 = commonNeighbours.at(make_pair(v, n));
				assert(n2.size() == 2);
				for (int i = 0; i < 2; ++i)
					ans = ans - 1.f / 16 * vertices[n2[i]];
			}
		}
		return ans;
	}
	else if ((adj[u].size() == 6 || adj[v].size() == 6) && min(adj[u].size(), adj[v].size()) >= 3) {
		if (adj[v].size() == 6)
			swap(u, v);
		return semiregularSubdivideEdge(u, v, adj);
	}
	else if (adj[u].size() != 6 && adj[v].size() != 6) {
		auto a = semiregularSubdivideEdge(u, v, adj),
			b = semiregularSubdivideEdge(v, u, adj);
		return .5f * (a + b);
	}
	else {
		cerr << "Butterfly: making up the boundary case" << endl;
		return .5 * (vertices[u] + vertices[v]);
	}
}

vector<int> Mesh::adjacentVertices(int v) const {
	map<int, vector<int>> graph;
	set<int> neighbours;
	for (int f : vertexNeighbours[v]) {
		if (get<0>(faces[f]) == v) {
			graph[get<1>(faces[f])].emplace_back(get<2>(faces[f]));
			graph[get<2>(faces[f])].emplace_back(get<1>(faces[f]));
			neighbours.emplace(get<1>(faces[f]));
			neighbours.emplace(get<2>(faces[f]));
		}
		else if (get<1>(faces[f]) == v) {
			graph[get<0>(faces[f])].emplace_back(get<2>(faces[f]));
			graph[get<2>(faces[f])].emplace_back(get<0>(faces[f]));
			neighbours.emplace(get<0>(faces[f]));
			neighbours.emplace(get<2>(faces[f]));
		}
		else {
			graph[get<0>(faces[f])].emplace_back(get<1>(faces[f]));
			graph[get<1>(faces[f])].emplace_back(get<0>(faces[f]));
			neighbours.emplace(get<0>(faces[f]));
			neighbours.emplace(get<1>(faces[f]));
		}
	}

	vector<int> ans;

	if (neighbours.size() == 0)
		return ans;

	vector<int> dfs_order;
	for (int n : neighbours) {
		assert(graph[n].size() >= 1 && graph[n].size() <= 2);
		if (graph[n].size() == 1)
			dfs_order.emplace_back(n);
	}

	if (dfs_order.empty())
		dfs_order.emplace_back(*neighbours.begin());

	set<int> visited;
	for (int v : dfs_order) {
		if (visited.count(v) > 0)
			continue;

		ans.emplace_back(v);
		visited.emplace(v);
		bool finish = false;
		while (!finish) {
			int last = ans.back();
			finish = true;
			for (int u : graph[last])
				if (visited.count(u) == 0) {
					ans.emplace_back(u);
					visited.emplace(u);
					finish = false;
				}
		}
	}

	return ans;
}

void Mesh::initVertexArray() {
	int n = faces.size();
	float *buffer = new float[n * 3 * 3];
	for (int i = 0; i < n; ++i) {
		vertices[get<0>(faces[i])].writeToBuffer(buffer + 9 * i);
		vertices[get<1>(faces[i])].writeToBuffer(buffer + 9 * i + 3);
		vertices[get<2>(faces[i])].writeToBuffer(buffer + 9 * i + 6);
	}

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, n * 3 * 3 * sizeof(float), buffer, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	delete[] buffer;
}

void Mesh::draw(GLuint shader) {
	glUseProgram(shader);

	glm::mat4 projM = glm::mat4(0.0f);
	glm::mat4 viewM = glm::mat4(0.0f);

	glGetFloatv(GL_PROJECTION_MATRIX, &projM[0][0]);
	glGetFloatv(GL_MODELVIEW_MATRIX, &viewM[0][0]);
	GLuint projectID = glGetUniformLocation(shader, "projectMatrix");
	GLuint modelViewID = glGetUniformLocation(shader, "modelViewMatrix");

	glUniformMatrix4fv(projectID, 1, GL_FALSE, &projM[0][0]);
	glUniformMatrix4fv(modelViewID, 1, GL_FALSE, &viewM[0][0]);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, faces.size() * 3);
	glBindVertexArray(0);
	glUseProgram(0);

	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
}