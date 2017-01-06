#ifndef GRAPH_H
#define GRAPH_H

#include <vector>
#include <sstream>
#include "tinyxml.h"
#include "CGFobject.h"
#include "CGFappearance.h"
#include "CGFcamera.h"
#include "CGFlight.h"
#include "Animation.h"

using std::vector;

class Global
{
	GLenum drawingMode;
	GLenum shadingMode;
	GLenum cullFace;
	GLenum order;

	float* backG;
	float* ambient;
	GLfloat doublesided, local, enabled;
	public:
		Global(){}
		void setDrawingElements(char* drawingMode, char* shading, float* backG)
		{
			if(strcmp(drawingMode,"fill")==0) this->drawingMode = GL_FILL;
			else if(strcmp(drawingMode,"line")==0) this->drawingMode = GL_LINE;
			else if(strcmp(drawingMode,"point")==0) this->drawingMode = GL_POINT;

			if(strcmp(shading,"flat")==0) this->shadingMode = GL_FLAT;
			else if(strcmp(shading, "gouraud")==0) this->shadingMode = GL_SMOOTH;

			this->backG=backG;
		}
		void setCullingElements(char* cullingFace, char* order)
		{
			if(strcmp(cullingFace ,"none")==0) this->cullFace = 0;
			else if(strcmp(cullingFace,"back")==0) this->cullFace = GL_BACK;
			else if(strcmp(cullingFace, "front")==0) this->cullFace = GL_FRONT;
			else if(strcmp(cullingFace, "both")==0) this->cullFace = GL_FRONT_AND_BACK;

			if(strcmp(order, "ccw")==0) this->order = GL_CCW;
			else if(strcmp(order, "cw")==0) this->order = GL_CW;
		}
		void setLigtingElements(bool doublesided, bool local, bool enabled, float* ambient)
		{
			if(doublesided == true) this->doublesided = GL_TRUE;
			else if(doublesided == false) this->doublesided = GL_FALSE;
			if(local == true) this->local = GL_TRUE;
			else if(local = false) this->local = GL_FALSE;
			if(enabled == true) this->enabled = GL_TRUE;
			else if(enabled == false) this->enabled = GL_FALSE;
			this->ambient=ambient;
		}
		GLenum getDrawingMode(){return this->drawingMode;}
		GLenum getShadingMode(){return this->shadingMode;}
		GLenum getCullFace(){return this->cullFace;}
		GLenum getOrder(){return this->order;}
		GLfloat getSided() {return this->doublesided;}
		GLfloat getLocal() {return this->local;}
		GLfloat getEnabled() {return this->enabled;}
		float* getBackground(){return this->backG;}
		float* getAmbient() {return this->ambient;}
};

class Light
{
	char* id;
	char* type;
	bool enabled;
	bool marker;
	vector<float>location;
	float* ambient;
	float* diffuse;
	float* specular;
	float angle;
	float exponent;
	float* direction;
	float* target;
	public:
		Light(char* id, char* type, bool enabled, bool marker, float angle,
			  float exponent, vector<float> location, float* target,
			  float* ambient, float* diffuse, float* specular) 
		{
			this->id=id;
			this->type = type;
			this->enabled = enabled;
			this->marker = marker;
			this->location = location;
			this->ambient = ambient;
			this->diffuse = diffuse;
			this->specular = specular;
			this->angle = angle;
			this->exponent = exponent;
			this->target=target;
			if(strcmp(this->type, "spot")==0){
				direction = (float*)malloc(sizeof(float)*3);
				direction[0] = target[0]-location[0];
				direction[1] = target[1]-location[1];
				direction[2] = target[2]-location[2];
			}
		}
		vector<float> getLocation(){return this->location;}
		float* getAmbient(){return this->ambient;}
		float* getDiffuse(){return this->diffuse;}
		float* getSpecular(){return this->specular;}
		char* getId(){return this->id;}
		char* getType(){return this->type;}
		bool getEnabled(){return this->enabled;}
		bool getMarker(){return this->marker;}
		float getAngle(){return this->angle;}
		float getExponent(){return this->exponent;}
		float* getDirection(){return this->direction;}
		float* getTarget(){return this->target;}
};

class Camera : public CGFcamera
{
	char* id;
	char* type;
	public:
		Camera(char* type, char* id)
		{
			this->id = id;
			this->type = type;
		}
		char* getId() {return this->id;}
		char* getType() {return this->type;}
		virtual vector<float> getValues() {vector<float>v;return v;}
		virtual void setValues(vector<float>v){};
		virtual void setView(){};
		virtual void applyView(){};
		virtual void updateProjectionMatrix(int width, int height){};
};

class CamPerspective : public Camera
{
	float near;
	float far;
	float angle;
	float posx;
	float posy;
	float posz;
	float targx;
	float targy;
	float targz;
	vector<float>v;

	public:
		CamPerspective(char* id) : Camera("perspective", id){}
		vector<float> getValues(){return v;}
		void setValues(vector<float>v)
		{
			this->v = v;
			this->near = v.at(0); far = v.at(1); angle = v.at(2);
			posx = v.at(3); posy = v.at(4); posz = v.at(5);
			targx = v.at(6); targy = v.at(7); targz = v.at(8);
		}
		void setView();
		void updateProjectionMatrix(int width, int height)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			float aspect =  (float)width / (float)height;
			gluPerspective(angle, aspect, near, far);
		}
		void applyView()
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			gluLookAt(posx, posy, posz,
					  targx, targy, targz,
				      0.0, 1.0, 0.0);
		}
};

class CamOrtho: public Camera
{
	float near; float far; float left; float right; float top; float bottom; float direction;
	vector<float>v;

	public:
		CamOrtho(char* id) : Camera("ortho", id){}
		vector<float> getValues(){return v;}
		void setValues(vector<float>v)
		{
			this->v=v;
			near = v.at(0); far = v.at(1); left = v.at(2);
			right = v.at(3); top = v.at(4); bottom = v.at(5);
			direction = v.at(6);
		}
		void setView();
		void updateProjectionMatrix(int width, int height)
		{
			glMatrixMode(GL_PROJECTION);
			glLoadIdentity();
			float aspect =  (float)width / (float)height;

			if(width <= height)
				glOrtho(left, right, bottom/aspect, top/aspect, near, far);
			else
				glOrtho(left*aspect, right*aspect, bottom, top, near, far);
		}
		void applyView()
		{
			glMatrixMode(GL_MODELVIEW);
			glLoadIdentity();
			if(direction == 0){
				glRotatef(-90, 0 , 1, 0); //x
			}
			else if(direction == 1){
				glRotatef(90, 1, 0, 0); //y
			}
		}
};

class Texture
{
	char* id;
	char* file;
	float lengthS, lengthT;
	public:
		Texture(char* id, char* file, float lengthS, float lengthT)
		{
			this->id=id;
			this->file=file;
			this->lengthS=lengthS;
			this->lengthT=lengthT;
		}
		char* getId(){return this->id;}
		char* getFile(){return this->file;}
		float getTexS(){return this->lengthS;}
		float getTexT(){return this->lengthT;}
};

class Appearance
{
	char* id;
	float shininess;
	float* ambient;
	float* diffuse;
	float* specular;
	CGFappearance* app;
	Texture* texture;
	char* type;
	public:
		Appearance(char* id, Texture* texture, float shininess, float*ambient, float*diffuse, float*specular, char* type)
		{
			this->id=id;
			this->texture = texture;
			this->shininess=shininess;
			this->ambient=ambient;
			this->diffuse=diffuse;
			this->specular=specular;
			this->type = type;
			app = new CGFappearance(ambient, diffuse, specular, shininess);
			if(strcmp(this->texture->getId(), "")!=0)
				app->setTexture(texture->getFile());
		}
		char* getId(){return this->id;}
		float getShine(){return this->shininess;}
		float* getAmbient(){return this->ambient;}
		float* getDiffuse(){return this->diffuse;}
		float* getSpecular(){return this->specular;}
		char* getType(){return this->type;}
		CGFappearance* getApp(){return this->app;}
		Texture* getTexture(){return this->texture;}
};

class Primitive
{
		char* type;
	public:
		Primitive(){}
		Primitive(char* type){this->type = type;}
		virtual vector<float> getValues() = 0;
		virtual void setValues(vector<float>v) = 0;
		virtual void draw(float texS, float texT)=0;
};

class Rectangle : public Primitive, CGFobject
{
	float x1, y1, x2, y2;
	vector<float>v;
	public:
		Rectangle() : Primitive("rectangle"){}
		vector<float> getValues(){return v;}
		void setValues(vector<float>v)
		{
			this->v=v;
			this->x1 = v.at(0); this->y1 = v.at(1); this->x2 = v.at(2); this->y2 = v.at(3);
		}
		void draw(float texS, float texT);
};

class Cube : public Primitive, CGFobject
{
	Primitive* face;
	vector<float>v;
	public:
		Cube() : Primitive("cube"){
			this->face = new Rectangle();
			this->v.push_back(0.0);
			this->v.push_back(0.0);
			this->v.push_back(1.0);
			this->v.push_back(1.0);
			this->face->setValues(this->v);
		}
		vector<float> getValues(){return v;}
		void setValues(vector<float>v){
			this->v=v;
		}
		void draw(float texS, float texT);
};

class Cylinder : public Primitive, CGFobject
{
	float base, top, height, slices, stacks;
	vector<float>v;
	public:
		Cylinder() : Primitive("cylinder"){}
		void setValues(vector<float>v)
		{
			this->v=v;
			this->base=v.at(0);
			this->top=v.at(1);
			this->height=v.at(2);
			this->slices=v.at(3);
			this->stacks=v.at(4);
		}
		vector<float>getValues(){return this->v;}
		void draw(float texS, float texT);
};

class Sphere : public Primitive, CGFobject
{
	float radius, slices, stacks;
	vector<float>v;
	public:
	Sphere() : Primitive("sphere"){}
		void setValues(vector<float>v)
		{
			this->v=v;
			this->radius=v.at(0);
			this->slices=v.at(1);
			this->stacks=v.at(2);
		}
		vector<float>getValues(){return this->v;}
		void draw(float texS, float texT);
};

class Torus : public Primitive, CGFobject
{
	float inner, outer, slices, loops;
	vector<float>v;
public:
	Torus() : Primitive("torus"){}
	void setValues(vector<float>v)
	{
		this->v=v;
		this->inner=v.at(0); this->outer=v.at(1);
		this->slices=v.at(2); this->loops=v.at(3);
	}
	vector<float>getValues(){return this->v;}
	void draw(float texS, float texT);
};

class Ring : public Primitive, CGFobject
{
	GLUquadricObj* outer;
	GLUquadricObj* inner;
	float innerTop;
	float outerTop;
	float innerBase;
	float outerBase;
	float height;
	vector<float>v;
public:
	Ring(float innerBase, float innerTop, float outerBase, float outerTop, float height) : Primitive("ring"){
		this->innerBase = innerBase;
		this->innerTop = innerTop;
		this->outerTop = outerTop;
		this->outerBase = outerBase;
		this->height = height;
		this->inner = gluNewQuadric();
		this->outer = gluNewQuadric();
		gluQuadricDrawStyle(inner, GLU_FILL);
		gluQuadricNormals(inner, GLU_SMOOTH);
		gluQuadricOrientation(inner, GLU_INSIDE);
		gluQuadricDrawStyle(outer, GLU_FILL);
		gluQuadricNormals(outer, GLU_SMOOTH);
		gluQuadricOrientation(outer, GLU_OUTSIDE);
	}
	void setValues(vector<float>v){
		this->v=v;
	}
	vector<float>getValues(){return this->v;}
	void draw(float texS, float texT);
};

class InitVar
{
	Global* global;
	vector<Camera*> cameras;
	vector<Light*> lights;
	vector<Texture*> textures;
	vector<Appearance*> appearances;
	char* initialCamera;

	vector<Animation*> animations;

	public:

		InitVar(){global=new Global();}
		void setGlobal(Global* global){this->global=global;}
		void setCameras(vector<Camera*>cameras){this->cameras=cameras;}
		void setLights(vector<Light*>lights){this->lights=lights;}
		void setTextures(vector<Texture*>textures){this->textures=textures;}
		void setAppearances(vector<Appearance*>appearances){this->appearances=appearances;}
		
		vector<Camera*> getCameras(){return this->cameras;}
		vector<Light*> getLights(){return this->lights;}
		vector<Texture*> getTextures(){return this->textures;}
		vector<Appearance*> getAppearances(){return this->appearances;}

		Global* getGlobal(){return this->global;}
		Texture* getTexById(char* id);
		Appearance* getAppById(char* id);
		
		int getCameraById(char* id);
		void setInitialCamera(char* ic){this->initialCamera = ic;}
		char* getInitialCamera(){return this->initialCamera;}

		void setAnimations(vector<Animation*> animations){
			this->animations = animations;
		}
	
		vector<Animation*> getAnimations(){
			return this->animations;
		}
		

};


#endif