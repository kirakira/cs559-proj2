#pragma once

#include "Train.h"
#include "TrainView.H"
#include "TrainWindow.H"
#include "ControlPoint.H"
#include "CurvePiece.H"

#define TRACK_RADIUS 3.f

#include <vector>
using namespace std;

enum VectorType {
	position = 0,
	tangent = 1,
	orient = 2
};

#define NUM_TRACK_TYPE 3

enum TrackType {
	simple = 0,
	railway = 1,
	highway = 2
};

class Track {
public:	

	Track(TrainView *tv);
	Track();
	~Track();

	vector<CurvePiece*> genPieces();
	//get vector for a specific U
	Pnt3f getTrackVector(float U, const VectorType &type);
	//get vector for a specific U using pieces constructed
	Pnt3f getTrackVector(float U, const VectorType &type, const vector<CurvePiece*> &pieces);

	//draw the tracks
	void draw(bool doingShadow);

	//draw track slices using vectors
	void drawTrackSlices(const vector<Pnt3f> &pos, const vector<Pnt3f> &orient, const vector<Pnt3f> &tan);

	//calculate the local coordinate system to draw the train
	void drawTrain(bool doingShadow);
	void drawTrainEach(bool doingShadow, float U, int type);

	//arc length calculation, deltaU for a delta Arc Length at specific U
	float arcLengthDelta(float arc);
	float arcLengthDelta(float U, float arc);

	//uniform function to put U in [0.0, Num of ControlPoints)
	float uniformU(float U) {
		return fmodf(U + (float)tv->tw->world.points.size(), (float)tv->tw->world.points.size());
	}

	//perspective train view
	void trainView(float aspect);

	//train light set up in front of the train, it will illuminate to the front of the train
	void setupTrainLight();

	TrainView *tv;
};

