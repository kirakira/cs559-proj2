#pragma once

//Created 03-26-15
#include "TrainView.H"
#include "ControlPoint.H"

#define TRAIN_BODY_LENGTH 10

#define TRAIN_CARS 3
#define TRAIN_SPACING_RATIO 1.4f

#define TRAIN_BOTTOM_BASE_HEIGHT 1.5f
#define TRAIN_BOTTOM_TOP_HEIGHT 3

#define TRAIN_DRIVER_ROOM_HEIGHT 5

#define TRAIN_ENGINE_CYLINDER_RADIUS 2

#define TRAIN_ENGINE_CHIMNEY_HEIGHT 4
#define TRAIN_ENGINE_CHIMNEY_RADIUS 0.5f

#define TRAIN_WHEEL_RADIUS 1
#define TRAIN_WHEEL_THICK 0.3f

#define TRAIN_PEOPLE_SPACING 1.0f
#define TRAIN_PEOPLE_NUM 2
#define TRAIN_CAR_BLOCK_HEIGHT 3.0f

#define TRAIN_PEOPLE_HEIGHT 2.0f
#define TRAIN_PEOPLE_RADIUS 1.0f
#define TRAIN_PEOPLE_ARM_LENGTH 1.0f

enum TrainType {
	trainEngine = 0,
	trainCar = 1,
	duck = 2
};
class Train
{
public:

	Train(TrainView *tv);
	Train();
	~Train();

	void draw(bool doingShadow, int type, bool armOn);
	void drawWheels(bool doingShadow);
	
	TrainView *tv;
private:

	void drawEngineBottom();
	void drawEngineDriverRoom();
	void drawEngineCylinder();
	void drawEngineChimney();
	void drawEngineFrontBase();

	void drawCarBlock();
	void drawPeople(bool armOn);

	void drawDuck();
};

