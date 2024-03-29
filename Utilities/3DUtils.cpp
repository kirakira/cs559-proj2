/************************************************************************
     File:        3DUtils.cpp

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu
     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     some useful routines for writing 3D interactive programs
                  written for CS638 - Michael Gleicher, November 1999
                  re-written and expanded, October 2005
   
     Platform:    Visio Studio.Net 2003

     CVS Info: Date checked in: $Date: 2010/09/16 19:23:30 $
               Revision number: $Revision: 1.6 $
*************************************************************************/
#define _USE_MATH_DEFINES
#include <math.h>

#include <windows.h>
#include <GL/gl.h>
#include <FL/Fl.h>
#include <GL/glu.h>

#include "3DUtils.H"
#include "Pnt3f.H"

#include <vector>
#include <tuple>
#include <set>
using namespace std;


//*************************************************************************
//
// A utility function - draw a little cube at the origin (use a 
// transform to put it in the appropriate place)
// note: we pass the size of the cube (rather than using a scale
// transform) since we want our normals to stay unit length if possible
//
// Note: 
//  1. This isn't necesarily the fastest way since I recompute each
//       vertex. It probably would be better to use a scale transform and then
//		 transform the normals.
//  2. Notice that I don't keep all my polygons with the same orientations!
//===============================================================================
void drawCube(float x, float y, float z, float l)
//===============================================================================
{
	glPushMatrix();
		glTranslated(x,y,z);
		// glScalef(l,l,l);			// the scale transform messes up normals
		float nl = -0.5f * l;
		float pl =  0.5f * l;

		glBegin(GL_QUADS);
			glNormal3d( 0,0,1);
			glVertex3d(pl,pl,pl);
			glVertex3d(nl,pl,pl);
			glVertex3d(nl,nl,pl);
			glVertex3d(pl,nl,pl);

			glNormal3d( 0, 0, -1);
			glVertex3d(pl,pl, nl);
			glVertex3d(pl,nl, nl);
			glVertex3d(nl,nl, nl);
			glVertex3d(nl,pl, nl);

			glNormal3d( 0, 1, 0);
			glVertex3d(pl,pl,pl);
			glVertex3d(pl,pl,nl);
			glVertex3d(nl,pl,nl);
			glVertex3d(nl,pl,pl);

			glNormal3d( 0,-1,0);
			glVertex3d(pl,nl,pl);
			glVertex3d(nl,nl,pl);
			glVertex3d(nl,nl,nl);
			glVertex3d(pl,nl,nl);

			glNormal3d( 1,0,0);
			glVertex3d(pl,pl,pl);
			glVertex3d(pl,nl,pl);
			glVertex3d(pl,nl,nl);
			glVertex3d(pl,pl,nl);

			glNormal3d(-1,0,0);
			glVertex3d(nl,pl,pl);
			glVertex3d(nl,pl,nl);
			glVertex3d(nl,nl,nl);
			glVertex3d(nl,nl,pl);
		glEnd();
	glPopMatrix();
}

//*************************************************************************
//
// the two colors for the floor for the check board
//
//*************************************************************************
float floorColor1[3] = { .7f, .7f, .7f }; // Light color
float floorColor2[3] = { .3f, .3f, .3f }; // Dark color

//*************************************************************************
//
// Draw the check board floor without texturing it
//===============================================================================
void drawFloor(float size, int nSquares)
//===============================================================================
{
	// parameters:
	float maxX = size/2, maxY = size/2;
	float minX = -size/2, minY = -size/2;

	int x,y,v[3],i;
	float xp,yp,xd,yd;
	v[2] = 0;
	xd = (maxX - minX) / ((float) nSquares);
	yd = (maxY - minY) / ((float) nSquares);
	glBegin(GL_QUADS);
	for(x=0,xp=minX; x<nSquares; x++,xp+=xd) {
		for(y=0,yp=minY,i=x; y<nSquares; y++,i++,yp+=yd) {
			glColor3fv(i%2==1 ? floorColor1:floorColor2);
			glNormal3f(0, 1, 0); 
			glVertex3d(xp,      0, yp);
			glVertex3d(xp,      0, yp + yd);
			glVertex3d(xp + xd, 0, yp + yd);
			glVertex3d(xp + xd, 0, yp);

		} // end of for j
	}// end of for i
	glEnd();
}

//*************************************************************************
//
// Shadows
// * Before drawing the floor, setup the stencil buffer and enable depth testing
// * A trick: because we only want to draw the shadows
//            on the ground plane, when we draw the ground plane
//            we'll set the stencil buffer to 1. then we'll only
//            draw shadows where the stencil buffer is one, so we 
//            don't have shadows floating in space
//===============================================================================
void setupFloor(void)
//===============================================================================
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS,0x1,0x1);
	glStencilOp(GL_REPLACE,GL_REPLACE,GL_REPLACE);
	glStencilMask(0x1);		// only deal with the 1st bit
}

//*************************************************************************
// now draw the objects normally - be sure to have them write the stencil
// to show the floor isn't there anymore
void setupObjects(void)
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS,0x0,0x0);
	glStencilOp(GL_REPLACE,GL_REPLACE,GL_REPLACE);
	glStencilMask(0x1);		// only deal with the 1st bit
}


//*************************************************************************
//
// These are cheap "hack" shadows - basically just squish the objects onto the floor.
//
// * We'll use a projection matrix to do the squishing (basically set the 
//   Y component to zero)
// * We'll also need to turn off lighting (since we want the objects to be black)
// * To make things look nice, we'll draw the shadows as transparent (so
//   we can see the groundplane)
// * To avoid Z-Fighting (since the shadows are on the ground), we'll turn
//   off the Z-Buffer - although, this means we'll see the shadows through
//   the floor
// * Finally, we'll use the stencil buffer to only draw where the ground
//   has already been drawn - this way we won't have shadows floating in
//   space!
//===============================================================================
void setupShadows(void)
//===============================================================================
{
	glDisable(GL_LIGHTING);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_EQUAL,0x1,0x1);
	glStencilOp(GL_KEEP,GL_ZERO,GL_ZERO);
	glStencilMask(0x1);		// only deal with the 1st bit

	glPushMatrix();
	// a matrix that squishes things onto the floor
	float sm[16] = {1,0,0,0, 0,0,0,0, 0,0,1,0, 0,0,0,1};
	glMultMatrixf(sm);
	// draw in transparent black (to dim the floor)
	glColor4f(0,0,0,.5);
}

//*************************************************************************
//
// * Warning - this puts things back to a "normal" state, not
//   necessarily where they were before setupShadows
//===============================================================================
void unsetupShadows(void)
//===============================================================================
{
  glPopMatrix();
  glEnable(GL_DEPTH_TEST);
  glDisable(GL_STENCIL_TEST);
  glDisable(GL_BLEND);
}


//*************************************************************************
//
// * Convert from mouse coordinates to world coordinates
//   in a sense, this mimics the old fashioned gl's mapw
//   the only advantage is that we don't have to pick the near clipping
//   plane, so we can be a little more well-balanced
//   this code mimics page 147 of the OpenGL book
//===============================================================================
int getMouseLine(double& x1, double& y1, double& z1,
								 double& x2, double& y2, double& z2)
//===============================================================================
{
  int x = Fl::event_x();
  int iy = Fl::event_y();

  double mat1[16],mat2[16];		// we have to deal with the projection matrices
  int viewport[4];

  glGetIntegerv(GL_VIEWPORT, viewport);
  glGetDoublev(GL_MODELVIEW_MATRIX,mat1);
  glGetDoublev(GL_PROJECTION_MATRIX,mat2);

  int y = viewport[3] - iy; // originally had an extra -1?

  int i1 = gluUnProject((double) x, (double) y, .25, mat1, mat2, viewport, &x1, &y1, &z1);
  int i2 = gluUnProject((double) x, (double) y, .75, mat1, mat2, viewport, &x2, &y2, &z2);

  return i1 && i2;
}


//*************************************************************************
//
// More of an explanation in the header file.
// The beginings of a mousepole handler, given a pair of points on the
// mouse ray, and the point in question, find a new point. By default,
// it assumes the plane parallel to the ground, unless the elevator
// button is pushed, in which case it is the YZ plane
//===============================================================================
inline double ABS(double x) { return (x<0) ? -x : x; };
//===============================================================================

//*************************************************************************
//
// * When you have a mouse line, you want to pick a point where you think the user 
//   wants to drag to
// * Idea: given a plane parallel to the floor, pick a point on that
//         plane (where the line intersects it)
// * Problem: what to do when the plane is parallel to the line?
// * Problem: how to make things go vertically?
// * Answer:
//   1. Have an "elevator mode" where we use a plane perpindicular to the floor
//   2. r1,r2 are two points on the line
//      a. l is the initial position of the point - we need this to know where
//         the plane is. 
//      b. r is the resulting position. it will share 1 of its coordinates
//         with l, but will be on the line
//===============================================================================
void mousePoleGo(double r1x, double r1y, double r1z,
									double r2x, double r2y, double r2z,
									double lx, double ly, double lz, 
									double &rx, double &ry, double &rz,
									bool elevator)
//===============================================================================
{
  rx = lx; ry=ly; rz=lz;
  if (elevator || (ABS(r1y - r2y) < .01)) {
	if (ABS(r1z-r2z) > ABS(r1x-r2x)) {
	  double zd = r1z-r2z;
	  if (ABS(zd) > .01) {
		double zp = (lz - r1z) / zd;
		rx = r1x + (r1x - r2x) * zp;
		ry = r1y + (r1y - r2y) * zp;
	  } 
	} else {
	  double xd = r1x-r2x;
	  if (ABS(xd) > .01) {
		double xp = (lx-r1x) / xd;
		rz = r1z + (r1z - r2z) * xp;
		ry = r1y + (r1y - r2y) * xp;
	  } 
	}
  } else {
	double yd = r1y - r2y;
	// we have already made sure that the elevator is not singular
	double yp = (ly - r1y) / yd;
	rx = r1x + (r1x - r2x) * yp;
	rz = r1z + (r1z - r2z) * yp;
  }
}

//===============================================================================
static const float rtdf = static_cast<float>(180.0 / M_PI);
float radiansToDegrees(const float x)
{
	return rtdf * x;
}

//================================================================================
// save the lighting state on the stack so it can be restored
struct LightState {
	bool lighting;
	bool smooth;

	LightState(bool l, bool s) : lighting(l), smooth(s) {};
};
static vector<LightState> lightStateStack;

void setLighting(const LightOnOff lighting, const LightOnOff smoothi)
{
	int smooth;
	bool lights = glIsEnabled(GL_LIGHTING) > 0;
	glGetIntegerv(GL_SHADE_MODEL,&smooth);
	lightStateStack.push_back(LightState( lights, (smooth == GL_SMOOTH) ));

	if (lighting != keep) {
		if (lighting == on) glEnable(GL_LIGHTING);
		else glDisable(GL_LIGHTING);
	}
	if (smoothi != keep) {
		if (smoothi == on) glShadeModel(GL_SMOOTH);
		else glShadeModel(GL_FLAT);
	}

}
void restoreLighting()
{
	if (!lightStateStack.empty()) {
		if ((lightStateStack.end()-1)->lighting)
			glEnable(GL_LIGHTING);
		else
			glDisable(GL_LIGHTING);

		if ((lightStateStack.end()-1)->smooth)
			glShadeModel(GL_SMOOTH);
		else
			glShadeModel(GL_FLAT);

	}
}

//***********************************************************************************************

const float EPS = 0.00001f;

void draw_segment(const Pnt3f &p1, const Pnt3f &p2) {
	glBegin(GL_LINE_STRIP);
	glVertex3f(p1.x, p1.y, p1.z);
	glVertex3f(p2.x, p2.y, p2.z);
	glEnd();
}

void view_matrix_from_z_axis(const Pnt3f &vec, GLfloat* &m) {
	Pnt3f u = vec;
	u.normalize();

	Pnt3f up = Pnt3f(0, 1, 0);
	Pnt3f v = u * up;
	if (v.length() < EPS) {
		v = Pnt3f(1.0, 0.0, 0.0);
	}
	v.normalize();
	Pnt3f w = v * u;
	w.normalize();

	m = new GLfloat[16] {
		v.x, v.y, v.z, 0,
		w.x, w.y, w.z, 0,
		u.x, u.y, u.z, 0,
		0,     0,   0, 1
	};
}

void draw_line_stripe_4pt(const Pnt3f &p1, const Pnt3f &p2, const Pnt3f &p3, const Pnt3f &p4) {
	glBegin(GL_LINE_STRIP);
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
		glVertex3f(p3.x, p3.y, p3.z);
		glVertex3f(p4.x, p4.y, p4.z);
		glVertex3f(p1.x, p1.y, p1.z);
	glEnd();
}

void draw_quad_4pt(const Pnt3f &p1, const Pnt3f &p2, const Pnt3f &p3, const Pnt3f &p4) {
	glBegin(GL_QUADS);
		glVertex3f(p1.x, p1.y, p1.z);
		glVertex3f(p2.x, p2.y, p2.z);
		glVertex3f(p3.x, p3.y, p3.z);
		glVertex3f(p4.x, p4.y, p4.z);
	glEnd();
}

void draw_quad_block(
	const Pnt3f &p1, const Pnt3f &p2, const Pnt3f &p3, const Pnt3f &p4,
	const Pnt3f &q1, const Pnt3f &q2, const Pnt3f &q3, const Pnt3f &q4
	) {
	draw_quad_4pt(p1, p2, p3, p4);
	draw_quad_4pt(q1, q2, q3, q4);
	draw_quad_4pt(p1, p2, q1, q2);
	draw_quad_4pt(p2, p3, q2, q3);
	draw_quad_4pt(p3, p4, q3, q4);
	draw_quad_4pt(p4, p1, q4, q1);
}
static GLUquadricObj *quad_obj = NULL;

void view_transform(const Pnt3f &pt, const Pnt3f &dir) {
	Pnt3f vec(dir);
	glTranslated(pt.x, pt.y, pt.z);
	vec.normalize();
	GLfloat *m;
	view_matrix_from_z_axis(vec, m);
	glMultMatrixf(m);
}

void draw_cylinder(const Pnt3f &p1, const Pnt3f &p2, float r1, float r2) {
	Pnt3f dir = p2 - p1;

	GLdouble length = dir.length();
	if (quad_obj == NULL) quad_obj = gluNewQuadric();
	gluCylinder(quad_obj, r1, r2, length, 1, 0);
	gluQuadricDrawStyle(quad_obj, GLU_FILL);
	gluQuadricDrawStyle(quad_obj, GLU_SMOOTH);
	
	glPushMatrix();
		view_transform(p1, dir);
		gluCylinder(quad_obj, r1, r2, length, 20, 2);
	glPopMatrix();
}




bool rectangle_overlap(float x1, float z1, float x2, float z2, float x3, float z3, float x4, float z4) {
	if (x2 <= x3 || x1 >= x4) return false;
	if (z1 <= z3 || z1 >= z4) return false;
	return true;
}

void constructTetrahedron(int base1, int base2, int base3, int top, vector<tuple<int, int, int>> *result) {
	result->emplace_back(make_tuple(base1, base2, top));
	result->emplace_back(make_tuple(base2, base3, top));
	result->emplace_back(make_tuple(base1, top, base3));
	result->emplace_back(make_tuple(base1, base3, base2));
}

vector<tuple<int, int, int>> convexHull(const vector<Pnt3f> &points) {
	int n = (int)points.size();
	vector<tuple<int, int, int>> ans;
	set<int> processed;
	for (int i = 0; i < n && ans.empty(); ++i)
		for (int j = i + 1; j < n && ans.empty(); ++j)
			for (int k = j + 1; k < n && ans.empty(); ++k)
				for (int l = k + 1; l < n && ans.empty(); ++l)
					if (!colinear(points[i], points[j], points[k])
						&& !colinear(points[i], points[j], points[l])
						&& !colinear(points[i], points[k], points[l])
						&& !colinear(points[j], points[k], points[l])) {
						constructTetrahedron(i, j, k, l, &ans);
						processed.emplace(i);
						processed.emplace(j);
						processed.emplace(k);
						processed.emplace(l);
					}

	// degeneration case
	if (ans.empty())
		return ans;

	while ((int)processed.size() < n) {
		int curr;
		for (int i = 0; i < n; ++i)
			if (processed.count(i) == 0) {
				curr = i;
				break;
			}
		processed.insert(curr);

		vector<tuple<int, int, int>> new_ans;
		for (const tuple<int, int, int> &ti : ans) {
			auto side1 = points[get<1>(ti)] - points[get<0>(ti)],
				side2 = points[get<2>(ti)] - points[get<1>(ti)];
			auto norm = side1 * side2, v = points[curr] - points[get<0>(ti)];
			if (fgt(dot(norm, v), 0))
				constructTetrahedron(get<0>(ti), get<1>(ti), get<2>(ti), curr, &new_ans);
			else
				new_ans.emplace_back(ti);
		}
		ans = new_ans;
	}

	return ans;
}