#ifndef TPinterface_H
#define TPinterface_H

#include "CGFinterface.h"
#include "SceneGraph.h"

class TPinterface: public CGFinterface {
	InitVar* iv;

	public:
		GLUI* glui;
		GLUI_Panel* lights;
		GLUI_Panel* actions;
		GLUI_Panel* app;
		GLUI_Panel* options;
		GLUI_Panel* difLvl;
		GLUI_Panel* gameMode;

		TPinterface(InitVar* iv);

		virtual void initGUI();
		virtual void processGUI(GLUI_Control *ctrl);
		virtual void processKeyboard(unsigned char key, int x, int y);
		virtual void processMouse(int button, int state, int x, int y);	
		void performPicking(int x, int y);
		void processHits(GLint hits, GLuint buffer[]); 
};


#endif
