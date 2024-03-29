#pragma once
// CS559 Train Project - 
// TrainWindow class description
//
// this class defines the window in which the project runs - its the
// outer windows that contain all of the widgets, including the "TrainView"
// which has the actual OpenGL window in which the train is drawn
//
// You might want to modify this class to add new widgets for controlling
// your train
//
// This takes care of lots of things - including installing itself into the
// FlTk "idle" loop so that we get periodic updates (if we're running the train).
//
// Written by Mike Gleicher, October 2008

//
// since this is an FLTk Window, we need to include FlTk Headers
// the pragmas turn off the warnings from FlTk - our code should have no warnings,
// but their code doesn't meet that standard
#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#include <Fl/Fl_Double_Window.h>
#include <Fl/Fl_Button.h>
#include <Fl/Fl_Group.H>
#include <Fl/Fl_Value_Slider.H>
#include <Fl/Fl_Browser.H>
#include <FL/Fl_Hor_Value_Slider.H>
#include <FL/Fl_Counter.H>

#pragma warning(pop)

// we need to know what is in the world to show
#include "World.H"

// other things we just deal with as pointers, to avoid circular references
class TrainView;

class TrainWindow : public Fl_Double_Window {
public:
	TrainWindow(const int x=50, const int y=50);

	// keep track of the stuff in the world
	World world;

	// the widgets that make up the Window
	TrainView* trainView;

	Fl_Group* widgets;	// all widgets, grouped for resizing ease

	// utility buttons
	Fl_Button* resetButton;

	// which viewpoint are we drawing from
	Fl_Button* worldCam;
	Fl_Button* trainCam;
	Fl_Button* topCam;

	// the type of the spline (use its value to determine)
	Fl_Browser* splineBrowser;
	Fl_Browser* sampleBrowser;
	Fl_Browser* trackBrowser;

	// are we animating the train?
	Fl_Button* runButton;
	// if we're animating it, how fast should it go?
	Fl_Value_Slider* speed;

	//Cardinal Cubic Tension
	Fl_Value_Slider* tension;

	Fl_Button* arcLength;		// do we use arc length for speed?
	Fl_Button* physics;			// use physics, kinetic energy + potential energy

	Fl_Button* people; //button for showing people on the car
	Fl_Button* addScenery; //button for adding scenery item
	Fl_Button* removeScenery; //button for removing scenery item

	Fl_Button* trainLight; //button for train light mode
	Fl_Button* drawDuck; //button for duck trains

	Fl_Counter* numCars; //button for manipulating the number of cars

	// call this method when things change
	void damageMe();

	// this moves the train forward on the track - its up to you to do this
	// correctly. it gets called from the idle callback loop
	// it should handle forward and backwards
	void advanceTrain(float dir = 1);

	// simple helper function to set up a button
	void togglify(Fl_Button*, int state=0);

};
