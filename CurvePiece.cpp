#include "CurvePiece.h"

CurvePiece::CurvePiece()
{
}


CurvePiece::~CurvePiece()
{
}

//generate or sample the points along one piece of curve
void CurvePiece::slicePoints(vector<Pnt3f> &pos, vector<Pnt3f> &orient, vector<Pnt3f> &tan, int type) {
	pos.clear(); orient.clear(); tan.clear();

	if (type == PieceSample::uniform) {
		for (int i = 0; i <= CurvePiece::SLICE; i++) {
			float t = (float)i / (float)CurvePiece::SLICE;

			Pnt3f position(positionAt(t, false));
			Pnt3f orientation(positionAt(t, true));
			Pnt3f tangent(tangentAt(t));

			pos.push_back(position);
			orient.push_back(orientation);
			tan.push_back(tangent);
		}
	}
	else if (type == PieceSample::arclength) {
		float t = 0;
		do {
			Pnt3f position(positionAt(t, false)), orientation(positionAt(t, true)), tangent(tangentAt(t));
			pos.push_back(position); orient.push_back(orientation); tan.push_back(tangent);

			//use 2.0 to show more clear difference with uniform sampling
			t += 1.0f / tangent.length();
		} while (t <= 1.0f);
		t = 1.0f;
		Pnt3f position(positionAt(t, false)), orientation(positionAt(t, true)), tangent(tangentAt(t));
		pos.push_back(position); orient.push_back(orientation); tan.push_back(tangent);
	}
	else if (type == PieceSample::adaptive) {
		//adaptive sampling

		float t = 0;
		do {
			Pnt3f position(positionAt(t, false)), orientation(positionAt(t, true)), tangent(tangentAt(t));
			pos.push_back(position); orient.push_back(orientation); tan.push_back(tangent);

			float x = 1.0f;

			if (tan.size() >= 2) {
				Pnt3f prev_tan = tan[tan.size() - 2];
				Pnt3f cur_tan = Pnt3f(tangent);
				
				Pnt3f diff_tan = cur_tan - prev_tan;

				//use curvature to enlarge or decrease the step
				float ratio = diff_tan.length() / tangent.length();
				
				x = (float)sqrt((1 + 1.2 * ratio) / (ratio + 0.0005));

			}

			t += x / tangent.length();
		} while (t <= 1.0f);
		t = 1.0f;
		Pnt3f position(positionAt(t, false)), orientation(positionAt(t, true)), tangent(tangentAt(t));
		pos.push_back(position); orient.push_back(orientation); tan.push_back(tangent);
	}
}

//position, orient and tangent vector for linear interpolation
Pnt3f LinePiece::positionAt(float t, bool orientation) {
	Pnt3f p0, p1;
	if (!orientation) {
		p0 = startPoint.pos; p1 = endPoint.pos;
	}
	else {
		p0 = startPoint.orient; p1 = endPoint.orient;
	}
	return ((1 - t) * p0 + t * p1);
}
Pnt3f LinePiece::tangentAt(float t) {
	return (endPoint.pos - startPoint.pos);
}

//Interpolate the orient and position using the same way!

/*
Cardinal Spline:
http://en.wikipedia.org/wiki/Cubic_Hermite_spline#Cardinal_spline
*/

Pnt3f CardinalPiece::positionAt(float t, bool orientation) {
	Pnt3f p0, p1, m0, m1;
	float c = tension;

	if (!orientation) {
		p0 = Pnt3f(startPoint.pos);
		p1 = Pnt3f(endPoint.pos);
		m0 = Pnt3f((1 - c) * 0.5f * (endPoint.pos - ctrlPoint1.pos));
		m1 = Pnt3f((1 - c) * 0.5f * (ctrlPoint2.pos - startPoint.pos));
	}
	else {
		p0 = Pnt3f(startPoint.orient);
		p1 = Pnt3f(endPoint.orient);
		m0 = Pnt3f((1 - c) * 0.5f * (endPoint.orient - ctrlPoint1.orient));
		m1 = Pnt3f((1 - c) * 0.5f * (ctrlPoint2.orient - startPoint.orient));
	}

	return
		Pnt3f(
			(2 * t*t*t - 3 * t*t	 + 1) * p0 +
			(	 t*t*t - 2 * t*t + t    ) * m0 + 
			(-2* t*t*t + 3 * t*t        ) * p1 + 
			(    t*t*t -     t*t        ) * m1
		);
}

Pnt3f CardinalPiece::tangentAt(float t) {
	Pnt3f p0, p1, m0, m1;
	float c = tension;

	p0 = Pnt3f(startPoint.pos);
	p1 = Pnt3f(endPoint.pos);
	m0 = Pnt3f((1 - c) * 0.5f * (endPoint.pos - ctrlPoint1.pos));
	m1 = Pnt3f((1 - c) * 0.5f * (ctrlPoint2.pos - startPoint.pos));
	
	return
		Pnt3f(
			(6 * t*t - 6 * t    ) * p0 +
			(3 * t*t - 4 * t + 1) * m0 +
			(-6* t*t + 6 * t    ) * p1 +
			(3 * t*t - 2 * t    ) * m1
		);
}

//special case for cardinal cubic spline
//reference: matrix approximation in http://en.wikipedia.org/wiki/Cubic_Hermite_spline#Cardinal_spline

Pnt3f CatmullRomPiece::positionAt(float t, bool orientation) {
	Pnt3f p0, p1, m0, m1;
	if (!orientation) {
		p0 = Pnt3f(startPoint.pos); p1 = Pnt3f(endPoint.pos);
		m0 = Pnt3f(ctrlPoint1.pos); m1 = Pnt3f(ctrlPoint2.pos);
	}
	else {
		p0 = Pnt3f(startPoint.orient); p1 = Pnt3f(endPoint.orient);
		m0 = Pnt3f(ctrlPoint1.orient); m1 = Pnt3f(ctrlPoint2.orient);
	}
	Pnt3f point(
		0.5f * 
		(	(t * ((2 - t) * t - 1)) * m0 +
			(t * t * (3 * t - 5) + 2) * p0 +
			(t * ((4 - 3 * t) * t + 1)) * p1 +
			((t-1) * t * t) * m1
		)
	);
	return point;
}

Pnt3f CatmullRomPiece::tangentAt(float t) {
	Pnt3f p0(startPoint.pos), p1(endPoint.pos), m0(ctrlPoint1.pos), m1(ctrlPoint2.pos);
	Pnt3f tan(
		0.5f * (
			(-3 * t*t + 4 * t - 1)		* m0 + 
			(9  * t*t - 10 * t)			* p0 + 
			(-9 * t*t + 8 * t + 1)	* p1 + 
			(3 * t*t - 2 * t)			* m1
		)
	);
	return tan;
}

//reference: <<Fundamentals of Computer Graphics>>

Pnt3f BSplinePiece::positionAt(float t, bool orientation) {
	Pnt3f p0, p1, p2, p3, p4;
	if (!orientation) {
		p0 = Pnt3f(ctrlPoint1.pos);
		p1 = Pnt3f(ctrlPoint2.pos);
		p2 = Pnt3f(ctrlPoint3.pos);
		p3 = Pnt3f(startPoint.pos);
		p4 = Pnt3f(endPoint.pos);
	}
	else {
		p0 = Pnt3f(ctrlPoint1.orient);
		p1 = Pnt3f(ctrlPoint2.orient);
		p2 = Pnt3f(ctrlPoint3.orient);
		p3 = Pnt3f(startPoint.orient);
		p4 = Pnt3f(endPoint.orient);
	}
	Pnt3f
		point(
		1.0f / 6.0f * (
		(-t*t*t + 3 * t*t - 3 * t + 1) * p0 +
		(3 * t*t*t - 6 * t*t + 4) * p1 +
		(-3 * t*t*t + 3 * t*t + 3 * t + 1) * p2 +
		(t*t*t) * p3
		)
		);
	return point;
}

Pnt3f BSplinePiece::tangentAt(float t) {
	Pnt3f p0, p1, p2, p3, p4;
	p0 = Pnt3f(ctrlPoint1.pos);
	p1 = Pnt3f(ctrlPoint2.pos);
	p2 = Pnt3f(ctrlPoint3.pos);
	p3 = Pnt3f(startPoint.pos);
	p4 = Pnt3f(endPoint.pos);
	Pnt3f
		tan(
		1.0f / 6.0f * (
		(-3 * t*t + 6  * t - 3) * p0 +
		(9  * t*t - 12 * t    ) * p1 +
		(-9 * t*t + 6  * t + 3) * p2 +
		( 3 * t*t             ) * p3

		)
		);
	return tan; 
}

Pnt3f BezierPiece::positionAt(float t, bool orientation) {
	//TODO
	return NULL;
}

Pnt3f BezierPiece::tangentAt(float t) {
	//TODO
	return NULL;
}
