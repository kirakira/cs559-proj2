// CS559 Train Project
// TrainView class implementation
// see the header for details
// look for TODO: to see things you want to add/change
// 

#include "GL/glew.h"

#include "TrainView.H"
#include "TrainWindow.H"
#include "Train.H"
#include "Track.H"
#include "PatchSurface.h"
#include "Mesh.h"
#include "Model.h"
#include "RevolutionSurface.h"

#include "Utilities/3DUtils.H"

#include <Fl/fl.h>
#include <iostream>

// we will need OpenGL, and OpenGL needs windows.h

#include <windows.h>

#include "ShaderTools.H"
#include "GL/gl.h"
#include "GL/glu.h"

#include "SkyBox.h"

#ifdef EXAMPLE_SOLUTION
#include "TrainExample/TrainExample.H"
#endif

TrainView::TrainView(int x, int y, int w, int h, const char* l) : Fl_Gl_Window(x,y,w,h,l)
	, glewInitialized(false)
{
	mode( FL_RGB|FL_ALPHA|FL_DOUBLE | FL_STENCIL );

	resetArcball();
}

TrainView::~TrainView() {
	glDeleteProgram(groundShaderProgram);
}

void TrainView::resetArcball()
{
	// set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this,40,250,.2f,.4f,0);
}

// FlTk Event handler for the window
// TODO if you want to make the train respond to other events 
// (like key presses), you might want to hack this.
int TrainView::handle(int event)
{
	// see if the ArcBall will handle the event - if it does, then we're done
	// note: the arcball only gets the event if we're in world view
	if (tw->worldCam->value())
		if (arcball.handle(event)) return 1;

	// remember what button was used
	static int last_push;

	switch(event) {
		case FL_PUSH:
			last_push = Fl::event_button();
			if (last_push == 1) {
				doPick();
				damage(1);
				return 1;
			};
			break;
		case FL_RELEASE:
			damage(1);
			last_push=0;
			return 1;
		case FL_DRAG:
			if ((last_push == 1) && (selectedCube >=0)) {
				ControlPoint* cp = &world->points[selectedCube];

				double r1x, r1y, r1z, r2x, r2y, r2z;
				getMouseLine(r1x,r1y,r1z, r2x,r2y,r2z);

				double rx, ry, rz;
				mousePoleGo(r1x,r1y,r1z, r2x,r2y,r2z, 
						  static_cast<double>(cp->pos.x), 
						  static_cast<double>(cp->pos.y),
						  static_cast<double>(cp->pos.z),
						  rx, ry, rz,
						  (Fl::event_state() & FL_CTRL) != 0);
				cp->pos.x = (float) rx;
				cp->pos.y = (float) ry;
				cp->pos.z = (float) rz;
				damage(1);
			}
			break;
			// in order to get keyboard events, we need to accept focus
		case FL_FOCUS:
			return 1;
		case FL_ENTER:	// every time the mouse enters this window, aggressively take focus
				focus(this);
				break;
		case FL_KEYBOARD:
		 		int k = Fl::event_key();
				int ks = Fl::event_state();
				if (k=='p') {
					if (selectedCube >=0) 
						printf("Selected(%d) (%g %g %g) (%g %g %g)\n",selectedCube,
							world->points[selectedCube].pos.x,world->points[selectedCube].pos.y,world->points[selectedCube].pos.z,
							world->points[selectedCube].orient.x,world->points[selectedCube].orient.y,world->points[selectedCube].orient.z);
					else
						printf("Nothing Selected\n");
					return 1;
				};
				break;
	}

	return Fl_Gl_Window::handle(event);
}

// this is the code that actually draws the window
// it puts a lot of the work into other routines to simplify things
void TrainView::draw()
{
	if (!glewInitialized) {
		glewInitialized = true;
		GLenum err = glewInit();
		if (GLEW_OK != err)
			fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

		initGround();
		initSkyBox();
		initTower();
		initFlag();
		fireflies = make_unique<Fireflies>(100);
	}

	printf("w = %d\n", w());

	glViewport(0,0,w(),h());

	// clear the window, be sure to clear the Z-Buffer too
	
	//glClearColor(0,0,.3f,0);		// background should be blue
	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	setProjection();		// put the code to set up matrices here

	// TODO you might want to set the lighting up differently
	// if you do, 
	// we need to set up the lights AFTER setting up the projection

	// enable the lighting
	//glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_LIGHTING);

	/*
	if (!tw->trainLight->value()) {
		glEnable(GL_LIGHT0);
		// top view only needs one light
		if (tw->topCam->value()) {
			glDisable(GL_LIGHT1);
			glDisable(GL_LIGHT2);
		}
		else {
			glEnable(GL_LIGHT1);
			glEnable(GL_LIGHT2);
		}
		// set the light parameters
		GLfloat lightPosition1[] = { 0, 1, 1, 0 }; // {50, 200.0, 50, 1.0};
		GLfloat lightPosition2[] = { 1, 0, 0, 0 };
		GLfloat lightPosition3[] = { 0, -1, 0, 0 };
		GLfloat yellowLight[] = { 0.5f, 0.5f, .1f, 1.0 };
		GLfloat whiteLight[] = { 1.0f, 1.0f, 1.0f, 1.0 };
		GLfloat blueLight[] = { .1f, .1f, .3f, 1.0 };
		GLfloat grayLight[] = { .3f, .3f, .3f, 1.0 };

		glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
		glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

		glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
		glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

		glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
		glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);
	}
	else {
		//added for train light
		track->setupTrainLight();
	}*/

	// now draw the ground plane
	//setupFloor();
	//glDisable(GL_LIGHTING);

	Pnt3f light(-100, 100, 100);

	skybox->draw();
	ground->draw(groundShaderProgram, 0, light, fireflies->getPositions(), false);
	//glEnable(GL_LIGHTING);
	setupObjects();
	
	// we draw everything twice - once for real, and then once for
	// shadows
	drawStuff(light);
}

bool TrainView::initTower() {
	tower = RevolutionSurface::generate({ { 0, 10, -1 }, { 0, 9, 1 },
	{ 0, 6, 80 }, { 0, 10, 85 }, { 0, 0, 100 }, { 0, 0, 120 } });
	if (!tower)
		return false;
	//tower->modifiedButterfly();
	return true;
}

bool TrainView::initGround() {
	Pnt3f controlPoints[] = {
		{ -150, 0, -150 }, { -150, 0, -100 }, { -150, 0, -50 }, { -150, 0, 0 }, { -150, 0, 50 }, { -150, 0, 100 }, { -150, 0, 150 },
		{ -100, 0, -150 }, { -100, 0, -100 }, { -100, 20, -50 }, { -100, 0, 0 }, { -100, 0, 50 }, { -100, 0, 100 }, { -100, 0, 150 },
		{ -50, 0, -150 }, { -50, 0, -100 }, { -50, 0, -50 }, { -50, 0, 0 }, { -50, 0, 50 }, { -50, 0, 100 }, { -50, 0, 150 },
		{ 0, 0, -150 }, { 0, 0, -100 }, { 0, 30, -50 }, { 0, 0, 0 }, { 0, 0, 50 }, { 0, 0, 100 }, { 0, 0, 150 },
		{ 50, 0, -150 }, { 50, 0, -100 }, { 50, 0, -50 }, { 50, 10, 0 }, { 50, 0, 50 }, { 50, 0, 100 }, { 50, 0, 150 },
		{ 100, 0, -150 }, { 100, 0, -100 }, { 100, 0, -50 }, { 100, 0, 0 }, { 100, 0, 50 }, { 100, 0, 100 }, { 100, 0, 150 },
		{ 150, 0, -150 }, { 150, 0, -100 }, { 150, 0, -50 }, { 150, 0, 0 }, { 150, 0, 50 }, { 150, 0, 100 }, { 150, 0, 150 }
	};

	char *err;
	groundShaderProgram = loadShader("shaders/ground.vert", "shaders/ground.frag", err);
	if (groundShaderProgram == 0) {
		printf(err);
		return false;
	}
	ground = PatchSurface::generate(controlPoints, 7, 7, .1f);
	return ground != nullptr;
}

bool TrainView::initFlag() {
	char *err;
	poolShaderProgram = loadShader("shaders/pool.vert", "shaders/pool.frag", err);
	if (poolShaderProgram == 0) {
		printf(err);
		return false;
	}

	vector<Pnt3f> controlPoints;
	int n = 28, m = 17;
	for (int i = 0; i < n; ++i)
		for (int j = 0; j < m; ++j)
			controlPoints.emplace_back(i, 0, j);

	flag = PatchSurface::generate(&controlPoints[0], n, m, 1);
	if (!pool)
		return false;

	controlPoints = { { 0, 1, -100 }, { 0, 1, -45 }, { 0, 1, 20 }, { 0, 1, 40 } };
	pole = RevolutionSurface::generate(std::move(controlPoints));
	return pole != nullptr;
}

bool TrainView::initSkyBox() {
	
	char *err;
	skyBoxProgram = loadShader("shaders/skybox.vert", "shaders/skybox.frag", err);
	if (skyBoxProgram == 0) {
		printf(err);
		return false;
	}
	skybox = make_unique<SkyBox>(skyBoxProgram);
	return true;
}

// note: this sets up both the Projection and the ModelView matrices
// HOWEVER: it doesn't clear the projection first (the caller handles
// that) - its important for picking
void TrainView::setProjection()
{
	// compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(w()) / static_cast<float>(h());

	if (tw->worldCam->value())
		arcball.setProjection(false);
	else if (tw->topCam->value()) {
		float wi,he;
		if (aspect >= 1) {
			wi = 110;
			he = wi/aspect;
		} else {
			he = 110;
			wi = he*aspect;
		}
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-wi,wi,-he,he,-200,200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(90,1,0,0);
	} else {
		// TODO put code for train view projection here!
		track->trainView(aspect);
	}
}

// this draws all of the stuff in the world
// NOTE: if you're drawing shadows, DO NOT set colors 
// (otherwise, you get colored shadows)
// this gets called twice per draw - once for the objects, once for the shadows
// TODO if you have other objects in the world, make sure to draw them
void TrainView::drawStuff(const Pnt3f &light, bool doingShadows)
{
	// draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (!tw->trainCam->value()) {
		for(size_t i=0; i<world->points.size(); ++i) {
			if (!doingShadows) {
				if ( ((int) i) != selectedCube)
					glColor3ub(240,60,60);
				else
					glColor3ub(240,240,30);
			}
			world->points[i].draw();
		}
	}
	// draw the track
	// TODO call your own track drawing code
#ifdef EXAMPLE_SOLUTION
	drawTrack(this, doingShadows);
#endif

	// draw the train
	// TODO call your own train drawing code
	// don't draw the train if you're looking out the front window
	/*
	if (!tw->trainCam->value())
		train->draw(doingShadows);
	*/

	track->draw(doingShadows);

	if (!tw->trainCam->value())
		track->drawTrain(doingShadows);

	tw->world.drawItems(doingShadows);


	// Tower
	glPushMatrix();
	glTranslatef(-80, 0, 80);
	glRotatef(-90, 1, 0, 0);
	tower->draw(0, 0, light, fireflies->getPositions(), true);
	glPopMatrix();

	// Pool
	glPushMatrix();
	glTranslatef(40, 50, 40);
	glRotatef(-90, 1, 0, 0);
	flag->draw(poolShaderProgram, ((float)GetTickCount()) / 1000.f, light, fireflies->getPositions(), false);
	pole->draw(0, 0, light, fireflies->getPositions(), false);
	glPopMatrix();

	// Fireflies
	fireflies->draw();
}

void TrainView::moveFireflies() {
	if (glewInitialized)
		fireflies->randomMove();
}

// this tries to see which control point is under the mouse
// (for when the mouse is clicked)
// it uses OpenGL picking - which is always a trick
// TODO if you want to pick things other than control points, or you
// changed how control points are drawn, you might need to change this
void TrainView::doPick()
{
	make_current();		// since we'll need to do some GL stuff

	int mx = Fl::event_x(); // where is the mouse?
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	// set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	gluPickMatrix((double)mx, (double)(viewport[3]-my), 5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	for(size_t i=0; i<world->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		world->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;

	printf("Selected Cube %d\n",selectedCube);
}
