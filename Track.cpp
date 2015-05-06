#include "Track.h"
#include "TrainView.H"
#include "ControlPoint.H"
#include "TrainWindow.H"
#include "CurvePiece.h"
#include "Train.h"

#include "FL/gl.h"
#include "FL/glu.h"
#include "FL/glut.H"
#include "Fl/Fl_Browser.H"
#include <vector>

Track::Track(TrainView *_tv) {
	this->tv = _tv;
}

Track::Track() {
}

Track::~Track() {
}

vector<CurvePiece*> Track::genPieces() {

	vector<CurvePiece*> ret;

	vector<ControlPoint> ctrlPoints = tv->tw->world.points;
	int type = 1;
	while (type < NUM_CURVE_TYPE && !tv->tw->splineBrowser->selected(type)) {
		type++;
	}
	type--;

	//Construct curve pieces based on segments between control points
	int total = ctrlPoints.size();
	for (int i = 0; i < total; i++) {
		ControlPoint startPoint = ctrlPoints[i];
		ControlPoint endPoint = ctrlPoints[(i + 1) % total];

		CurvePiece *piece = NULL;
		switch (type) {
		case (CurveType::line) :
			piece = new LinePiece(ctrlPoints[i], ctrlPoints[(i + 1) % total]);
			break;
		case (CurveType::cardinal) :
			piece = new CardinalPiece(
				ctrlPoints[i], ctrlPoints[(i + 1) % total],
				ctrlPoints[(i - 1 + total) % total], ctrlPoints[(i + 2) % total], (float)tv->tw->tension->value()
			);
			break;
		case (CurveType::catmullrom) :
			piece = new CatmullRomPiece(
				ctrlPoints[i], ctrlPoints[(i + 1) % total],
				ctrlPoints[(i - 1 + total) % total], ctrlPoints[(i + 2) % total]
			);
			break;
		case (CurveType::bspline) :
			piece = new BSplinePiece(
				ctrlPoints[i], ctrlPoints[(i + 1) % total],
				ctrlPoints[(i - 3 + total) % total], ctrlPoints[(i - 2 + total) % total], ctrlPoints[(i - 1 + total) % total]
			);
		}
		if (piece == NULL) {
			fprintf(stderr, "Wrong curve piece type!");
			break;
		}

		ret.push_back(piece);
	}

	return ret;
}


Pnt3f Track::getTrackVector(float U, const VectorType &type) {
	vector<CurvePiece*> pieces = genPieces();
	int n = pieces.size();

	Pnt3f ret = getTrackVector(U, type, pieces);

	for (int i = 0; i < n; i++)
		delete pieces[i];

	return ret;
}

Pnt3f Track::getTrackVector(float U, const VectorType &type, const vector<CurvePiece*> &pieces) {
	int n = pieces.size();
	int cur = (int)U;
	if (cur >= n) {
		cur = 0;
		U -= (float)n;
	}
	float seg_u = U - (float)cur;

	Pnt3f ret;
	switch (type) {
	case (VectorType::position) :
		ret = pieces[cur]->positionAt(seg_u, false);
		break;
	case (VectorType::tangent) :
		ret = pieces[cur]->tangentAt(seg_u);
		break;
	case (VectorType::orient) :
		ret = pieces[cur]->positionAt(seg_u, true);
		break;
	}
	return ret;
}


//Starting from trainU
float Track::arcLengthDelta(float arc) {

	return arcLengthDelta(tv->tw->world.trainU, arc);
}

//arc length calculation
//several choices: approximation, tangent approximation ( arc / tan length ), 
const float arcLengthUnit = 1.0f;


float Track::arcLengthDelta(float U, float arc) {

	vector<CurvePiece*> pieces = genPieces();
	int n = pieces.size();

	float ret = 0.f;
	float dir = (arc > 0 ? 1.f : -1.f);

	while (fabs(arc) > arcLengthUnit) {
		//go every unit arc length to get the delta_U
		Pnt3f tan = getTrackVector(U, VectorType::tangent, pieces);
		float deltaU = dir * arcLengthUnit / tan.length();
		ret += deltaU;
		U += deltaU; U = uniformU(U);
		arc -= dir * arcLengthUnit;
		//update U and arc, step forward
	}
	Pnt3f tan = getTrackVector(U, VectorType::tangent, pieces);
	float deltaU = arc / tan.length();
	ret += deltaU;
	
	for (int i = 0; i < n; i++)
		delete pieces[i];

	return ret;

}

void Track::draw(bool doingShadow) {
	vector<CurvePiece*> curve_pieces = genPieces();
	int n = curve_pieces.size();

	int sample_type = 1;
	while (sample_type < NUM_SAMPLE_TYPE && !tv->tw->sampleBrowser->selected(sample_type)) {
		sample_type++;
	}
	sample_type--;

	for (int i = 0; i < n; i++) {
		vector<Pnt3f> pos, orient, tan;
		curve_pieces[i]->slicePoints(pos, orient, tan, sample_type);
		drawTrackSlices(pos, orient, tan);
	}

	for (int i = 0; i < n; i++)
		delete curve_pieces[i];
}

void Track::drawTrackSlices(const vector<Pnt3f> &pos, const vector<Pnt3f> &orient, const vector<Pnt3f> &tan) {
	int n = pos.size();

	int type = 1;
	while (type < NUM_TRACK_TYPE && !tv->tw->trackBrowser->selected(type)) {
		type++;
	}
	type--;

	//simple track
	if (type == TrackType::simple) {
		glColor3f(0.2f, 0.2f, 0.2f);

		//connecting the curve centers
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < n; i++) {
			glVertex3f(pos[i].x, pos[i].y, pos[i].z);
		}
		glEnd();

		//calculate the left(right) side boundary points, connect them
		for (int i = 0; i < n; i++) {
			Pnt3f side = tan[i] * orient[i]; side.normalize();
			draw_segment(pos[i], pos[i] + side * TRACK_RADIUS);
			draw_segment(pos[i], pos[i] - side * TRACK_RADIUS);
			if (i > 0) {
				Pnt3f prev_side = tan[i - 1] * orient[i - 1]; prev_side.normalize();
				draw_segment(pos[i] + side * TRACK_RADIUS, pos[i - 1] + prev_side * TRACK_RADIUS);
				draw_segment(pos[i] - side * TRACK_RADIUS, pos[i - 1] - prev_side * TRACK_RADIUS);
			}
		}
	}
	else if (type == TrackType::railway) {
		//railway tracks

		float tieWidth = 0.2f, tieLong = TRACK_RADIUS * 1.1f, tieHeight = 0.5f;
		float railWidth = 0.3f, railHeight = 0.2f;

		Pnt3f prev_tangent, prev_side, prev_normal;

		for (int i = 0; i < n; i++) {
			Pnt3f tangent = tan[i]; tangent.normalize();
			Pnt3f side = tan[i] * orient[i]; side.normalize();
			Pnt3f normal = side * tangent; normal.normalize();

			//draw the ties
			Pnt3f
				p0(pos[i] - normal * railHeight + tangent * tieWidth + side * tieLong),
				p1(pos[i] - normal * railHeight - tangent * tieWidth + side * tieLong),
				p2(pos[i] - normal * railHeight - tangent * tieWidth - side * tieLong),
				p3(pos[i] - normal * railHeight + tangent * tieWidth - side * tieLong);
			Pnt3f
				p4(pos[i] - normal * (tieHeight + railHeight) + tangent * tieWidth + side * tieLong),
				p5(pos[i] - normal * (tieHeight + railHeight) - tangent * tieWidth + side * tieLong),
				p6(pos[i] - normal * (tieHeight + railHeight) - tangent * tieWidth - side * tieLong),
				p7(pos[i] - normal * (tieHeight + railHeight) + tangent * tieWidth - side * tieLong);
			
			glColor3f(94 / 255.f, 38 / 255.f, 18/255.f);
			draw_quad_block(p0, p1, p2, p3, p4, p5, p6, p7);

			//draw the parallel rails
			glColor3f(0.3f, 0.3f, 0.3f);
			if (i > 0) {
				{
					Pnt3f R1 = pos[i] + side * (TRACK_RADIUS - 1.0f), R0 = pos[i - 1] + prev_side * (TRACK_RADIUS - 1.0f);
					Pnt3f
						q0(R1 + side * railWidth),
						q1(R1 - side * railWidth),
						q2(R0 - prev_side * railWidth),
						q3(R0 + prev_side * railWidth),
						q4(R1 + side * railWidth - normal * railHeight),
						q5(R1 - side * railWidth - normal * railHeight),
						q6(R0 - prev_side * railWidth - prev_normal * railHeight),
						q7(R0 + prev_side * railWidth - prev_normal * railHeight);
					draw_quad_block(q0, q1, q2, q3, q4, q5, q6, q7);
				}
				{
					Pnt3f L1 = pos[i] - side * (TRACK_RADIUS - 1.0f), L0 = pos[i - 1] - prev_side * (TRACK_RADIUS - 1.0f);
					Pnt3f
						q0(L1 + side * railWidth),
						q1(L1 - side * railWidth),
						q2(L0 - prev_side * railWidth),
						q3(L0 + prev_side * railWidth),
						q4(L1 + side * railWidth - normal * railHeight),
						q5(L1 - side * railWidth - normal * railHeight),
						q6(L0 - prev_side * railWidth - prev_normal * railHeight),
						q7(L0 + prev_side * railWidth - prev_normal * railHeight);
					draw_quad_block(q0, q1, q2, q3, q4, q5, q6, q7);
				}
			}

			prev_tangent = tangent; prev_side = side; prev_normal = normal;
		}
	}
	else if (type == TrackType::highway) {
		Pnt3f prev_tangent, prev_side, prev_normal;

		const float roadHeight = 0.2f, roadExpand = TRACK_RADIUS + 1.0f;
		const float markWidth = 0.2f, markLong = 1.0f;

		for (int i = 0; i < n; i++) {
			Pnt3f tangent = tan[i]; tangent.normalize();
			Pnt3f side = tan[i] * orient[i]; side.normalize();
			Pnt3f normal = side * tangent; normal.normalize();

			//draw the yellow mark on the road surface

			{
				Pnt3f
					p0(pos[i] + side * markWidth),
					p1(pos[i] - side * markWidth),
					p2(pos[i] - side * markWidth + tangent * markLong),
					p3(pos[i] + side * markWidth + tangent * markLong);
				
				glColor3f(0.5f, 0.5f, 0.f);
				draw_quad_4pt(p0, p1, p2, p3);

			}

			glColor3f(0.2f, 0.2f, 0.2f);

			//draw the road blocks

			if (i > 0) {
				Pnt3f
					p0(pos[i] - side * roadExpand),
					p1(pos[i - 1] - prev_side * roadExpand),
					p2(pos[i - 1] + prev_side * roadExpand),
					p3(pos[i] + side * roadExpand),

					p4(pos[i] - side * roadExpand - normal * roadHeight),
					p5(pos[i - 1] - prev_side * roadExpand - prev_normal * roadHeight),
					p6(pos[i - 1] + prev_side * roadExpand - prev_normal * roadHeight),
					p7(pos[i] + side * roadExpand - normal * roadHeight);

				draw_quad_block(p0, p1, p2, p3, p4, p5, p6, p7);
			}

			prev_tangent = tangent; prev_side = side; prev_normal = normal;
		}
	}
}

//draw the whole train, separated with multiple cars

const int TRAIN_BODY_SAMPLE = 10;

void Track::drawTrain(bool doingShadow) {
	float U = tv->tw->world.trainU;
	int numCars = (int)tv->tw->numCars->value();

	for (int i = 0; i < numCars ; i++) {
		int type = 0;
		if (i == 0) {
			type = TrainType::trainEngine;
		}
		else {
			if (!tv->tw->drawDuck->value())
				type = TrainType::trainCar;
			else
				type = TrainType::duck;
		}
		drawTrainEach(doingShadow, U, type);

		//use arc length to calculate U value for the following cars
		U += arcLengthDelta(U, -TRAIN_BODY_LENGTH * TRAIN_SPACING_RATIO);
		U = uniformU(U);
	}
}

void Track::drawTrainEach(bool doingShadow, float U, int type) {
	vector<CurvePiece*> pieces = genPieces();
	int n = pieces.size();

	Pnt3f pos = getTrackVector(U, VectorType::position, pieces);
	Pnt3f tan = getTrackVector(U, VectorType::tangent, pieces);
	Pnt3f orient = getTrackVector(U, VectorType::orient, pieces);

	//get normalized local coordinate system based on tangent, orient

	tan.normalize();
	Pnt3f side = tan * orient;
	side.normalize();
	Pnt3f normal = side * tan;
	normal.normalize();
	
	// z-axis->tan,	x->side, y->normal
	
	GLfloat m[] = {
		side.x, side.y, side.z, 0.f,
		normal.x, normal.y, normal.z, 0.f,
		tan.x, tan.y, tan.z, 0.f,
		pos.x, pos.y, pos.z, 1.0f
	};

	bool armOn = false;
	if (tan.y < -0.00001f) {
		armOn = true;
	}

	glPushMatrix();
		glMultMatrixf(m);
		tv->train->draw(doingShadow, type, armOn);
	glPopMatrix();
	
	//draw wheels

	glColor3f(0.0f, 1.f, 0.f);

	for (int dir = -1; dir <= 1; dir += 2) {
		float wheel_U = U + arcLengthDelta(U, -(float)dir * TRAIN_BODY_LENGTH / 2.0f * 0.8f);
		wheel_U = uniformU(wheel_U);
		Pnt3f wheel_pos = getTrackVector(wheel_U, VectorType::position, pieces);
		Pnt3f wheel_tan = getTrackVector(wheel_U, VectorType::tangent, pieces);
		Pnt3f wheel_orient = getTrackVector(wheel_U, VectorType::orient, pieces);

		wheel_tan.normalize();
		Pnt3f wheel_side = wheel_tan * wheel_orient;
		wheel_side.normalize();
		Pnt3f wheel_normal = wheel_side * wheel_tan;
		wheel_normal.normalize();

		Pnt3f
			wheel_left_center = wheel_pos - wheel_side * (TRACK_RADIUS - 1.0f) + wheel_normal * TRAIN_WHEEL_RADIUS,
			wheel_right_center = wheel_pos + wheel_side * (TRACK_RADIUS - 1.0f) + wheel_normal * TRAIN_WHEEL_RADIUS;

		GLfloat back_left_m[] = {
			wheel_tan.x, wheel_tan.y, wheel_tan.z, 0.f,
			wheel_normal.x, wheel_normal.y, wheel_normal.z, 0.f,
			wheel_side.x, wheel_side.y, wheel_side.z, 0.f,
			wheel_left_center.x, wheel_left_center.y, wheel_left_center.z, 1.0f
		};

		glPushMatrix();
		glMultMatrixf(back_left_m);
		tv->train->drawWheels(doingShadow);
		glPopMatrix();

		GLfloat back_right_m[] = {
			wheel_tan.x, wheel_tan.y, wheel_tan.z, 0.f,
			wheel_normal.x, wheel_normal.y, wheel_normal.z, 0.f,
			-wheel_side.x, -wheel_side.y, -wheel_side.z, 0.f,
			wheel_right_center.x, wheel_right_center.y, wheel_right_center.z, 1.0f
		};

		glPushMatrix();
		glMultMatrixf(back_right_m);
		tv->train->drawWheels(doingShadow);
		glPopMatrix();
	}

	for (int i = 0; i < n; i++)
		delete pieces[i];
}

void Track::trainView(float aspect) {
	vector<CurvePiece*> curve_pieces = genPieces();
	int n = curve_pieces.size();
	
	//Move the eye to the head of the train
	float trainU = tv->tw->world.trainU;
	
	float eyeU = trainU + arcLengthDelta(TRAIN_BODY_LENGTH / 2.0f);
	eyeU = uniformU(eyeU);
	
	//get the local normalized orthogonal coordinate system
	int cur = (int)eyeU;
	float seg_u = eyeU - (float)cur;
	
	Pnt3f pos = curve_pieces[cur]->positionAt(seg_u, false);
	Pnt3f tan = curve_pieces[cur]->tangentAt(seg_u);
	Pnt3f orient = curve_pieces[cur]->positionAt(seg_u, true);
	
	for (int i = 0; i < n; i++)
		delete curve_pieces[i];

	tan.normalize();
	Pnt3f side = tan * orient;
	side.normalize();
	Pnt3f normal = side * tan;
	normal.normalize();

	pos = pos + normal * (TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_ENGINE_CYLINDER_RADIUS);
	Pnt3f center = pos + tan * 100.f;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	//use perspective here
	gluPerspective(45, aspect, 0.1f, 200.f);
	//glOrtho(-wi, wi, -he, he, -200, 200);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	//look at from eye's position to a point along the looking direction, along with normal(heading up) vector

	gluLookAt(pos.x, pos.y, pos.z,
		center.x, center.y, center.z, 
		normal.x, normal.y, normal.z
	);

}

//reference: http://www.opengl.org.ru/docs/pg/0504.html

void Track::setupTrainLight() {
	vector<CurvePiece*> curve_pieces = genPieces();
	int n = curve_pieces.size();

	//Move the eye to the head of the train
	float trainU = tv->tw->world.trainU;

	float eyeU = trainU + arcLengthDelta(TRAIN_BODY_LENGTH / 2.0f);
	eyeU = uniformU(eyeU);

	//get the local orthogonal coordinate system
	int cur = (int)eyeU;
	float seg_u = eyeU - (float)cur;

	Pnt3f pos = curve_pieces[cur]->positionAt(seg_u, false);
	Pnt3f tan = curve_pieces[cur]->tangentAt(seg_u);
	Pnt3f orient = curve_pieces[cur]->positionAt(seg_u, true);

	tan.normalize();
	Pnt3f side = tan * orient;
	side.normalize();
	Pnt3f normal = side * tan;
	normal.normalize();

	pos = pos + normal * (TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_ENGINE_CYLINDER_RADIUS);
	
	GLfloat pos_arr[] = { pos.x, pos.y, pos.z, 1.f };
	GLfloat diffuse_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat ambient_color[] = { 0.7f, 0.7f, 0.7f, 1.0f };
	GLfloat specular_color[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat dir_arr[] = { tan.x, tan.y, tan.z, 1.f };
	float spot_cutoff = 180.f;
	float attenuation_constant = 1.0f;
	float attenuation_linear = 0.1f;
	float attenuation_quad =  0.15f ;

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT3);

	glLightfv(GL_LIGHT3, GL_POSITION, pos_arr);
	glLightfv(GL_LIGHT3, GL_DIFFUSE, diffuse_color);
	glLightfv(GL_LIGHT3, GL_AMBIENT, ambient_color);
	glLightfv(GL_LIGHT3, GL_SPECULAR, specular_color);
	glLightf(GL_LIGHT3, GL_SPOT_CUTOFF, spot_cutoff);
	glLightf(GL_LIGHT3, GL_CONSTANT_ATTENUATION, attenuation_constant);
	glLightf(GL_LIGHT3, GL_LINEAR_ATTENUATION, attenuation_linear);
	glLightf(GL_LIGHT3, GL_QUADRATIC_ATTENUATION, attenuation_quad);

	for (int i = 0; i < n; i++)
		delete curve_pieces[i];
}