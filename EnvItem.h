#pragma once

#define NUM_ENV_ITEM 1

enum EnvType {
	tree = 0,
	paramid = 1,
	pool = 2
};

class EnvItem {
public:
	EnvItem(float x1, float z1, float x2, float z2) {
		low_x = x1; low_z = z1; high_x = x2; high_z = z2;
	}
	EnvItem() {
	}
	~EnvItem() {
	}

	//Each item is bounded in a rectangle, we have to guarantee the rectangle doesn't collide with the control points

	float low_x, low_z, high_x, high_z;
	
	virtual void draw(bool doingShadow) = 0;

};

class Tree : public EnvItem {
public:
	Tree(float x1, float y1, float x2, float y2) : EnvItem(x1, y1, x2, y2) {
	}
	Tree() {
	}
	~Tree() {
	}

	void draw(bool doingShadow);
};

class Pool : public EnvItem {
public:
	Pool(float x1, float z1, float x2, float z2) : EnvItem(x1, z1, x2, z2) {
	}
	Pool() {
	}
	~Pool() {
	}

	void draw(bool doingShadow);

};

class Paramid : public EnvItem {
public:
	Paramid(float x1, float z1, float x2, float z2) : EnvItem(x1, z1, x2, z2) {
	}
	Paramid() {
	}
	~Paramid() {
	}

	void draw(bool doingShadow);
};