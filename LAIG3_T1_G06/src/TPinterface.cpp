#include "TPinterface.h"
#include "TPscene.h"
#include "CGFapplication.h"
#include <iostream>

TPinterface::TPinterface(InitVar* iv)
{
	this->iv=iv;
}

void TPinterface::initGUI()
{
	printf("INITIATING USER INTERFACE...\n");
	
	/*EXTERNAL INTERFACE*/
	this->glui = GLUI_Master.create_glui( "Eximo", 0, 400, 100 );
	
	new GLUI_StaticText(this->glui, "Interface");

	this->actions = new GLUI_Panel(this->glui, "Actions");

	GLUI_Button* undo = addButtonToPanel(this->actions, "Undo", 9);
	GLUI_Button* endTurn = addButtonToPanel(this->actions, "End Turn", 10);
	
	new GLUI_Separator(this->actions);
	
	new GLUI_Separator(this->actions);
	
	GLUI_Button* quit = addButtonToPanel(this->actions, "Quit Game", 12);
	
	new GLUI_Separator(this->glui);

	this->options = new GLUI_Panel(this->glui, "Options");
	
	GLUI_Checkbox* freeCamera = addCheckboxToPanel(this->options, "Free Camera", &(((TPscene*) scene)->freeCamera), 11);

	new GLUI_Separator(this->options);

	this->difLvl = new GLUI_Panel(this->options, "Difficulty");
	GLUI_RadioGroup* dif = new GLUI_RadioGroup(this->difLvl, &(((TPscene*) scene)->dificulty), 14);
	addRadioButtonToGroup(dif, "Level 1");
	addRadioButtonToGroup(dif, "Level 2");

	new GLUI_Separator(this->options);
	
	this->gameMode = new GLUI_Panel(this->options, "Game Mode");
	GLUI_RadioGroup* gameM = new GLUI_RadioGroup(this->gameMode, &(((TPscene*) scene)->gameMode), 15);
	addRadioButtonToGroup(gameM, "Human vs Human");
	addRadioButtonToGroup(gameM, "Human vs PC");
	addRadioButtonToGroup(gameM, "PC vs PC");

	new GLUI_Separator(this->glui);

	this->lights = new GLUI_Panel(this->glui, "Lights");

	for(unsigned int i = 0; i < iv->getLights().size(); i++)
	{
		char* id = iv->getLights().at(i)->getId();
		GLUI_Checkbox* lightBox = addCheckboxToPanel(this->lights, id , &(((TPscene*) scene)->lightCheck[i]), 1);
		if(iv->getLights().at(i)->getEnabled()==false)
			lightBox->set_int_val(0);
		else if(iv->getLights().at(i)->getEnabled()==true)
			lightBox->set_int_val(1);
	}

	new GLUI_Separator(this->glui);

	this->app = new GLUI_Panel(this->glui, "Appearance");
	
	new GLUI_StaticText(this->app, "Rooms");

	GLUI_Listbox *rooms = addListboxToPanel(this->app, "", &(((TPscene*) scene)->roomSet),13);
	rooms->add_item(0, "Living Room v1");
	rooms->add_item(1, "Living Room v2");

	new GLUI_Separator(this->app);

	new GLUI_StaticText(this->app, "Board & Pieces");

	GLUI_Listbox *board = addListboxToPanel(this->app, "", &(((TPscene*) scene)->board_set),5);
	board->add_item(0, "Marble");
	board->add_item(1, "Wood");
	board->add_item(2, "Glass");
}

void TPinterface::processGUI(GLUI_Control *ctrl)
{
	switch(ctrl->user_id)
	{
	case 1:
		{
			(((TPscene*) scene)->toggleLights());
			break;
		};
	case 5:
		{
			(((TPscene*) scene)->board->setCurrentWhitePiece((((TPscene*) scene)->board_set)));
			(((TPscene*) scene)->board->setCurrentBlackPiece((((TPscene*) scene)->board_set)));
			(((TPscene*) scene)->board->setCurrentWhiteTile((((TPscene*) scene)->board_set)));
			(((TPscene*) scene)->board->setCurrentBlackTile((((TPscene*) scene)->board_set)));
			break;
		};
	case 9:
		{
			(((TPscene*) scene)->undoPlay());
			break;
		};
	case 10:
		{
			if((((TPscene*) scene)->endTurnReady)){
				(((TPscene*) scene)->cameraRotation) = true;
			}
			break;
		};
	case 12:
		{
			(((TPscene*) scene)->quitGame());
			break;
		};
	}
}

#define BUFSIZE 256
GLuint selectBuf[BUFSIZE];

void TPinterface::processMouse(int button, int state, int x, int y) 
{
	CGFinterface::processMouse(button,state, x, y);

	// do picking on mouse press (GLUT_DOWN)
	// this could be more elaborate, e.g. only performing picking when there is a click (DOWN followed by UP) on the same place
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
		performPicking(x,y);
}

void TPinterface::performPicking(int x, int y) 
{
	// Sets the buffer to be used for selection and activate selection mode
	glSelectBuffer (BUFSIZE, selectBuf);
	glRenderMode(GL_SELECT);

	// Initialize the picking name stack
	glInitNames();

	// The process of picking manipulates the projection matrix
	// so we will be activating, saving and manipulating it
	glMatrixMode(GL_PROJECTION);

	//store current projmatrix to restore easily in the end with a pop
	glPushMatrix ();

	//get the actual projection matrix values on an array of our own to multiply with pick matrix later
	GLfloat projmat[16];
	glGetFloatv(GL_PROJECTION_MATRIX,projmat);

	// reset projection matrix
	glLoadIdentity();

	// get current viewport and use it as reference for 
	// setting a small picking window of 5x5 pixels around mouse coordinates for picking
	GLint viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// this is multiplied in the projection matrix
	gluPickMatrix ((GLdouble) x, (GLdouble) (CGFapplication::height - y), 5.0, 5.0, viewport);

	// multiply the projection matrix stored in our array to ensure same conditions as in normal render
	glMultMatrixf(projmat);

	// force scene drawing under this mode
	// only the names of objects that fall in the 5x5 window will actually be stored in the buffer
	scene->display();

	// restore original projection matrix
	glMatrixMode (GL_PROJECTION);
	glPopMatrix ();

	glFlush();

	// revert to render mode, get the picking results and process them
	GLint hits;
	hits = glRenderMode(GL_RENDER);
	processHits(hits, selectBuf);
}

void TPinterface::processHits (GLint hits, GLuint buffer[]) 
{
	GLuint *ptr = buffer;
	GLuint mindepth = 0xFFFFFFFF;
	GLuint *selected=NULL;
	GLuint nselected;

	for (int i=0;i<hits;i++) {
		int num = *ptr; ptr++;
		GLuint z1 = *ptr; ptr++;
		ptr++;
		if (z1 < mindepth && num>0) {
			mindepth = z1;
			selected = ptr;
			nselected=num;
		}
		for (int j=0; j < num; j++) 
			ptr++;
	}
	
	if (selected!=NULL)
	{
		(((TPscene*) scene)->processInput(nselected, selected));
	}
}

void TPinterface::processKeyboard(unsigned char key, int x, int y)
{
	switch(key)
	{
		case 'q':
			{
				(((TPscene*) scene)->quitGame());
				break;
			}
		case 'w':
			{
				if((((TPscene*) scene)->endTurnReady)){
					(((TPscene*) scene)->cameraRotation) = true;
				}
				break;
			}
		case 'r':
		{
			(((TPscene*) scene)->undoPlay());
			break;
		}
	}
}

