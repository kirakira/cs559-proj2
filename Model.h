#pragma once

#include <map>
#include <string>
#include <vector>
#include <tuple>
#include <memory>

#include "Utilities/Pnt3f.h"
#include "FL/gl.h"

class Scalar {
	bool constant;
	int vi;
	float v;

public:
	explicit Scalar(int variableIndex);
	explicit Scalar(float value);

	float bind(const std::vector<float> &variableValues);
};

class Model
{
	Model() = default;

	// Methods for initializing a model - to be called by ModelLoader
	void addConvex(const std::vector<tuple<Scalar, Scalar, Scalar>> &points);
	void beginScope();
	void endScope();
	void translate(Scalar x, Scalar y, Scalar z);
	void rotate(Scalar angle, Scalar x, Scalar z);
	void scale(Scalar x, Scalar y, Scalar z);
	void callModel(int modelIndex, const std::vector<Scalar> &arguments);
	void setModel(int modelIndex, const Model *model);

	static std::vector<std::tuple<int, int, int>> convexHull(const std::vector<Pnt3f> &points);

	friend class ModelLoader;

public:
	virtual ~Model() = default;

	void render(const std::vector<float> &variableValues, GLuint shader);
};

class ModelLoader
{
	std::map<std::string, std::unique_ptr<Model>> models;

public:
	// returns the names of successfully loaded models
	std::vector<std::string> load(const std::string &filename);

	Model *getModel(const std::string &name) const;
};