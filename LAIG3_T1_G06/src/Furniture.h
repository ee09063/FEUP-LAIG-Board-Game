#ifndef _FURNITURE_H_
#define _FURNITURE_H_

#include "SceneGraph.h"

class Leg
{
	Primitive* sph;
	Primitive* cyl;
	vector<Appearance*>app;
public:
	Leg(vector<Appearance*> app);
	void draw(int set);
};

class Table
{
	Leg* leg;
	Primitive* unit;
	float length;
	float width;
	vector<Appearance*> table_top;
public:
	Table(float lenght, float width, vector<Appearance*>app);
	void draw(int set);
};

class Television
{
	Primitive* screen;
	vector<Appearance*> tv;
	int set;
public:
	Television(vector<Appearance*> app, int set);
	void draw();
};

class Chair
{
	Leg* leg;
	Primitive* unit;
	Primitive* cyl;
	vector<Appearance*>fabric;;
public:
	Chair(vector<Appearance*> app);
	void draw(int set);
};

class Lamp
{
	Primitive* base;
	Primitive* pin;
	Primitive* unit;
	Primitive* ring;
	Appearance* metal;
	Appearance* black;
	
public:
	Lamp(vector<Appearance*>app);
	void draw(int set);
};

class Tea
{
	Table* table;
	Ring* cup;
	GLUquadricObj* bot;
	vector<Appearance*>apps;
public:
	Tea(vector<Appearance*>app);
	void draw(int set);
};

class Cabinet
{
	Primitive* unit;
	Primitive* face;
	Appearance* front;
	Appearance* rest;
public:
	Cabinet(vector<Appearance*>app);
	void draw(int set);
};

#endif