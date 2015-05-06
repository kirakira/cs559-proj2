//Change 03-25-15, change the name to Main.cpp for naming conflicts
#include "stdio.h"

#include "TrainWindow.H"

#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#pragma warning(disable:4204)
#include <Fl/Fl.h>

#include <GL/glew.h>
#include "ShaderTools.H"

#pragma warning(pop)


int main(int, char**)
{
	printf("CS559 Train - Shouhm, Wangmh\n");
	printf("Welcome to Proj2!");

	TrainWindow tw;
	tw.show();

	Fl::run();
}
