#pragma once

#include "ControlPoint.H"

#include <vector>

using namespace std;

#define NUM_CURVE_TYPE 4
#define NUM_SAMPLE_TYPE 3

enum CurveType {
	line = 0,
	cardinal = 1,
	catmullrom = 2,
	bspline = 3,
	bezier = 4
};

enum PieceSample {
	uniform = 0,
	arclength = 1,
	adaptive = 2
};

class CurvePiece
{
public:
	static const int SLICE = 50;

	ControlPoint startPoint, endPoint;
	ControlPoint ctrlPoint1, ctrlPoint2;
	float tension = 0;

	CurvePiece(const ControlPoint &startPoint, const ControlPoint &endPoint,
		const ControlPoint &ctrlPoint1, const ControlPoint &ctrlPoint2, const float ten = 0) {
		this->startPoint = startPoint; this->endPoint = endPoint;
		this->ctrlPoint1 = ctrlPoint1; this->ctrlPoint2 = ctrlPoint2;
		this->tension = ten;
	}

	CurvePiece();
	~CurvePiece();

	//TODO: get all slices
	void slicePoints(vector<Pnt3f> &pos, vector<Pnt3f> &orient, vector<Pnt3f> &tan, int type);

	//get the position at paramenter t
	virtual Pnt3f positionAt(float t, bool orientation = false) = 0;
	//get the tangent(direction) at parameter t
	virtual Pnt3f tangentAt(float t) = 0;
};

class LinePiece : public CurvePiece {
public:
	LinePiece(const ControlPoint &startPoint, const ControlPoint &endPoint,
		const ControlPoint &ctrlPoint1 = ControlPoint(),
		const ControlPoint &ctrlPoint2 = ControlPoint()):
		CurvePiece(startPoint, endPoint, ctrlPoint1, ctrlPoint2) {
	}

	Pnt3f positionAt(float t, bool orientation = false);
	Pnt3f tangentAt(float t);
};

class CardinalPiece : public CurvePiece {
public:
	CardinalPiece(const ControlPoint &startPoint, const ControlPoint &endPoint,
		const ControlPoint &ctrlPoint1,
		const ControlPoint &ctrlPoint2, float ten) : CurvePiece(startPoint, endPoint, ctrlPoint1, ctrlPoint2, ten) {
	}
	Pnt3f positionAt(float t, bool orientation = false);
	Pnt3f tangentAt(float t);
};

class CatmullRomPiece : public CurvePiece {
public:
	CatmullRomPiece(
		const ControlPoint &startPoint,
		const ControlPoint &endPoint,
		const ControlPoint &ctrlPoint1,
		const ControlPoint &ctrlPoint2) : CurvePiece(startPoint, endPoint, ctrlPoint1, ctrlPoint2) {
	}
	Pnt3f positionAt(float t, bool orientation = false);
	Pnt3f tangentAt(float t);
};

class BSplinePiece : public CurvePiece {
public:
	BSplinePiece(
		const ControlPoint &startPoint,
		const ControlPoint &endPoint,
		const ControlPoint &ctrlPoint1,
		const ControlPoint &ctrlPoint2,
		const ControlPoint &ctrlPoint3) : CurvePiece(startPoint, endPoint, ctrlPoint1, ctrlPoint2) {
		this->ctrlPoint3 = ctrlPoint3;
	}
	Pnt3f positionAt(float t, bool orientation = false);
	Pnt3f tangentAt(float t);
	
	ControlPoint ctrlPoint3;
};

class BezierPiece : public CurvePiece {
public:
	Pnt3f positionAt(float t, bool orientation = false);
	Pnt3f tangentAt(float t);
};
