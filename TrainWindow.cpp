// CS559 Train Project -
// Train Window class implementation
// - note: this is code that a student might want to modify for their project
//   see the header file for more details
// - look for the TODO: in this file
// - also, look at the "TrainView" - its the actual OpenGL window that
//   we draw into
//
// Written by Mike Gleicher, October 2008
//

#include "TrainWindow.H"
#include "TrainView.H"
#include "CallBacks.H"
#include "Train.h"
#include "Track.h"

#include <FL/fl.h>
#include <FL/Fl_Box.h>

// for using the real time clock
#include <time.h>



/////////////////////////////////////////////////////
TrainWindow::TrainWindow(const int x, const int y) : Fl_Double_Window(x,y,900,600,"Train and Roller Coaster")
{
	// make all of the widgets
	begin();	// add to this widget
	{
		int pty=5;			// where the last widgets were drawn

		trainView = new TrainView(5,5,590,590);
		trainView->tw = this;

		// Add train to the trainview
		trainView->train = new Train(trainView);
		// Add track to the window
		trainView->track = new Track(trainView);
			
		trainView->world = &world;
		this->resizable(trainView);

		// to make resizing work better, put all the widgets in a group
		widgets = new Fl_Group(600,5,400,590);
		widgets->begin();

		// camera buttons - in a radio button group
		Fl_Group* camGroup = new Fl_Group(600, pty, 295, 20);
		camGroup->begin();
		worldCam = new Fl_Button(610, pty, 90, 20, "World");
		worldCam->type(FL_RADIO_BUTTON);		// radio button
		worldCam->value(1);			// turned on
		worldCam->selection_color((Fl_Color)3); // yellow when pressed
		worldCam->callback((Fl_Callback*)damageCB, this);
		trainCam = new Fl_Button(705, pty, 90, 20, "Train");
		trainCam->type(FL_RADIO_BUTTON);
		trainCam->value(0);
		trainCam->selection_color((Fl_Color)3);
		trainCam->callback((Fl_Callback*)damageCB, this);
		topCam = new Fl_Button(800, pty, 90, 20, "Top");
		topCam->type(FL_RADIO_BUTTON);
		topCam->value(0);
		topCam->selection_color((Fl_Color)3);
		topCam->callback((Fl_Callback*)damageCB, this);
		camGroup->end();

		pty += 30;

		runButton = new Fl_Button(605,pty,60,20,"Run");
		togglify(runButton);

		Fl_Button* rb = new Fl_Button(700,pty,25,20,"@<<");
		rb->callback((Fl_Callback*)backCB,this);
		Fl_Button* fb = new Fl_Button(675, pty, 25, 20, "@>>");
		fb->callback((Fl_Callback*)forwCB, this);

		arcLength = new Fl_Button(730,pty,80,20,"ArcLength");
		togglify(arcLength,1);

		physics = new Fl_Button(815, pty, 80, 20, "Physics");
		togglify(physics, 0);

		pty+=25;

		speed = new Fl_Value_Slider(655,pty,240,20,"speed");
		speed->range(0,5);
		speed->value(2);
		speed->align(FL_ALIGN_LEFT);
		speed->type(FL_HORIZONTAL);

		pty += 40;

		// browser to select spline types
		// TODO(*) make sure these choices are the same as what the code supports
		splineBrowser = new Fl_Browser(605,pty,110,85,"Spline Type");
		splineBrowser->type(2);		// select
		splineBrowser->callback((Fl_Callback*)damageCB,this);
		splineBrowser->add("Linear");
		splineBrowser->add("Cardinal Cubic");
		splineBrowser->add("Catmull-Rom");
		splineBrowser->add("Cubic B-Spline");
		splineBrowser->select(3);

		sampleBrowser = new Fl_Browser(720, pty, 80, 85, "Sampling Type");
		sampleBrowser->type(2);
		sampleBrowser->callback((Fl_Callback*)damageCB, this);
		sampleBrowser->add("Uniform");
		sampleBrowser->add("Arc Length");
		sampleBrowser->add("Adaptive");
		sampleBrowser->select(2);


		trackBrowser = new Fl_Browser(805, pty, 90, 85, "Track Type");
		trackBrowser->type(2);
		trackBrowser->callback((Fl_Callback*)damageCB, this);
		trackBrowser->add("Simple");
		trackBrowser->add("Railway");
		trackBrowser->add("Highway");
		trackBrowser->select(2);

		pty += 110;

		tension = new Fl_Value_Slider(655, pty, 240, 20, "tension");
		tension->range(-0.99, 0.99);
		tension->value(0.1);
		tension->align(FL_ALIGN_LEFT);
		tension->type(FL_HORIZONTAL);
		tension->callback((Fl_Callback*)damageCB, this);

		pty += 50;

		// add and delete points
		Fl_Button* ap = new Fl_Button(605,pty,80,20,"Add Point");
		ap->callback((Fl_Callback*)addPointCB,this);
		Fl_Button* dp = new Fl_Button(690,pty,80,20,"Delete Point");
		dp->callback((Fl_Callback*)deletePointCB,this);

		pty += 25;
		// reset the points
		resetButton = new Fl_Button(735,pty,60,20,"Reset");
		resetButton->callback((Fl_Callback*)resetCB,this);
		Fl_Button* loadb = new Fl_Button(605,pty,60,20,"Load");
		loadb->callback((Fl_Callback*) loadCB, this);
		Fl_Button* saveb = new Fl_Button(670,pty,60,20,"Save");
		saveb->callback((Fl_Callback*) saveCB, this);

		pty += 25;
		// roll the points
		Fl_Button* rx = new Fl_Button(605,pty,30,20,"R+X");
		rx->callback((Fl_Callback*)rpxCB,this);
		Fl_Button* rxp = new Fl_Button(635,pty,30,20,"R-X");
		rxp->callback((Fl_Callback*)rmxCB,this);
		Fl_Button* rz = new Fl_Button(670,pty,30,20,"R+Z");
		rz->callback((Fl_Callback*)rpzCB,this);
		Fl_Button* rzp = new Fl_Button(700,pty,30,20,"R-Z");
		rzp->callback((Fl_Callback*)rmzCB,this);

		pty+=30;

		addScenery = new Fl_Button(605, pty, 100, 20, "Add Item");
		addScenery->callback((Fl_Callback*)addSceneryCB,this);
		removeScenery = new Fl_Button(705,pty,100,20,"Remove Item");
		removeScenery->callback((Fl_Callback*)removeSceneryCB, this);

		people = new Fl_Button(815, pty, 80, 20, "People");
		togglify(people, 0);

		pty += 25;

		trainLight = new Fl_Button(605, pty, 80, 20, "Train Light");
		togglify(trainLight, 0);

		numCars = new Fl_Counter(700, pty, 40, 20, "Num Cars");
		numCars->range(1.0f, 1.0f);
		numCars->value(4);
		numCars->callback((Fl_Callback*)damageCB, this);
		numCars->step(1.0f);
		numCars->type(FL_SIMPLE_COUNTER);

		drawDuck = new Fl_Button(760, pty, 80, 20, "Duck Car!");
		togglify(drawDuck, 0);

		// TODO add widgets for all of your fancier features here
#ifdef EXAMPLE_SOLUTION
		makeExampleWidgets(this,pty);
#endif

		// we need to make a little phantom widget to have things resize correctly
		Fl_Box* resizebox = new Fl_Box(600,595,300,5);
		widgets->resizable(resizebox);

		widgets->end();

	}
	end();	// done adding to this widget

	// set up callback on idle
	Fl::add_idle((void (*)(void*))runButtonCB,this);
}

// handy utility to make a button into a toggle
void TrainWindow::togglify(Fl_Button* b, int val)
{
    b->type(FL_TOGGLE_BUTTON);		// toggle
    b->value(val);		// turned off
    b->selection_color((Fl_Color)3); // yellow when pressed	
	b->callback((Fl_Callback*)damageCB,this);
}

void TrainWindow::damageMe()
{
	if (trainView->selectedCube >= ((int)world.points.size()))
		trainView->selectedCube = 0;
	trainView->damage(1);
}

/////////////////////////////////////////////////////
// this will get called (approximately) 30 times per second
// if the run button is pressed
void TrainWindow::advanceTrain(float dir)
{
	// TODO make this work for your train

	if (!arcLength->value()) {
		world.trainU += dir * ((float)speed->value() * 0.03f);
	} else {
		if (!physics->value()) {
			float velocity = dir * (float)speed->value() * 1.0f;
			//use arc-length for delta_U
			world.trainU += trainView->track->arcLengthDelta(velocity);
		}
		else {
			float V = (float)speed->value(); //speed->value() as the initial value at trainU = 0
			float initial_H = trainView->track->getTrackVector(0, VectorType::position).y;
			float cur_H = trainView->track->getTrackVector(world.trainU, VectorType::position).y;

			//calculate the kinetic energy from the initial kinetic energy
			float Kinetic = 0.5f * V * V - (cur_H - initial_H) * 0.1f;
			float velocity;
			if (Kinetic <= 0.001f) {
				//reverse direction if velocity reaches 0
				world.recordDir = -world.recordDir;
				velocity = world.recordDir * 0.1f;
			}
			else {
				velocity = world.recordDir * (fabs(Kinetic) * 2.0f);
			}

			world.trainU += trainView->track->arcLengthDelta(velocity);

		}
	}

	world.trainU = trainView->track->uniformU(world.trainU);

#ifdef EXAMPLE_SOLUTION
	// note - we give a little bit more example code here than normal,
	// so you can see how this works

	if (arcLength->value()) {
		float vel = ew.physics->value() ? physicsSpeed(this) * (float)speed->value() : dir * (float)speed->value();
		world.trainU += arclenVtoV(world.trainU, vel, this);
	} else {
		world.trainU +=  dir * ((float)speed->value() * .1f);
	}

	float nct = static_cast<float>(world.points.size());
	if (world.trainU > nct) world.trainU -= nct;
	if (world.trainU < 0) world.trainU += nct;
#endif
}