// CS559 Utility Code
// - implementation of simple 3D point class
// see the header file for details
//
// file put together by Mike Gleicher, October 2008
//

#include "Pnt3f.H"
#include "math.h"
#include <cstdio>

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

void Pnt3f::normalize()
{
	float l = x*x + y*y + z*z;
	if (l<.000001) {
		x = 0;
		y = 1;
		z = 0;
	} else {
		l = sqrt(l);
		x /= l;
		y /= l;
		z /= l;
	}
}

void Pnt3f::print() {
	printf("(%f, %f, %f) ", x, y, z);
}