// CS559 Utility Code
// - implementation of simple 3D point class
// see the header file for details
//
// file put together by Mike Gleicher, October 2008
//

#include "Pnt3f.H"
#include "math.h"
#include <cstdio>
#include <vector>

using namespace std;

const float Pnt3f::EPS = 1e-3f;

Pnt3f::Pnt3f() : x(0), y(0), z(0)
{
}

Pnt3f::Pnt3f(const float* iv) : x(iv[0]), y(iv[1]), z(iv[2])
{
}

Pnt3f::Pnt3f(const float _x, const float _y, const float _z) : x(_x), y(_y), z(_z)
{
}

Pnt3f::Pnt3f(const Pnt3f& pt): x(pt.x), y(pt.y), z(pt.z) {
}

Pnt3f::Pnt3f(const glm::vec3 &v) : x(v.x), y(v.y), z(v.z) {
}

Pnt3f::Pnt3f(initializer_list<float> l) {
	vector<float> v;
	for (auto iter = l.begin(); iter != l.end(); ++iter)
		v.emplace_back(*iter);
	if (v.size() > 0)
		x = v[0];
	if (v.size() > 1)
		y = v[1];
	if (v.size() > 2)
		z = v[2];
}

float Pnt3f::norm() const {
	return sqrt(x * x + y * y + z * z);
}

void Pnt3f::normalize()
{
	if (isZero()) {
		x = 0;
		y = 1;
		z = 0;
	}
	else {
		auto n = norm();
		x /= n;
		y /= n;
		z /= n;
	}
}

void Pnt3f::print() {
	printf("(%f, %f, %f) ", x, y, z);
}

void Pnt3f::writeToBuffer(float *buffer) const {
	buffer[0] = x;
	buffer[1] = y;
	buffer[2] = z;
}

bool Pnt3f::isZero() const {
	return norm() < EPS;
}

glm::vec3 Pnt3f::toGLM() const {
	return glm::vec3(x, y, z);
}