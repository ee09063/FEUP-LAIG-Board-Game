#include "XMLScene.h"
#include <sys/stat.h>

int exist(char *name)
{
  struct stat   buffer;
  return (stat (name, &buffer) == 0);
}

Texture* XMLScene::getTextureById(char* id)
{
	for(unsigned int i = 0; i < this->textures.size(); i++){
		if(strcmp(this->textures.at(i)->getId(), id)==0){
			return this->textures.at(i);
		}
	}
	printf("ERROR: TEXTURE '%s' NOT FOUND...\n", id);
	getchar();
	exit(-1);
}

Appearance* XMLScene::getAppearanceById(char* id)
{
	for(unsigned int i = 0; i < this->appearances.size(); i++){
		if(strcmp(this->appearances.at(i)->getId(), id)==0){
			return this->appearances.at(i);
		}
	}
	printf("ERROR: APPEARANCE '%s' NOT FOUND...\n", id);
	getchar();
	exit(-1);
}

Animation* XMLScene::getAnimationById(char* id)
{
	for(unsigned int i = 0; i < this->animations.size(); i++){
		if(strcmp(id, this->animations.at(i)->getId())==0){
			return this->animations.at(i);
		}
	}
	printf("ERROR: ANIMATION '%s' NOT FOUND. EXITING...\n", id);
	getchar();
	exit(-1);
}

XMLScene::XMLScene(char *filename, InitVar *iv)
{
	doc=new TiXmlDocument( filename );
	bool loadOkay = doc->LoadFile();

	if ( !loadOkay )
	{
		printf( "COULD NOT LOAD FILE '%s'. ERROR ='%s'. EXITING...\n", filename, doc->ErrorDesc() );
		getchar();
		exit( 1 );
	}
	else {
		printf("LOADED FILE '%s'\n", filename);
	}

	TiXmlElement* anfElement= doc->FirstChildElement( "anf" );

	if (anfElement == NULL)
	{
		printf("ANF BLOCK NOT FOUND. EXITING...\n");
		getchar();
		exit(1);
	}
			
	globalElement = anfElement->FirstChildElement( "globals" );
	appElement = anfElement->FirstChildElement( "appearances" );
	textsElement =  anfElement->FirstChildElement( "textures" );
	cameraElement = anfElement->FirstChildElement("cameras");
	lightElement = anfElement->FirstChildElement("lights");
	animationElement = anfElement->FirstChildElement("animations");
	
	if (globalElement == NULL){
		printf("GLOBAL BLOCK NOT FOUND. EXITING...\n");
		getchar();
		exit(-1);
	}
	else
	{
		Global* gl = registerGlobalVariables(globalElement);
		iv->setGlobal(gl);
		
		/**CAMERAS HERE*/
		if(cameraElement)
		{
			char* initialCamera;
			if(cameraElement->Attribute("initial")){
				initialCamera = (char*)cameraElement->Attribute("initial");
			}
			else{
				printf("INITIAL CAMERA NOT SET. EXITING...\n");
				getchar();
				exit(-1);
			}
			iv->setInitialCamera(initialCamera);
			printf("REGISTERING CAMERAS...\n");
			registerCameras(cameraElement);
			iv->setCameras(this->cameras);
			if(iv->getCameraById(iv->getInitialCamera()) == -1){
				printf("ERROR: INITIAL CAMERA NOT FOUND. EXITING...\n");
				getchar();
				exit(-1);
			}
		}
		else{
			printf("CAMERA BLOCK NOT FOUND. EXITING...\n");
			getchar();
			exit(-1);
		}
	
		/**LIGHTS HERE**/
		if(lightElement)
		{
			printf("REGISTERING LIGHTS...\n");
			registerLights(lightElement);
			iv->setLights(this->lights);
		}
		else{
			printf("LIGHT BLOCK NOT FOUND. EXITING...\n");
			getchar();
			exit(-1);
		}
		
		/*TEXTURAS*/
		if(textsElement)
		{
			printf("REGISTERING TEXTURES...\n");
			registerTextures(textsElement);
			iv->setTextures(this->textures);
		}
		else{
			printf("TEXTURE BLOCK NOT FOUND. EXITING...\n");
			getchar();
			exit(-1);
		}
		
		/*APPEARANCES*/
		if(appElement)
		{
			printf("REGISTERING APPEARANCES...\n");
			registerApps(appElement);
			iv->setAppearances(this->appearances);
		}
		else{
			printf("APPEARANCE BLOCK NOT FOUND. EXITING...\n");
			getchar();
			exit(-1);
		}
	
		if(animationElement)
		{
			printf("REGISTERING ANIMATIONS...\n");
			registerAnimations(animationElement);
			iv->setAnimations(this->animations);
			for(unsigned int i = 0; i < iv->getAnimations().size();i++)
			{
				printf("ANIMATION SIZE %d\n", iv->getAnimations().size());
				printf("%s", iv->getAnimations().at(i)->getId());
			}
		}
		else printf("ANIMATION ELEMENT NOT DETECTED.\n");
	}
}

XMLScene::~XMLScene()
{
	delete(doc);
}

Global* XMLScene::registerGlobalVariables(TiXmlElement* globalElement)
{
	Global* gl = new Global();
	printf("Processing global variables:\n");
	/*DRAWING ELEMENT*/
	char* drawingMode=NULL;
	char* shading=NULL;
	float* background = (float*)malloc(sizeof(float)*4);
	/**CULLING ELEMENT**/
	/**USUALLY IS CCW**/
	char* face=NULL;
	char* order=NULL;
	/*LIGHTING ELEMENT*/
	bool local;
	bool enabled;
	bool doublesided;
	float* ambientLight = (float*)malloc(sizeof(float)*4);
	
	/*READING DRAWING ELEMENTS*/
	TiXmlElement* drawingElement=globalElement->FirstChildElement("drawing");
	if (drawingElement)
	{
		if(drawingElement->Attribute("mode")){
			drawingMode=(char*)drawingElement->Attribute("mode");
		}
		else{
			printf("ERROR: ATTRIBUTE MODE NOT FOUND IN DRAWING SECTION. EXITING...\n");
			getchar();
			exit(-1);
		}
		if(drawingElement->Attribute("shading")){
			shading = (char*)drawingElement->Attribute("shading");
		}
		else{
			printf("ERROR: ATTRIBUTE SHADING NOT FOUND IN DRAWING SECTION. EXITING...\n");
			getchar();
			exit(-1);
		}
		char *valString=NULL;
		float backA, backB, backC, backD;
		
		if(drawingElement->Attribute("background")){
			valString=(char *) drawingElement->Attribute("background");
			
			if(valString && sscanf_s(valString,"%f %f %f %f",&backA, &backB, &backC, &backD)==4)
			{
				background[0]=backA; background[1]=backB; background[2]=backC; background[3]=backD;
			}
			else{
				printf("ERROR PARSING BACKGROUND. EXITING...\n");
				getchar();
				exit(-1);
			}
		}
		else{
			printf("ERROR: ATTRIBUTE BACKGROUND NOT FOUND IN DRAWING SECTION. EXITING...\n");
			getchar();
			exit(-1);
		}
	}
	else{
		printf("ERROR: DRAWING SECTION NOT FOUND. EXITING...\n");
		getchar();
		exit(-1);
	}

	/**READING CULLING ELEMENTS*/
	TiXmlElement* cullingElement = globalElement->FirstChildElement("culling");
	if(cullingElement)
	{
		printf("Processing culling elements\n");
		if(cullingElement->Attribute("face")){
			face = (char*)cullingElement->Attribute("face");
		}
		else{
			printf("ERROR: ATTRIBUTE FACE NOT FOUND IN CULLING SECTION. EXITING...\n");
			getchar();
			exit(-1);
		}
		if(cullingElement->Attribute("order")){
			order = (char*)cullingElement->Attribute("order");
		}
		else{
			printf("ERROR: ATTRIBUTE ORDER NOT FOUND IN CULLING SECTION. EXITING...\n");
			getchar();
			exit(-1);
		}
	}
	else{
		printf("ERROR: CULLING SECTION NOT FOUND. EXITING...\n");
		getchar();
		exit(-1);
	}

	/**READING LIGHT ELEMENTS*/
	TiXmlElement* lightingElement = globalElement->FirstChildElement("lighting");
	if(lightingElement)
	{
		printf("Processing lighting elements\n");
		float ambA, ambB, ambC, ambD;

		char *valString=NULL;

		char* sdoublesided;
		if(lightingElement->Attribute("doublesided")){
			sdoublesided = (char*)lightingElement->Attribute("doublesided");
			if(strcmp(sdoublesided, "true")==0){doublesided=true;}
			else if(strcmp(sdoublesided, "false")==0){doublesided=false;}
			else{
				printf("ERROR: NOT ACCEPTED VALUES IN ATTRIBUTE DOUBLESIDED. EXITING...\n");
				getchar();
				exit(-1);
			}
		}
		else{
			printf("ERROR: ATTRIBUTE DOUBLESIDED NOT FOUND IN LIGHTING SECTION. EXITING...\n");
			getchar();
			exit(-1);
		}

		char* slocal;
		if(lightingElement->Attribute("local")){
			slocal= (char*)lightingElement->Attribute("local");
			if(strcmp(slocal, "true")==0){local=true;}
			else if(strcmp(slocal, "false")==0){local=false;}
			else{
				printf("ERROR: NOT ACCEPTED VALUES IN ATTRIBUTE LOCAL. EXITING...\n");
				getchar();
				exit(-1);
			}
		}
		else{
			printf("ERROR: ATTRIBUTE LOCAL NOT FOUND IN LIGHTING SECTION. EXITING...\n");
			getchar();
			exit(-1);
		}
		
		char* senabled;
		if(lightingElement->Attribute("enabled")){
			senabled = (char*)lightingElement->Attribute("enabled");
			if(strcmp(senabled, "true")==0){enabled=true;}
			else if(strcmp(senabled, "false")==0){enabled=false;}
			else{
				printf("ERROR: NOT ACCEPTED VALUES IN ATTRIBUTE ENABLED. EXITING...\n");
				getchar();
				exit(-1);
			}
		}
			else{
			printf("ERROR: ATTRIBUTE ENABLED NOT FOUND IN LIGHTING SECTION. EXITING...\n");
			getchar();
			exit(-1);
		}

		
		valString = (char*)lightingElement->Attribute("ambient");
		
		if(valString && sscanf_s(valString,"%f %f %f %f",&ambA, &ambB, &ambC, &ambD)==4)
		{
			ambientLight[0]=ambA;
			ambientLight[1]=ambB;
			ambientLight[2]=ambC;
			ambientLight[3]=ambD;
		}
		else{
			printf("ERROR PARSING AMBIENT ATRIBUTTE IN GLOBAL ELEMENTS. EXITING...\n");
			getchar();
			exit(-1);
		}
	}
	else{
		printf("LIGHTING SECTION NOT FOUND. EXITING...\n");
		getchar();
		exit(-1);
	}

	/**NOW TO FINALLY REGISTER THE GLOBAL ELEMENTS**/
	printf("REGISTERING GLOBAL VARIABLES\n");
	gl->setDrawingElements(drawingMode, shading, background);
	gl->setCullingElements(face, order);
	gl->setLigtingElements(doublesided, local, enabled, ambientLight);
	
	return gl;
}

void XMLScene::registerCameras(TiXmlElement* cameraElement)
{
	vector<char*> idList;
	for(TiXmlElement *cam = cameraElement->FirstChildElement(); cam; cam = cam->NextSiblingElement())
	{
		Camera* c = NULL;
		char* cameraId;
		if(cam->Attribute("id")){
			cameraId = (char*)cam->Attribute("id");
		}
		else{
			printf("ERROR: ATRIBUTTE ID MISSING IN CAMERA SECTION. EXITING...\n");
			getchar();
			exit(-1);
		}
		if(checkRepeated(cameraId, idList))
		{
			printf("REPEATED CAMERA NAME DETECTED -> '%s'. EXITING...\n", cameraId);
			getchar();
			exit(-1);
		}
		idList.push_back(cameraId);

		if(strcmp(cam->Value(), "perspective")==0)
		{
			char* valString=(char*)cam->Attribute("pos");
			char* valString2=(char*)cam->Attribute("target");
			float near, far, angle, posx, posy, posz, targx, targy, targz;
			if (cam->QueryFloatAttribute("near",&near)==TIXML_SUCCESS &&
				cam->QueryFloatAttribute("far",&far)==TIXML_SUCCESS &&
				cam->QueryFloatAttribute("angle",&angle)==TIXML_SUCCESS &&
				valString && sscanf_s(valString,"%f %f %f",&posx, &posy, &posz)==3 &&
				valString2 && sscanf_s(valString2,"%f %f %f",&targx, &targy, &targz)==3)
			{
				c = new CamPerspective(cameraId);
				vector<float>values;
				values.push_back(near); values.push_back(far); values.push_back(angle);
				values.push_back(posx); values.push_back(posy); values.push_back(posz);
				values.push_back(targx); values.push_back(targy); values.push_back(targz);
				c->setValues(values);
				cameras.push_back(c);
			}
			else{
				printf("ERROR PARSING ATTRIBUTES OF CAMERA '%s'. EXITING...\n", cam->Attribute("id"));
				getchar();
				exit(-1);
			}
					
		}
		else if(strcmp(cam->Value(), "ortho")==0)
		{
			float near, far, right, left, top, bottom, direction;
			char* dir=(char*)cam->Attribute("direction");
			if (cam->QueryFloatAttribute("near",&near)==TIXML_SUCCESS &&
				cam->QueryFloatAttribute("far",&far)==TIXML_SUCCESS &&
				cam->QueryFloatAttribute("right",&right)==TIXML_SUCCESS &&
				cam->QueryFloatAttribute("left",&left)==TIXML_SUCCESS &&
				cam->QueryFloatAttribute("top",&top)==TIXML_SUCCESS &&
				cam->QueryFloatAttribute("bottom",&bottom)==TIXML_SUCCESS &&
				dir)
			{
				c = new CamOrtho(cameraId);
				vector<float>values;
				
				if(strcmp("x", dir)==0) direction=0;
				else if(strcmp("y", dir)==0) direction=1;
				else if(strcmp("z", dir)==0) direction=2;
				else{
					printf("ERROR: WRONG VALUE IN DIRECTION FOR CAMERA '%s'. EXITING...\n", cam->Attribute("id"));
					getchar();
					exit(-1);
				}

				values.push_back(near); values.push_back(far); values.push_back(left);
				values.push_back(right); values.push_back(top); values.push_back(bottom);
				values.push_back(direction);
				c->setValues(values);
				this->cameras.push_back(c);
			}
			else{
				printf("ERROR PARSING ATTRIBUTES OF CAMERA '%s'. EXITING...\n", cam->Attribute("id"));
				getchar();
				exit(-1);
			}
		}
		else{
				printf("ERROR PARSING TYPE OF CAMERA '%s'. EXITING...\n", cam->Attribute("id"));
				getchar();
				exit(-1);
		}
	}
	printf("Number of cameras -> '%d'\n", cameras.size());
	if(cameras.size() == 0){
		printf("ERROR: CAMERA BLOCK IS EMPTY. EXITING...\n");
		getchar();
		exit(-1);
	}
}

void XMLScene::registerLights(TiXmlElement* lightElement)
{
	vector<char*> idList;
	int lightCount = 0;
	for(TiXmlElement *l = lightElement->FirstChildElement(); l; l=l->NextSiblingElement())
	{
		if(lightCount > 7)
		{
			printf("ERROR: ONLY 8 LIGHTS SUPPORTED. EXITING...\n");
			getchar();
			exit(1);
		}
		Light* light;
		char* id;
		if(l->Attribute("id")){
			id=(char*)l->Attribute("id");
		}
		else{
			printf("ERROR: ID NOT FOUND IN LIGHT. EXITING...\n");
			getchar();
			exit(-1);
		}
		if(checkRepeated(id, idList))
		{
			printf("REPEATED LIGHT NAME DETECTED -> '%s'", id);
			getchar();
			exit(-1);
		}
		idList.push_back(id);

		char* type;
		if(l->Attribute("type")){
			type=(char*)l->Attribute("type");
		}
		else{
			printf("ERROR: TYPE NOT FOUND IN LIGHT '%s'. EXITING...\n",id);
			getchar();
			exit(-1);
		}
		bool enabled=NULL;
		bool marker=NULL;
		
		if(l->Attribute("enabled")){
			if(strcmp("true",(char*)l->Attribute("enabled"))==0){enabled=true;}
			else if(strcmp("false",(char*)l->Attribute("enabled"))==0){enabled=false;}
			else{
				printf("ERROR: WRONG VALUE IN ENABLED ATRIBUTTE IN LIGHT '%s'. EXITING...", id);
				getchar();
				exit(-1);
			}
		}
		else{
			printf("ERROR: ATRIBUTTE ENABLED NOT FOUND IN LIGHT '%s'. EXITING...",id);
			getchar(),
			exit(-1);
		}
					
		if(l->Attribute("marker")){
			if(strcmp("true",(char*)l->Attribute("marker"))==0){marker=true;}
			else if(strcmp("false",(char*)l->Attribute("marker"))==0){marker=false;}
			else{
				printf("ERROR: WRONG VALUE IN MARKER ATRIBUTTE IN LIGHT '%s'. EXITING...", id);
				getchar();
				exit(-1);
			}
		}
		else{
			printf("ERROR: ATRIBUTTE MARKER NOT FOUND IN LIGHT '%s'. EXITING...",id);
			getchar(),
			exit(-1);
		}

		char* valString;
		float locx, locy, locz;
		vector<float>location;

		valString=(char*)l->Attribute("pos");
		if (valString && sscanf_s(valString,"%f %f %f",&locx, &locy, &locz)==3)
		{
			location.push_back(locx); location.push_back(locy); location.push_back(locz);
		}
		else{
			printf("ERROR PARSING POS ATTRIBUTE IN LIGHT '%s'. EXITING...", id);
			getchar();
			exit(-1);
		}
		
		float* ambient = (float*)malloc(sizeof(float)*4);
		float* diffuse = (float*)malloc(sizeof(float)*4);
		float* specular = (float*)malloc(sizeof(float)*4);
		/**COMPONENTS**/
		bool a = false, d = false, s = false;
		for(TiXmlElement* c = l->FirstChildElement(); c; c=c->NextSiblingElement())
		{
			float cA, cB, cC, cD;
			if(strcmp(c->Attribute("type"), "ambient")==0)
			{
				valString = (char*)c->Attribute("value");
				if (valString && sscanf_s(valString,"%f %f %f %f",&cA, &cB, &cC, &cD)==4)
				{
					ambient[0]=cA; ambient[1]=cB; ambient[2]=cC; ambient[3]=cD;
					a = true;

				}
				else{
					printf("ERROR PARSING AMBIENT ATTRIBUTE IN LIGHT '%s'. EXITING...", id);
					getchar();
					exit(-1);
				}
			}
			else if(strcmp(c->Attribute("type"), "diffuse")==0)
			{
				valString = (char*)c->Attribute("value");
				if (valString && sscanf_s(valString,"%f %f %f %f",&cA, &cB, &cC, &cD)==4)
				{
					diffuse[0]=cA; diffuse[1]=cB; diffuse[2]=cC; diffuse[3]=cD;
					d = true;
				}
				else{
					printf("ERROR PARSING DIFFUSE ATTRIBUTE IN LIGHT '%s'. EXITING...", id);
					getchar();
					exit(-1);
				}
			}
			else if(strcmp(c->Attribute("type"), "specular")==0)
			{
				valString = (char*)c->Attribute("value");
				if (valString && sscanf_s(valString,"%f %f %f %f",&cA, &cB, &cC, &cD)==4)
				{
					specular[0]=cA; specular[1]=cB; specular[2]=cC; specular[3]=cD;
					s = true;
				}
				else{
					printf("ERROR PARSING SPECULAR ATTRIBUTE IN LIGHT '%s'. EXITING...", id);
					getchar();
					exit(-1);
				}
			}
			else{
				printf("ERROR PARSING COMPONENT IN LIGHT '%s'. NAME NOT FOUND. EXITING...", id);
				getchar();
				exit(-1);
			}
		}
		if(a && s && d){
			}
		else {
			printf("ERROR: COMPONENT MISSING IN LIGHT '%s'. EXITING...\n", id);
			getchar();
			exit(-1);
		}

		if(strcmp(type,"omni")==0)
		{
			float* direction = NULL;
			light = new Light(id, "omni", enabled, marker, 0.0, 0.0, location, direction, ambient, diffuse, specular);
			this->lights.push_back(light);
			
		}
		else if(strcmp(type,"spot")==0)
		{
			float angle;
			float exponent;
			char* valString = (char*)l->Attribute("target");
			float dA, dB, dC;
			float* direction = (float*)malloc(sizeof(float)*3);
			if(l->QueryFloatAttribute("angle",&angle)==TIXML_SUCCESS &&
			   l->QueryFloatAttribute("exponent",&exponent)==TIXML_SUCCESS && 
			   valString && sscanf_s(valString,"%f %f %f",&dA, &dB, &dC)==3)
			{
				direction[0]=dA; direction[1]=dB; direction[2]=dC;
			}
			else printf("PROBLEM READING TARGET, ANGLE OR EXPONENT IN LIGHT '%s'\n", id);

			light=new Light(id, "spot", enabled, marker, angle, exponent, location, direction, ambient, diffuse, specular);
			this->lights.push_back(light);
		}
		else{
			printf("ERROR PARSING LIGHT TYPE ATTRIBUTE IN LIGHT '%s'. EXITING...", id);
			getchar();
			exit(-1);
		}
		lightCount++;
	}
	printf("Number of lights -> '%d'\n", lights.size());
}

void XMLScene::registerTextures(TiXmlElement* textsElement)
{
	vector<char*> idList;
	for(TiXmlElement *tex = textsElement->FirstChildElement(); tex; tex = tex->NextSiblingElement())
	{
		char* id;
		if(tex->Attribute("id")){
			id = (char*)tex->Attribute("id");
		}
		else{
			printf("ERROR: ATTRIBUTE ID MISSING IN TEXTURE SECTION. EXITING...\n");
			getchar();
			exit(-1);
		}

		if(checkRepeated(id, idList))
		{
			printf("REPEATED NAME DETECTED IN TEXTURES -> '%s'", id);
			getchar();
			exit(-1);
		}
		idList.push_back(id);
		
		char* file;
		if(tex->Attribute("file")){
			file=(char*)tex->Attribute("file");
			if(!exist(file)){
				printf("ERROR: FILE '%s' DOES NOT EXIST -> TEXTURE '%s'. EXITING...\n", file,id);
				getchar();
				exit(-1);
			}
		}
		else{
			printf("ATRIBUTTE FILE DOES NOT EXIST IN TEXTURE '%s'. EXITING...\n", id);
			getchar();
			exit(-1);
		}

		float lengthT, lengthS;
		if(tex->QueryFloatAttribute("texlength_s",&lengthS)==TIXML_SUCCESS &&
			tex->QueryFloatAttribute("texlength_t",&lengthT)==TIXML_SUCCESS)
		{
			Texture* t = new Texture(id, file, lengthS, lengthT);
			this->textures.push_back(t);
		}
		else{
			printf("ERROR PARSING LENGHT T AND S IN TEXTURE '%s'. EXITING...\n", id);
			getchar();
			exit(-1);
		}
	}
	printf("Number of textures -> '%d'\n", textures.size());
}

void XMLScene::registerApps(TiXmlElement* appElement){
	vector<char*> idList;

	for(TiXmlElement *app = appElement->FirstChildElement(); app; app=app->NextSiblingElement())
	{	
		char* id;
		if(app->Attribute("id")){
			id = (char*)app->Attribute("id");
		}
		else{
			printf("ERROR: ATTRIBUTE ID MISSING IN TEXTURE SECTION. EXITING...\n");
			getchar();
			exit(-1);
		}

		if(checkRepeated(id, idList)){
			printf("REPEATED APPEARANCE NAME DETECTED -> '%s'", id);
			getchar();
			exit(-1);
		}

		idList.push_back(id);

		char* textureref="";
		Texture* texture;
		if(app->Attribute("textureref"))
		{
			textureref=(char*)app->Attribute("textureref");
			texture = this->getTextureById(textureref);
		}
		else if(strcmp(textureref, "") == 0)
		{
			texture = new Texture("", "", 1.0, 1.0);
		}

		float* ambient = (float*)malloc(sizeof(float)*4);
		float* diffuse = (float*)malloc(sizeof(float)*4);
		float* specular = (float*)malloc(sizeof(float)*4);
		char* valString = NULL;
		float shininess;
		
		if(app->QueryFloatAttribute("shininess",&shininess)==TIXML_SUCCESS){}
		else{
			printf("ERROR PARSING SHININESS IN APPEARANCE '%s'. EXITING...\n", id);
			getchar();
			exit(-1);
		}

		char* appType="";
		if(app->Attribute("type"))
		{
			appType=(char*)app->Attribute("type");
		}

		bool a = false, d = false, s = false;
		for(TiXmlElement* c = app->FirstChildElement(); c; c=c->NextSiblingElement())
		{
			float cA, cB, cC, cD;
			if(strcmp(c->Attribute("type"), "ambient")==0){
				valString = (char*)c->Attribute("value");
				if (valString && sscanf_s(valString,"%f %f %f %f",&cA, &cB, &cC, &cD)==4){
					ambient[0]=cA; ambient[1]=cB; ambient[2]=cC; ambient[3]=cD;
					a = true;
				}
				else{
					printf("ERROR PARSING AMBIENT IN APPEARANCE '%s'. EXITING...\n", id);
					getchar();
					exit(-1);
				}
			}
			else if(strcmp(c->Attribute("type"), "diffuse")==0){
				valString = (char*)c->Attribute("value");
				if (valString && sscanf_s(valString,"%f %f %f %f",&cA, &cB, &cC, &cD)==4){
					diffuse[0]=cA; diffuse[1]=cB; diffuse[2]=cC; diffuse[3]=cD;
					d = true;
				}
				else{
					printf("ERROR PARSING DIFFUSE IN APPEARANCE '%s'. EXITING...\n", id);
					getchar();
					exit(-1);
				}
			}
			else if(strcmp(c->Attribute("type"), "specular")==0){
				valString = (char*)c->Attribute("value");
				if (valString && sscanf_s(valString,"%f %f %f %f",&cA, &cB, &cC, &cD)==4){
					specular[0]=cA; specular[1]=cB; specular[2]=cC; specular[3]=cD;
					s = true;
				}
				else{
					printf("ERROR PARSING SPECULAR IN APPEARANCE '%s'. EXITING...\n", id);
					getchar();
					exit(-1);
				}
			}
		}
		if(a && s && d){
			}
		else {
			printf("ERROR: COMPONENT MISSING IN APPEARANCE '%s'. EXITING...\n", id);
			getchar();
			exit(-1);
		}
		Appearance* appearance = new Appearance(id,texture,shininess,ambient,diffuse,specular, appType);
		this->appearances.push_back(appearance);
	}
	printf("Number of appearances -> '%d'\n", this->appearances.size());
}

void XMLScene::registerAnimations(TiXmlElement* animationElement)
{
	/*
	vector<char*> idList;
	for(TiXmlElement* anime = animationElement->FirstChildElement(); anime; anime=anime->NextSiblingElement())
	{	
		Animation* animation = NULL;
		char* id;
		if(anime->Attribute("id")){
			id = (char*)anime->Attribute("id");
		}
		else{
			printf("ERROR: ATTRIBUTE ID MISSING IN ANIMATION SECTION. EXITING...\n");
			getchar();
			exit(-1);
		}

		if(checkRepeated(id, idList)){
			printf("REPEATED ANIMATION NAME DETECTED -> '%s'", id);
			getchar();
			exit(-1);
		}

		idList.push_back(id);

		
		char* animationType;
		if(anime->Attribute("type"))
		{
			animationType = (char*)anime->Attribute("type");
		}
		else{
			printf("ANIMATION TYPE MISSING IN ANIMATION %s. EXITING...\n", id);
			getchar();
			exit(-1);
		}
		
		if(strcmp("linear", animationType)==0)
		{
			float timeSpan;
			if(anime->QueryFloatAttribute("span",&timeSpan)==TIXML_SUCCESS){}
			else{
				printf("ERROR PARSING TIME SPAN IN ANIMATION '%s'. EXITING...\n", id);
				getchar();
				exit(-1);
			}
			animation = new AnimationLinear(id, timeSpan);			
		}
		else if(strcmp("circular", animationType)==0)
		{
			/*
			float startang, rotang, radius;
			float span;
			float* center = (float*)malloc(sizeof(float)*3);
			char* valString = NULL;
			valString = (char*)anime->Attribute("center");

			if(anime->QueryFloatAttribute("span",&span)==TIXML_SUCCESS &&
				anime->QueryFloatAttribute("startang",&startang)==TIXML_SUCCESS &&
				anime->QueryFloatAttribute("rotang",&rotang)==TIXML_SUCCESS &&
				anime->QueryFloatAttribute("radius",&radius)==TIXML_SUCCESS &&
				valString && sscanf_s(valString,"%f %f %f",&center[0], &center[1], &center[2])==3)
			{
				animation = new AnimationCircular(id, startang, rotang, center, radius, span);  
			}
			else
			{
				printf("ERROR PARSING ATTRIBUTES IN ANIMATION '%s'. EXITING...\n",id);
				getchar();
				exit(-1);
			}
		}
		else{
			printf("INVALID ANIMATION TYPE IN ANIMATION '%s'", id);
		}
		this->animations.push_back(animation);
	}
	*/
	printf("Number of animations -> '%d'\n", this->animations.size());
}

bool checkRepeated(char* id, vector<char*> idList)
{
	for(unsigned int i = 0; i < idList.size(); i++)
	{
		if(strcmp(id, idList.at(i))==0)
			return true;
	}
	return false;
}

