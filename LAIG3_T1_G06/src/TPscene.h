#ifndef TPSCENE_H
#define TPSCENE_H

#include "CGFscene.h"
#include "Geometry.h"
#include "Prolog.h"
#include <vector>

using std::vector;

class TPscene : public CGFscene
{
	InitVar *iv;
	vector<CGFlight*> lights;
	GLenum drawingMode;
	Animation* moveAnimation;
	Animation* cameraMove;
	
public:
	int drawMode;
	int cameraMode;
	int* lightCheck;
	int freeCamera;

	int whitePieceApp;
	int blackPieceApp;
	int whiteTileApp;
	int blackTileApp;
	int board_set;
	int dificulty;
	int gameMode;

	bool turn;
	int STATE;

	bool canProcessInput;
	int latestPlayType;

	int lastX, lastY;

	bool gameOver;

	bool cameraRotation;
	bool endTurnReady;
	unsigned long cameraTime;

	int pieceSaveCount;

	Board* board;
	vector<Board*> prevBoards;
	PrologConnection* prolog;

	vector<Room*> rooms;
	int roomSet;

	CGFcamera* freeCam;

	TPscene( InitVar *iv)
	{
		this->iv=iv;
		this->lightCheck = (int*)malloc(sizeof(int)*iv->getLights().size());
		this->cameraMode = 0;
		this->STATE = 0;
		this->moveAnimation = new AnimationLinear("movementAnimation", 1);
		this->cameraMove = new AnimationCircular("cameraMovement", 1);
		this->canProcessInput = true;
		this->latestPlayType = 0;
		this->lastX = 0;
		this->lastY = 0;
		this->gameOver = false;
		this->cameraRotation = false;
		this->endTurnReady = false;
		this->cameraTime = 0;
		this->pieceSaveCount = 0;
		this->freeCamera = 0;
		this->roomSet = 0;
		this->gameMode = 0;
		this->dificulty = 0;
		this->board_set = 0;
	}
	void init();
	void display();
	void toggleLights();
	void letThereBeLight();
	void chooseActiveCamera();
	void quitGame();

	void setDrawingMode(GLenum drawingMode){this->drawingMode = drawingMode;}

	unsigned long time;
	virtual void update(unsigned long);

	void processInput(int nselected, GLuint* selected);
	void changeTurn();
	void undoPlay();
	int checkVictoryConditions();
	Board* cloneBoard(Board* _old);

	void updateCameraRotation(unsigned long deltaTime);
	void endPieceAnimation(Piece *p);
};

#endif