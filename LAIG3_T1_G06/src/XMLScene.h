#pragma once
#ifndef _XMLSCENE_H_
#define _XMLSCENE_H_

#include "tinyxml.h"
#include "string.h"
#include "SceneGraph.h"

using std::vector;

void postProcess(InitVar* iv);

bool checkRepeated(char* id, vector<char*> idList);

class XMLScene
{
public:
	XMLScene(char *filename, InitVar *iv);
	~XMLScene();

	static TiXmlElement *findChildByAttribute(TiXmlElement *parent,const char * attr, const char *val);
	
	vector<Camera*> cameras;
	vector<Light*> lights;
	vector<Texture*> textures;
	vector<Appearance*> appearances;
	vector<Animation*> animations;

	Texture* getTextureById(char* id);
	Appearance* getAppearanceById(char* id);
	Animation* getAnimationById(char* id);

	Global* registerGlobalVariables(TiXmlElement* globalElement);
	void registerCameras(TiXmlElement* cameraElement);
	void registerLights(TiXmlElement* lightElement);
	void registerTextures(TiXmlElement* textsElement);
	void registerApps(TiXmlElement* appElement);
	void registerAnimations(TiXmlElement* animationElement);

protected:

	TiXmlDocument* doc;

	TiXmlElement* globalElement; 
	TiXmlElement* appElement;
	TiXmlElement* textsElement;
	TiXmlElement* primElement;
	TiXmlElement* cameraElement;
	TiXmlElement* lightElement;
	TiXmlElement* animationElement;
};

#endif