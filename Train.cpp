#include "Train.h"
#include "TrainView.H"
#include "ControlPoint.H"
#include "TrainWindow.H"
#include "World.H"
#include "Track.h"

#include "FL/gl.h"
#include "FL/glu.h"
#include "FL/glut.H"

#include <vector>

Train::Train(TrainView *_tv) {
	this->tv = _tv;
}

Train::Train() {
}

Train::~Train()
{
}

void Train::draw(bool doingShadow, int type, bool armOn) {
	
	if (type == TrainType::trainEngine) {
		//draw engine

		drawEngineBottom(); //draw the bottom base
		drawEngineFrontBase(); //draw the front of bottom base

		drawEngineDriverRoom(); //draw the driver's room behind the cylinder
		drawEngineCylinder(); //draw the engine cylinder
		drawEngineChimney(); //draw the chimney 

	}
	else if (type == TrainType::trainCar) {
		
		drawEngineBottom();
		drawEngineFrontBase();
		drawCarBlock(); //draw the raw rectangular block

		if (tv->tw->people->value()) {
			drawPeople(armOn); //draw people if people is on
		}
	}
	else if (type == TrainType::duck) {
		drawDuck(); //draw duck if duck is on
	}
	
}

//draw wheels at wheel local coordinate system
void Train::drawWheels(bool doingShadow) {

	glPushMatrix();
		glTranslatef(0.f, 0.f, -TRAIN_WHEEL_THICK);
		gluDisk(gluNewQuadric(), 0.f, TRAIN_WHEEL_RADIUS, 50, 5);
		gluCylinder(gluNewQuadric(), TRAIN_WHEEL_RADIUS, TRAIN_WHEEL_RADIUS, 2 * TRAIN_WHEEL_THICK, 50, 5);
	glPopMatrix();

}


void Train::drawEngineBottom() {
	//draw bottom hold
	Pnt3f
		p0(TRACK_RADIUS, TRAIN_BOTTOM_BASE_HEIGHT, -TRAIN_BODY_LENGTH / 2.f),
		p1(-TRACK_RADIUS, TRAIN_BOTTOM_BASE_HEIGHT, -TRAIN_BODY_LENGTH / 2.f),
		p2(-TRACK_RADIUS, TRAIN_BOTTOM_BASE_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f),
		p3(TRACK_RADIUS, TRAIN_BOTTOM_BASE_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);
	Pnt3f
		p4(TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, -TRAIN_BODY_LENGTH / 2.f),
		p5(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, -TRAIN_BODY_LENGTH / 2.f),
		p6(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f),
		p7(TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);

	glColor3f(41 / 255.f, 36 / 255.f, 33 / 255.f);
	draw_quad_block(p0, p1, p2, p3, p4, p5, p6, p7);

}


void Train::drawEngineFrontBase() {

	glColor3f(41 / 255.f, 36 / 255.f, 33 / 255.f);

	glBegin(GL_TRIANGLES);
	glVertex3f(TRACK_RADIUS, TRAIN_BOTTOM_BASE_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);
	glVertex3f(TRACK_RADIUS, TRAIN_BOTTOM_BASE_HEIGHT, TRAIN_BODY_LENGTH / 2.f);
	glVertex3f(TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);
	glEnd();

	glBegin(GL_TRIANGLES);
	glVertex3f(-TRACK_RADIUS, TRAIN_BOTTOM_BASE_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);
	glVertex3f(-TRACK_RADIUS, TRAIN_BOTTOM_BASE_HEIGHT, TRAIN_BODY_LENGTH / 2.f);
	glVertex3f(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(TRACK_RADIUS, TRAIN_BOTTOM_BASE_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);
	glVertex3f(-TRACK_RADIUS, TRAIN_BOTTOM_BASE_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);
	glVertex3f(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);
	glVertex3f(TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);
	glEnd();

	glBegin(GL_QUADS);
	glVertex3f(TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);
	glVertex3f(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);
	glVertex3f(-TRACK_RADIUS, TRAIN_BOTTOM_BASE_HEIGHT, TRAIN_BODY_LENGTH / 2.f);
	glVertex3f(TRACK_RADIUS, TRAIN_BOTTOM_BASE_HEIGHT, TRAIN_BODY_LENGTH / 2.f);
	glEnd();

}

void Train::drawEngineDriverRoom() {
	Pnt3f
		p0( TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, -TRAIN_BODY_LENGTH / 2.f),
		p1(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, -TRAIN_BODY_LENGTH / 2.f),
		p2(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, -TRAIN_BODY_LENGTH / 2.f * 0.5f),
		p3( TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, -TRAIN_BODY_LENGTH / 2.f * 0.5f);
	Pnt3f
		p4(TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_DRIVER_ROOM_HEIGHT, -TRAIN_BODY_LENGTH / 2.f),
		p5(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_DRIVER_ROOM_HEIGHT, -TRAIN_BODY_LENGTH / 2.f),
		p6(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_DRIVER_ROOM_HEIGHT, -TRAIN_BODY_LENGTH / 2.f * 0.5f),
		p7(TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_DRIVER_ROOM_HEIGHT, -TRAIN_BODY_LENGTH / 2.f * 0.5f);

	glColor3f(41 / 255.f, 36 / 255.f, 33 / 255.f);
	draw_quad_block(p0, p1, p2, p3, p4, p5, p6, p7);

}

void Train::drawEngineCylinder() {
	Pnt3f
		front_center(0.f, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_ENGINE_CYLINDER_RADIUS, TRAIN_BODY_LENGTH / 2.f * 0.8f),
		back_center(0.f, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_ENGINE_CYLINDER_RADIUS, -TRAIN_BODY_LENGTH / 2.f * 0.5f);

	glColor3f(41 / 255.f, 36 / 255.f, 33 / 255.f);
	draw_cylinder(front_center, back_center, TRAIN_ENGINE_CYLINDER_RADIUS, TRAIN_ENGINE_CYLINDER_RADIUS);

	Pnt3f
		out_center(0.f, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_ENGINE_CYLINDER_RADIUS, TRAIN_BODY_LENGTH / 2.f);
		
	draw_cylinder(front_center, out_center, TRAIN_ENGINE_CYLINDER_RADIUS, TRAIN_ENGINE_CYLINDER_RADIUS * 0.7f);
	
	glPushMatrix();
		glTranslatef(0.f, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_ENGINE_CYLINDER_RADIUS, TRAIN_BODY_LENGTH / 2.f);
		gluDisk(gluNewQuadric(), 0.f, TRAIN_ENGINE_CYLINDER_RADIUS * 0.7f, 30, 3);
	glPopMatrix();
}

void Train::drawEngineChimney() {
	Pnt3f
		bottom_center(0.f, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_ENGINE_CYLINDER_RADIUS, TRAIN_BODY_LENGTH / 2.f * 0.7f),
		top_center(0.f, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_ENGINE_CYLINDER_RADIUS + TRAIN_ENGINE_CHIMNEY_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.7f);

	glColor3f(0.f, 0.f, 0.f);
	draw_cylinder(bottom_center, top_center, TRAIN_ENGINE_CHIMNEY_RADIUS, TRAIN_ENGINE_CHIMNEY_RADIUS);

}

void Train::drawCarBlock() {
	Pnt3f
		p0(TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, -TRAIN_BODY_LENGTH / 2.f),
		p1(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, -TRAIN_BODY_LENGTH / 2.f),
		p2(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f),
		p3(TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f),
		p4(TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_CAR_BLOCK_HEIGHT, -TRAIN_BODY_LENGTH / 2.f),
		p5(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_CAR_BLOCK_HEIGHT, -TRAIN_BODY_LENGTH / 2.f),
		p6(-TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_CAR_BLOCK_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f),
		p7(TRACK_RADIUS, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_CAR_BLOCK_HEIGHT, TRAIN_BODY_LENGTH / 2.f * 0.8f);

	glColor3f(0.2f, 0.2f, 0.2f);
	draw_quad_block(p0, p1, p2, p3, p4, p5, p6, p7);

}

void Train::drawPeople(bool armOn) {

	for (int x = -1; x <= 1; x++) {
		for (int z = -TRAIN_PEOPLE_NUM/2; z <= TRAIN_PEOPLE_NUM/2; z++) {
			if (z == 0) continue;
			glPushMatrix();
				glColor3f(0.f, 0.f, 1.f);
				glTranslatef((float)x * TRAIN_PEOPLE_SPACING, TRAIN_BOTTOM_TOP_HEIGHT + TRAIN_CAR_BLOCK_HEIGHT, (float)z * TRAIN_PEOPLE_SPACING);
				
				//draw body
				Pnt3f body_bottom(0.f, 0.f, 0.f), body_top(0.f, TRAIN_PEOPLE_HEIGHT, 0.f);

				glColor3f(1.0f, 1.0f, 0.f);
				draw_cylinder(body_bottom, body_top, TRAIN_PEOPLE_RADIUS * 0.8f, TRAIN_PEOPLE_RADIUS * 0.8f);

				//draw head
				glPushMatrix();
					glTranslatef(0.f, TRAIN_PEOPLE_HEIGHT + TRAIN_PEOPLE_RADIUS, 0.f);
					gluSphere(gluNewQuadric(), TRAIN_PEOPLE_RADIUS, 30, 3);
				glPopMatrix();

				//draw arms
				if (armOn) {
					glColor3f(0.f, 0.f, 1.f);
					glPushMatrix(); {
						glTranslatef(TRAIN_PEOPLE_RADIUS, TRAIN_PEOPLE_HEIGHT*0.8f, 0.f);
						Pnt3f
							arm_left(0.f, 0.f, 0.f), arm_right(TRAIN_PEOPLE_ARM_LENGTH, 0.f, 0.f);
						draw_cylinder(arm_left, arm_right, TRAIN_PEOPLE_RADIUS * 0.5f, TRAIN_PEOPLE_RADIUS * 0.5f);
					}
					glPopMatrix();

					glPushMatrix();
					{
						glTranslatef(-TRAIN_PEOPLE_RADIUS, TRAIN_PEOPLE_HEIGHT*0.8f, 0.f);
						Pnt3f
							arm_left(0.f, 0.f, 0.f), arm_right(-TRAIN_PEOPLE_ARM_LENGTH, 0.f, 0.f);
						draw_cylinder(arm_left, arm_right, TRAIN_PEOPLE_RADIUS * 0.5f, TRAIN_PEOPLE_RADIUS * 0.5f);
					}
					glPopMatrix();
				}
			glPopMatrix();
		}
	}
}

//Reference : Assignment 4 in CS559, Spring 2015

//The duck and balloon are both from my own 559 project (fall 2012)
// The duck is drawn using a GL Display List
static GLUquadric * q = gluNewQuadric();

void Train::drawDuck() {

	//set up material parameters
	//numbers from http://devernay.free.fr/cours/opengl/materials.html
	//
	//yellow plastic
	GLfloat material_ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	GLfloat material_diffuse[] = { .5f, .5f, 0.0f, 1.0f };
	GLfloat material_specular[] = { .6f, .6f, .5f, 1.0f };
	GLfloat material_shininess[] = { .25f * 128.0f };

	//yellow rubber
	//GLfloat material_ambient[] = {.05, .05, 0, 1};
	//GLfloat material_diffuse[] = { .5, .5, .4, 1 };
	//GLfloat material_specular[] = { .7, .7, .04, 1 };
	//GLfloat material_shininess[] = { .078125 * 128 };
	glMaterialfv(GL_FRONT, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, material_shininess);

	//set the color of the body here, but not the eyes or beak
	glColor3d(1, 1, 0);

		glPushMatrix();

		glScalef(3.f, 5.f, 5.f);

		if (q) {
			// draw the body

			glPushMatrix();
			glScaled(1, .7, 1.2);

			gluSphere(q, 1, 100, 100);
			glPopMatrix();

			// draw the head
			glPushMatrix();
			glScaled(.4, .4, .4);
			glTranslated(0, 1.8, 2.2);
			gluSphere(q, 1, 100, 100);
			glPopMatrix();

			// draw the wings
			glPushMatrix();
			glScaled(.3, .4, .75);
			glTranslated(2.9, 0, 0);
			gluSphere(q, 1, 100, 100);
			glPopMatrix();

			glPushMatrix();
			glScaled(.3, .4, .75);
			glTranslated(-2.9, 0, 0);
			gluSphere(q, 1, 100, 100);
			glPopMatrix();

			//draw a tail
			glPushMatrix();
			glScaled(.5, .3, .5);
			glTranslated(0, .8, -1.8);
			glRotated(-145, 1, 0, 0);
			gluCylinder(q, 1, 0, 1, 100, 100);
			glPopMatrix();

			// draw the eyes
			glPushMatrix();
			glScaled(.1, .1, .1);
			glColor3d(1, 1, 1);
			glTranslated(2, 10, 11.3);
			gluSphere(q, 1, 100, 100);
			glPopMatrix();

			glPushMatrix();
			glScaled(.1, .1, .1);
			glColor3d(1, 1, 1);
			glTranslated(-2, 10, 11.3);
			gluSphere(q, 1, 100, 100);
			glPopMatrix();

			// blue pupils
			glPushMatrix();
			glScaled(.05, .05, .05);
			glColor3d(0, 0, 1);
			glTranslated(4, 20, 24);
			gluSphere(q, 1, 100, 100);
			glPopMatrix();

			glPushMatrix();
			glScaled(.05, .05, .05);
			glColor3d(0, 0, 1);
			glTranslated(-4, 20, 24);
			gluSphere(q, 1, 100, 100);
			glPopMatrix();

			// draw the beak
			glPushMatrix();
			glScaled(.25, .1, .5);
			glColor3d(1, .5, 0);
			glTranslated(0, 7.5, 2.35);
			gluCylinder(q, 1, 0, 1, 100, 100);
			glPopMatrix();

			glPopMatrix();
		} //end if q

}