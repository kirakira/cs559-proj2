#pragma once

#include "glm/glm.hpp"

class TrainWindow;
class World;
class Train;
class Track;

#include <GL/glew.h>
//
// You might change the TrainView in order to add different objects to
// be drawn, or adjust the lighting, or ...
//
// The TrainView also needs to handle its user interface events.
// since this is an FLTk Window, we need to include FlTk Headers
// the pragmas turn off the warnings from FlTk - our code should have no warnings,
// but their code doesn't meet that standard
#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#include <Fl/Fl_Gl_Window.h>
#pragma warning(pop)

// this uses the old ArcBall Code
#include "Utilities/ArcBallCam.H"
#include "SkyBox.h"
#include "Projector.h"
#include "BillBoardTree.h"
#include "Mesh.h"
#include <memory>
#include "Fireflies.h"

// note that this mainly takes care of the drawing and other window
// stuff
class TrainView : public Fl_Gl_Window
{
	bool glewInitialized;
	std::unique_ptr<Mesh> ground;
	std::unique_ptr<Mesh> tower;
	std::unique_ptr<Mesh> flag;
	std::unique_ptr<Mesh> pole;
	std::unique_ptr<Fireflies> fireflies;

	GLuint groundShaderProgram;
	GLuint poolShaderProgram;
	std::unique_ptr<SkyBox> skybox;
	GLuint skyBoxProgram;
	std::unique_ptr<Projector> projector;
	GLuint projectorProgram;
	std::unique_ptr<BillBoardTree> billboard;
	GLuint billboardProgram;

public:
	// note that we keep the "standard widget" constructor arguments
	TrainView(int x, int y, int w, int h, const char* l = 0);
	virtual ~TrainView();

	// overrides of important window things
	virtual int handle(int);
	virtual void draw();

	// all of the actual drawing happens in this routine
	// it has to be encapsulated, since we draw differently if
	// we're drawing shadows (no colors, for example)
	void drawStuff(const Pnt3f &light, bool doingShadows=false);

	//init shaders for our objects!
	bool initGround();
	bool initSkyBox();
	bool initProjector();
	bool initBillBoard();
	bool initTower();
	bool initFlag();

	// setup the projection - assuming that the projection stack has been
	// cleared for you
	void setProjection();

	// keep an ArcBall for the UI
	ArcBallCam arcball;
	void resetArcball();

	void moveFireflies();

	// pick a point (for when the mouse goes down)
	void doPick();
	// simple - just remember which cube is selected
	int selectedCube;

	// easy access to parent
	TrainWindow* tw;
	World* world;

	Train* train;
	Track* track;

	glm::mat4 viewMatrix;

	void setViewMatrix();
	glm::mat4 getViewMatrix();
	glm::mat4 getModelMatrix();
	static glm::mat4 getNormalMatrix();
	static glm::mat4 getModelViewMatrix();
	static glm::mat4 getProjectionMatrix();

	static void bindUniformMatrix(GLuint shader, const char* name, GLfloat *value);
};
