//Change 03-25-15, change the name to Main.cpp for naming conflicts
#include "stdio.h"

#include "TrainWindow.H"

#pragma warning(push)
#pragma warning(disable:4312)
#pragma warning(disable:4311)
#pragma warning(disable:4204)
#include <Fl/Fl.h>

#include <GL/glew.h>
#include <FL/glut.h>
#include "ShaderTools.H"

#include "Mesh.h"

#pragma warning(pop)


int main(int argc, char** argv)
{
	Pnt3f a(0, 0, 0), b(1, 1, 1), c(2, 2, 2), d(3, 3, 3), e(4, 4, 4), f(5, 5, 5), g(6, 6, 6);
	Mesh m({ a, b, c, d, e, f, g }, { std::make_tuple(0, 1, 2),
		std::make_tuple(0, 5, 6), std::make_tuple(0, 3, 4), std::make_tuple(0, 2, 3) });
	m.modifiedButterfly();
	
	printf("CS559 Train - Shouhm, Wangmh\n");
	printf("Welcome to Proj2!");

	glutInit(&argc, argv);
	TrainWindow tw;
	tw.show();

	Fl::run();
}
