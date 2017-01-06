#ifndef ANIMATION_H
#define ANIMATION_H

#include <vector>
#include <GL/glut.h>
#include <stdlib.h>


#define M_PI 3.14159265358979323846
#define ANIMATIONTIME 1
#define ANIMATIONRADIUS 1

using std::vector;

class Animation
{
	char* id;
	char* type;
	float total_time;

public:
	unsigned long time_actual;
	bool ended;
	bool active;

	float distance;
	float velx;
	float vely;
	float velz;

	Animation(char* id, char* type, float span);
	
	char* getId(){return this->id;}
	char* getType(){return this->type;}
	float getSpan(){return this->total_time;}
	void setVelX(float vel){this->velx = vel;}
	void setVelY(float vel){this->vely = vel;}
	void setVelZ(float vel){this->velz = vel;}
	virtual vector<float> updateAnimation(vector<float> v, unsigned long time, unsigned long delta) = 0;
};

class AnimationLinear : public Animation
{
	float trans_x, trans_y, trans_z, dir_angle;
	float total_distance;
	bool firstPass;
	float ox;
	float oz;
	
public:
	AnimationLinear(char* id, float timeSpan) : Animation(id, "linear", timeSpan){firstPass = true;}
	vector<float> updateAnimation(vector<float> v, unsigned long time, unsigned long delta);
};

class AnimationCircular : public Animation
{
	float initial_angle;
	float final_angle;
	float* center;
	float omega;
	float radius;
	float time_span;

public:
	AnimationCircular(char* id, float time_span) : Animation(id, "circular", time_span){
		this->time_span = time_span;
	}
	vector<float> updateAnimation(vector<float> v, unsigned long time, unsigned long delta);
};
#endif