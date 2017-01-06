#include "Animation.h"

#define CAMERA_X 24.5
#define CAMERA_Y 22
#define CAMERA_Z 20
#define RADIUS 3

/*CONVERSIONS*/
float DtR(float degree)
{
	return (degree * M_PI) / 180.0;
}

float RtD(float rad)
{
	return (180.0 * rad) / M_PI;
}

Animation::Animation(char* id, char* type, float total_time)
{
		this->id = id;
		this->type = type;
		this->total_time = total_time;
		this->time_actual = 0;
		this->ended = false;
		this->active = false;
}

vector<float> AnimationLinear::updateAnimation(vector<float> f, unsigned long time, unsigned long delta)
{
	float xi = f.at(0);
	float xf = f.at(1);
	float zi = f.at(2);
	float zf = f.at(3);

	if(this->firstPass){
		this->ox = xi;
		this->oz = zi;
		this->firstPass = false;
	}

	vector<float> v;
	this->velx = xf-ox / ANIMATIONTIME;
	this->velz = zf-oz / ANIMATIONTIME;
	float x = xi + velx * delta/1000.0;
	float z = zi + velz * delta/1000.0;
	float alpha = (time/1000.0 * M_PI) / ANIMATIONTIME;
	float y = sin(alpha) * ANIMATIONRADIUS;

	float continueAnimating;
	if(time/1000.0 >= ANIMATIONTIME){
		continueAnimating = -1;
		this->firstPass = true;
		y = 0.0;
	}
	else continueAnimating = 1;
	v.push_back(x);
	v.push_back(y);
	v.push_back(z);
	v.push_back(continueAnimating);
	return v;
}

vector<float> AnimationCircular::updateAnimation(vector<float> v, unsigned long time, unsigned long delta)
{	
	vector<float> f;
	
	float alpha = 180.0 * time/1000.0;
	float a = DtR(alpha);
	float x;
	float z;

	if(v.at(9) == 1){
		x = CAMERA_X + RADIUS * sin(a + M_PI);
		z = CAMERA_Z + RADIUS * cos(a + M_PI);
	}
	else{
		x = CAMERA_X + RADIUS * sin(a);
		z = CAMERA_Z + RADIUS * cos(a);
	}
	float y = CAMERA_Y;

	f.push_back(v.at(0));
	f.push_back(v.at(1));
	f.push_back(v.at(2));
	f.push_back(x);
	f.push_back(y);
	f.push_back(z);
	f.push_back(v.at(6));
	f.push_back(v.at(7));
	f.push_back(v.at(8));

	float continueAnimating;
	if(time/1000.0 >= ANIMATIONTIME){
		continueAnimating = -1;
	}
	else continueAnimating = 1;

	f.push_back(continueAnimating);
	
	return f;
}