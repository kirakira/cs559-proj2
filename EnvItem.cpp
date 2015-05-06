#include "EnvItem.h"
#include "Utilities/3DUtils.h"
#include "Utilities/Pnt3f.h"

#include "FL/gl.h"

void Tree::draw(bool doingShadow) {

	float cx = (low_x + high_x) / 2.0f, cy = (low_z + high_z) / 2.0f;

	glPushMatrix();
		glTranslatef(low_x, 0.f, low_z);

		glColor3f(115 / 255.f, 74 / 255.f, 18 / 255.f);

		Pnt3f
			tree_stick_bottom(0.f, 0.f, 0.f), tree_stick_top(0.f, 10.f, 0.f);

		draw_cylinder(tree_stick_bottom, tree_stick_top, 2.f, 2.f);
		
		glColor3f(34 / 255.f, 139 / 255.f, 34 / 255.f);

		glBegin(GL_TRIANGLES);
		{
			glVertex3f(5.f, 10.f, 5.f);
			glVertex3f(5.f, 10.f, -5.f);
			glVertex3f(0.f, 20.f, 0.f);
		
			glVertex3f(5.f, 10.f, -5.f);
			glVertex3f(-5.f, 10.f, -5.f);
			glVertex3f(0.f, 20.f, 0.f);

			glVertex3f(-5.f, 10.f, -5.f);
			glVertex3f(-5.f, 10.f, 5.f);
			glVertex3f(0.f, 20.f, 0.f);

			glVertex3f(-5.f, 10.f, 5.f);
			glVertex3f(5.f, 10.f, 5.f);
			glVertex3f(0.f, 20.f, 0.f);
		}
		glEnd();

		glBegin(GL_QUADS);
		{
			glVertex3f(5.f, 10.f, 5.f);
			glVertex3f(5.f, 10.f, -5.f);
			glVertex3f(-5.f, 10.f, -5.f);
			glVertex3f(-5.f, 10.f, 5.f);

		}
		glEnd();
	glPopMatrix();
}

void Pool::draw(bool doingShadow) {
	
	glColor3f(0.f, 0.f, 1.0f);

	glBegin(GL_QUADS);
		glVertex3f(low_x, 0.f, low_z);
		glVertex3f(low_x, 0.f, high_z);
		glVertex3f(high_x, 0.f, high_z);
		glVertex3f(high_x, 0.f, low_z);
	glEnd();
}

void Paramid::draw(bool doingShadow) {
	int height = rand() % 20;
	glColor3f(0.8f, 1.0f, 0.1f);
	glBegin(GL_TRIANGLES);
		glVertex3f(low_x, 0.f, low_z);
		glVertex3f(low_x, 0.f, high_z);
		glVertex3f((low_x + high_x) / 2.0f, (float)height, (low_z + high_z) / 2.0f);

		glVertex3f(low_x, 0.f, high_z);
		glVertex3f(high_x, 0.f, high_z);
		glVertex3f((low_x + high_x) / 2.0f, (float)height, (low_z + high_z) / 2.0f);

		glVertex3f(high_x, 0.f, high_z);
		glVertex3f(high_x, 0.f, low_z);
		glVertex3f((low_x + high_x) / 2.0f, (float)height, (low_z + high_z) / 2.0f);

		glVertex3f(high_x, 0.f, low_z);
		glVertex3f(low_x, 0.f, low_z);
		glVertex3f((low_x + high_x) / 2.0f, (float)height, (low_z + high_z) / 2.0f);

	glEnd();
}
