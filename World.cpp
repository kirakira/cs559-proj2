#include "World.H"

#include "Utilities/3DUtils.h"

#include <FL/fl_ask.h>

#include <cstdlib>

World::World() : trainU(0), recordDir(1.0f)
{
	resetPoints();
	resetItems();
}

// provide a default set of points
void World::resetPoints()
{
	points.clear();
	points.push_back(ControlPoint(Pnt3f(50,5,0)));
	points.push_back(ControlPoint(Pnt3f(0,5,50)));
	points.push_back(ControlPoint(Pnt3f(-50,5,0)));
	points.push_back(ControlPoint(Pnt3f(0,5,-50)));

	// we had better put the train back at the start of the track...
	trainU = 0.0;
}

void World::resetItems() {
	int tot = items.size();
	for (int i = 0; i < tot; i++)
		delete items[i];
	items.clear();
}

// handy utility to break a string into a list of words
// this originally came from an old 559 project a long time ago,
// and now appears in just about every program I write
void breakString(char* str, std::vector<const char*>& words) {
	// start with no words
	words.clear();

	// scan through the string, starting at the beginning
	char* p = str;

	// stop when we hit the end of the string
	while(*p) {
		// skip over leading whitespace - stop at the first character or end of string
		while (*p && *p<=' ') p++;

		// now we're pointing at the first thing after the spaces
		// make sure its not a comment, and that we're not at the end of the string
		// (that's actually the same thing)
		if (! (*p) || *p == '#')
		break;

		// so we're pointing at a word! add it to the word list
		words.push_back(p);

		// now find the end of the word
		while(*p > ' ') p++;	// stop at space or end of string

		// if its ethe end of the string, we're done
		if (! *p) break;

		// otherwise, turn this space into and end of string (to end the word)
		// and keep going
		*p = 0;
		p++;
	}
}

// the file format is simple
// first line: an integer with the number of control points
// other lines: one line per control point
// either 3 (X,Y,Z) numbers on the line, or 6 numbers (X,Y,Z, orientation)
void World::readPoints(const char* filename)
{
	FILE* fp = fopen(filename,"r");
	if (!fp) {
		fl_alert("Can't Open File!\n");
	} else {
		char buf[512];

		// first line = number of points
		fgets(buf,512,fp);
		size_t npts = (size_t) atoi(buf);

		if( (npts<4) || (npts>65535)) {
			fl_alert("Illegal Number of Points Specified in File");
		} else {
			points.clear();
			// get lines until EOF or we have enough points
			while( (points.size() < npts) && fgets(buf,512,fp) ) {
				Pnt3f pos,orient;
				vector<const char*> words;
				breakString(buf,words);
				if (words.size() >= 3) {
					pos.x = (float) strtod(words[0],0);
					pos.y = (float) strtod(words[1],0);
					pos.z = (float) strtod(words[2],0);
				} else {
					pos.x=0;
					pos.y=0;
					pos.z=0;
				}
				if (words.size() >= 6) {
					orient.x = (float) strtod(words[3],0);
					orient.y = (float) strtod(words[4],0);
					orient.z = (float) strtod(words[5],0);
				} else {
					orient.x = 0;
					orient.y = 1;
					orient.z = 0;
				}
				orient.normalize();
				points.push_back(ControlPoint(pos,orient));
			}
		}
		fclose(fp);
	}
	trainU = 0;
}

// write the control points to our simple format
void World::writePoints(const char* filename)
{
	FILE* fp = fopen(filename,"w");
	if (!fp) {
		fl_alert("Can't open file for writing");
	} else {
		fprintf(fp,"%d\n",points.size());
		for(size_t i=0; i<points.size(); ++i)
			fprintf(fp,"%g %g %g %g %g %g\n",
				points[i].pos.x, points[i].pos.y, points[i].pos.z, 
				points[i].orient.x, points[i].orient.y, points[i].orient.z);
		fclose(fp);
	}
}

void World::genItemSpace(float &low_x, float &low_z, float &high_x, float &high_z, bool &succ) {
	int times = 100;
	do {
		int x1 = rand() % 200; x1 -= 100;
		int z1 = rand() % 200; z1 -= 100;
		int x2 = rand() % 200; x2 -= 100;
		int z2 = rand() % 200; z2 -= 100;
		if (x1 > x2) { int t = x1; x1 = x2; x2 = t; }
		if (z1 > z2) { int t = z1; z1 = z2; z2 = t; }

		//discard too small or too large
		if (x2 - x1 < 10.f || x2 - x1 > 40.f) continue; 
		if (z2 - z1 < 10.f || z2 - z1 > 40.f) continue;

		//prevent from colliding with control points
		bool collide = false;
		for (int i = 0; i < (int)points.size(); i++) {
			float x = points[i].pos.x, z = points[i].pos.z;
			if ((float)x1 <= x && x <= (float)x2) { collide = true; break; }
			if ((float)z1 <= z && z <= (float)z2) { collide = true; break; }
		}

		//prevent from colliding with current item space
		for (int i = 0; i < (int)items.size(); i++) {
			if (rectangle_overlap((float)x1, (float)z1, (float)x2, (float)z2, items[i]->low_x, items[i]->low_z, items[i]->high_x, items[i]->high_z)) {
				collide = true;
				break;
			}
		}

		//if not collide, return the selected region
		if (!collide) {
			low_x = (float)x1; low_z = (float)z1;
			high_x = (float)x2; high_z = (float)z2;
			succ = true;
			return;
		}
		times--;
	} while (times > 0);
	succ = false;
}

void World::addSceneryItem() {

	//randomly select one scenery item from our list
	int id = rand() % NUM_ENV_ITEM;
	EnvItem* envItem;

	float low_x, low_z, high_x, high_z;
	bool succ;

	//generate the item space
	genItemSpace(low_x, low_z, high_x, high_z, succ);

	if (!succ) {
		printf("Too crowded! No space to put environment items!");
		return;
	}

	if (id == EnvType::tree) {
		envItem = new Tree(low_x, low_z, high_x, high_z);
		items.push_back(envItem);
	}
	else if (id == EnvType::paramid) {
		envItem = new Paramid(low_x, low_z, high_x, high_z);
		items.push_back(envItem);
	}
	else if (id == EnvType::pool) {
		envItem = new Pool(low_x, low_z, high_x, high_z);
		items.push_back(envItem);
	}
}

void World::removeSceneryItem() {
	if (items.size() > 0) {
		int last = items.size() - 1;
		delete items[last];
		items.pop_back();
	}
}

void World::drawItems(bool doingShadow) {
	for (int i = 0; i < (int)items.size(); i++) {
		items[i]->draw(doingShadow);
	}
}