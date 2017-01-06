#include "TPscene.h"
#include "CGFaxis.h"
#include "CGFapplication.h"
#include "CGFappearance.h"
#include <string>
#include <ostream>

#define CAMERA_X 24.5

int lightId(int l)
{
	if(l==0) return GL_LIGHT0;
	if(l==1) return GL_LIGHT1;
	if(l==2) return GL_LIGHT2;
	if(l==3) return GL_LIGHT3;
	if(l==4) return GL_LIGHT4;
	if(l==5) return GL_LIGHT5;
	if(l==6) return GL_LIGHT6;
	if(l==7) return GL_LIGHT7;
	else return -1;
}

void TPscene :: updateCameraRotation(unsigned long deltaTime)
{
	this->canProcessInput = false;
		this->cameraTime += deltaTime;
		vector<float> cameraValues = this->iv->getCameras().at(0)->getValues();
		if(this->turn)
			cameraValues.push_back(1);
		else
			cameraValues.push_back(-1);
		vector<float> v = this->cameraMove->updateAnimation(cameraValues, this->cameraTime, deltaTime);
		if(v.at(9) == -1)//TIME TO STOP ANIMATING CAMERA
		{
			this->cameraRotation = false;
			this->cameraTime = 0;
			if(this->turn == true){//ends in the side of the black player
				v.at(3) = CAMERA_X;
				v.at(5) = 23.0;
			}
			else if(this->turn == false){//ends in the side of the white player
				v.at(3) = CAMERA_X;
				v.at(5) = 17.0;
			}
			v.erase(v.end()-1);
			this->iv->getCameras().at(0)->setValues(v);
			this->changeTurn();	
		}
		else if(v.at(9) == 1){
			v.erase(v.end()-1);
			this->iv->getCameras().at(0)->setValues(v);
		}
}

void TPscene :: endPieceAnimation(Piece *p)
{
	this->canProcessInput = true;
	p->setTime(0); 
	p->setAnimated(false);
	p->updateInitialPosition(p->getFinalX(), p->getFinalZ()); p->updateY(0.0);
	/*CHECK IF SAVING PIECE IS POSSIBLE*/
	if(p->getFinalZ() == 0 || p->getFinalZ() == 7)
	{
		if(this->latestPlayType == 9)//REACHED END FROM CAPTURE
		{
			this->board->captureMiddlePiece(p, this->lastX, this->lastY);
		}
		/*PIECE REACHED END OF BOARD, IS CAPTURED*/
		this->board->CapturePiece(p);
		string b = this->board->getCurrentBoardToString();
		int ok = this->prolog->mustSavePiece(this->turn, b);
		if(ok == -1){
			printf("YOU CANNOT SAVE A PIECE\n");
			this->canProcessInput = false;
			this->endTurnReady = true;
		}
		else if(ok == 1){
			printf("YOU MUST SAVE A PIECE\n");
			this->latestPlayType = 6;
			this->STATE = 1;
		}
	}
	else{
		printf("YOU HAVE NOT REACHED THE END OF THE BOARD.\n");
		if(this->latestPlayType == 7){/*SIMPLE MOVE CHANGE TURN*/
			p->setSelected(false);
			this->canProcessInput = false;
			this->endTurnReady = true;
		}
		else if(this->latestPlayType == 8 || this->latestPlayType == 9){
			if(this->latestPlayType == 9)
			{
				this->board->captureMiddlePiece(p, this->lastX, this->lastY);
			}
			string b = this->board->getCurrentBoardToString();
			int ok = this->prolog->mandatoryMovement(this->turn, p->getCenterZ(), p->getCenterX(), this->latestPlayType, b);
			if(ok == -1){
				printf("YOU CANNOT MOVE AGAIN.\n");
				p->setSelected(false);
				this->canProcessInput = false;
				this->endTurnReady = true;
			}
			else if(ok == 1){
				printf("YOU MUST MOVE AGAIN.\n");
				this->STATE = 1;
			}
		}
	}
}

void TPscene::update(unsigned long upTime)
{
	unsigned long deltaTime = upTime - time;
	if(deltaTime > 100) deltaTime = 100;
	this->time = upTime;
	if(this->cameraRotation == true)
	{
		this->updateCameraRotation(deltaTime);
	}
	for(unsigned int i = 0; i < this->board->getPieces().size(); i++)
	{
		Piece* p = this->board->getPieces().at(i);
		if(p->getAnimated())
		{
			this->canProcessInput = false;
			p->setTime(p->getTime() + deltaTime);
			vector<float> f;
			f.push_back(p->getCenterX()); f.push_back(p->getFinalX());
			f.push_back(p->getCenterZ()); f.push_back(p->getFinalZ());
			vector<float>v = this->moveAnimation->updateAnimation(f, p->getTime(), deltaTime);
			if(v.at(3) == -1)/*TIME TO STOP ANIMATING*/
			{
				this->endPieceAnimation(p);
			}
			else
			{
				p->updateInitialPosition(v.at(0), v.at(2));
				p->updateY(v.at(1));
			}
		}
	}
}

void TPscene::toggleLights()
{
	for(unsigned int i = 0; i < lights.size(); i++){
		if(lightCheck[i]==1){
			lights.at(i)->enable();
		}
		else if(lightCheck[i]==0){
			lights.at(i)->disable();
		}
	}
}

void TPscene::init() 
{
	Global *gl = iv->getGlobal();
	// Enables lighting computations

	if(gl->getEnabled() == GL_TRUE)
	{
		printf("LIGHTING ENABLED...\n");
		glEnable(GL_LIGHTING);
	}
	/**SHADE MODEL**/
	glShadeModel(gl->getShadingMode());

	if(gl->getCullFace() != 0)
	{
		printf("CULLFACE ENABLED...\n");
		glEnable(GL_CULL_FACE);
		glCullFace(gl->getCullFace());
		glFrontFace(gl->getOrder());
	}

	glEnable(GL_COLOR_MATERIAL);

	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, gl->getSided());
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, gl->getAmbient());
	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, gl->getLocal());

	glClearColor(gl->getBackground()[0], gl->getBackground()[1], gl->getBackground()[2], gl->getBackground()[3]);
	
	this->letThereBeLight();

	glNormal3f(0,0,1);
	glEnable(GL_NORMALIZE);

	this->drawingMode = iv->getGlobal()->getDrawingMode();

	this->cameraMode = iv->getCameraById(iv->getInitialCamera());

	this->freeCam = CGFscene::activeCamera;

	TPscene::time = 0;
	this->cameraTime = 0;
	setUpdatePeriod(40);

	this->turn = true;
	this->STATE = 0;

	this->board = new Board(this->iv->getAppearances());
	
	Board* _new = this->cloneBoard(this->board);
	this->prevBoards.push_back(_new);
	/*INITIATE THE ROOMS*/
	Room* livRoom = new LivingRoom(this->iv->getAppearances(), 0);
	Room* livRoom2 = new LivingRoom(this->iv->getAppearances(), 1);

	this->rooms.push_back(livRoom);
	this->rooms.push_back(livRoom2);
	
	this->prolog = new PrologConnection();
	this->prolog->socketConnect();
}

void TPscene::display() 
{
	// ---- BEGIN Background, camera and axis setup
	
	// Clear image and depth buffer everytime we update the scene
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	// Apply transformations corresponding to the camera position relative to the origin
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(this->freeCamera == 0)
	{
		CGFcamera* cam = (CGFcamera*)iv->getCameras().at(this->cameraMode);
		CGFscene::activeCamera = cam;
		CGFapplication::activeApp->forceRefresh();
	}
	else{
		CGFscene::activeCamera = this->freeCam;
		CGFapplication::activeApp->forceRefresh();
	}
	CGFscene::activeCamera->applyView();

	/**DRAWING THE LIGHTS**/
	/**UPDATE POSITION FOR BOTH, DIRECTION FOR SPOTLIGHT**/
	for(unsigned int i = 0; i < lights.size(); i++)
	{
		if(iv->getLights().at(i)->getMarker() == true)
		{
			lights.at(i)->draw();
		}
		else
			lights.at(i)->update();
		if(strcmp("spot", iv->getLights().at(i)->getType())==0)
		{
			float* direction = iv->getLights().at(i)->getDirection();
			GLfloat spotDir[] = {direction[0], direction[1], direction[2]};
			glLightfv(GL_LIGHT0+i, GL_SPOT_DIRECTION, spotDir); 
		}
	}

	// Draw axis
	axis.draw();

	// ---- END Background, camera and axis setup

	// ---- BEGIN Primitive drawing section

	glPolygonMode(GL_FRONT_AND_BACK, this->drawingMode);

	glPushMatrix();

	this->rooms.at(this->roomSet)->draw();

	this->board->draw(1.0, 1.0);

	glPopMatrix();


	// ---- END Primitive drawing section
	glutSwapBuffers();
}

void TPscene::letThereBeLight()
{
	printf("CREATING LIGHTS...\n");
	for(unsigned int i = 0; i < iv->getLights().size(); i++)
	{
		glPushMatrix();
		Light* l = iv->getLights().at(i);
		float light_pos[4] = {l->getLocation()[0], l->getLocation()[1], l->getLocation()[2], 1.0};
		float* ambient = l->getAmbient();
		float* diffuse = l->getDiffuse();
		float* specular = l->getSpecular();

		CGFlight* light = new CGFlight(lightId(i), light_pos);
		
		light->setAmbient(ambient);
		light->setDiffuse(diffuse);
		light->setSpecular(specular);
		
		if(strcmp(iv->getLights().at(i)->getType(),"spot")==0)
		{
			float angle = iv->getLights().at(i)->getAngle();
			float exp = iv->getLights().at(i)->getExponent();
			float* direction = iv->getLights().at(i)->getDirection();

			glLightf(lightId(i),GL_SPOT_CUTOFF, angle);
			glLightf(lightId(i),GL_SPOT_EXPONENT, exp);
			glLightfv(lightId(i),GL_SPOT_DIRECTION, direction);
		}
		
		if(l->getEnabled()==true)
			light->enable();
		else if(l->getEnabled()==false)
			light->disable();
		lights.push_back(light);

		glPopMatrix();
	}
}

void TPscene::chooseActiveCamera()
{
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	iv->getCameras().at(this->cameraMode)->setView();
	
}

void TPscene::processInput(int nselected, GLuint* selected)
{
	if(this->canProcessInput)
	{
		if(this->STATE == 0) // PICK THE PIECE
		{
			if(nselected == 1)
			{
				Piece* p = this->board->getPieceByName(selected[0]);
				string b = this->board->getCurrentBoardToString();
				int x_coord = p->getCenterZ();
				int y_coord = p->getCenterX();
				int ok = this->prolog->sendStartingPosition(this->turn, x_coord, y_coord, b);
				if(ok == 1){
					printf("VALID SELECTION.\n");
					this->board->setSelectedPiece(p);
					p->setSelected(true);
					this->STATE = 1;
				}
				else if(ok == -1){
					printf("INVALID SELECTION.\n");
					this->STATE = 0;
				}
			}
		}
		else if(this->STATE == 1) // PICK THE DESTINATION
		{
			if(nselected == 1)/*CHOOSE ANOTHER PIECE*/
			{
				Piece* p = this->board->getPieceByName(selected[0]);
				string b = this->board->getCurrentBoardToString();
				int x_coord = p->getCenterZ();
				int y_coord = p->getCenterX();
				//printf("Position of piece is %d %d\n", x_coord, y_coord);
				int ok = this->prolog->sendStartingPosition(this->turn, x_coord, y_coord, b);
				if(ok == 1){
					printf("VALID SELECTION.\n");
					this->board->setSelectedPiece(p);
					p->setSelected(true);
					this->STATE = 1;
				}
				else if(ok == -1){
					printf("INVALID SELECTION.\n");
					this->STATE = 0;
				}
			}
			else if(nselected == 2)
			{
				int x = selected[0];
				int y = selected[1];
				Tile* t = this->board->getTile(x, y);
				Piece* p = this->board->getSelectedPiece();
				string b = this->board->getCurrentBoardToString();
				int x_coord = p->getCenterZ();
				int y_coord = p->getCenterX();
				int ok = this->prolog->sendDestination(this->turn, x_coord, y_coord, x, y, this->latestPlayType, b);
				if(this->latestPlayType == 6)/*Save piece*/
				{
					if(ok != -1){
						printf("VALID CHOICE TO SAVE PIECE.\n");
						this->pieceSaveCount++;
						this->board->savePiece(this->turn,x,y);
						if(this->pieceSaveCount >= 2){//SAVED THE MAX AMOUNT OF PIECES
							this->pieceSaveCount = 0;
							this->canProcessInput = false;
							this->endTurnReady = true;
						}
						else{
							string b = this->board->getCurrentBoardToString();
							int ok = this->prolog->mustSavePiece(this->turn, b);
							if(ok != -1){
								printf("YOU MUST SAVE ANOTHER PIECE.\n");
								this->STATE = 1;
							}
							else{
								printf("YOU CANNOT SAVE MORE PIECES.\n");
								this->pieceSaveCount = 0;
								this->canProcessInput = false;
								this->endTurnReady = true;
							}
						}		
					}
					else{
						printf("INVALID DESTINATION TO SAVE PIECE.\n");
						this->STATE = 1;
					}
				}
				else if(this->latestPlayType != 6){/*NORMAL PLAY*/
					if(ok != -1){
						this->latestPlayType = ok;
						printf("VALID DESTINATION. PLAY WAS %d\n", ok);
						p->updateFinalPosition(y, x);
						p->setAnimated(true);
						this->canProcessInput = false;
						this->lastX = p->getCenterZ();
						this->lastY = p->getCenterX();
					}
					else if(ok == -1){
						printf("INVALID DESTINATION.\n");
						this->STATE = 1;
					}
				}
			}
		}
	}
}

void TPscene::changeTurn()
{
	this->turn = !this->turn;
	this->canProcessInput = true;
	this->endTurnReady = false;
	string b = this->board->getCurrentBoardToString();
	int ok = this->checkVictoryConditions();
	if(ok == 1)//CAN STILL PLAY
	{
		this->latestPlayType = 0;
		Board* _new = this->cloneBoard(this->board);
		this->prevBoards.push_back(_new);
		printf("TURN CHANGE, PLAYER IS ");
		if(this->turn) printf("WHITE\n");
		else printf("BLACK\n");
		this->STATE = 0;
	}
	else if(ok == -1){//GAME OVER
		this->canProcessInput = false;
		printf("GAME OVER : ");
		if(this->turn) printf("BLACK WINS\n");
		else printf("WHITE WINS\n");
	}
}

void TPscene::undoPlay()
{
	if(this->STATE != 0){
		int last = this->prevBoards.size()-1;
		Board* newBoard = this->prevBoards.at(last);
		this->board = this->cloneBoard(newBoard);
		this->STATE = 0;
		this->endTurnReady = false;
		this->canProcessInput = true;
		this->latestPlayType = 0;
		printf("TURN IS RESET.\n");
	}
	else printf("CANNOT RESET TURN.\n");
}

Board* TPscene::cloneBoard(Board* _old)
{
	Board* _new = new Board(this->iv->getAppearances());
	
	_new->setCurrentWhitePiece(_old->getCurrentWhitePiece());
	_new->setCurrentBlackPiece(_old->getCurrentBlackPiece());
	_new->setCurrentWhiteTile(_old->getCurrentWhiteTile());
	_new->setCurrentBlackTile(_old->getCurrentBlackTile());

	_new->setCapBPieces(_old->getCapBPieces());
	_new->setCapWPieces(_old->getCapWPieces());

	for(unsigned int i = 0; i < _new->getPieces().size(); i++)
	{
		Piece* newP = _new->getPieces().at(i);
		Piece* oldP = _old->getPieces().at(i);

		newP->updateInitialPosition(oldP->getCenterX(), oldP->getCenterZ());
		newP->updateY(oldP->getCenterY());
		newP->setCapture(oldP->getCaptured());
	}

	return _new;
}

void TPscene::quitGame()
{
	this->prolog->quit();
	exit(0);
}

int TPscene::checkVictoryConditions()
{
	if(this->turn){//from black to white
		if(this->board->getCapWPieces() == 16){//all white pieces captured
			return -1;
		}
		else{//check possible moves
			for(unsigned int i = 0; i < this->board->getPieces().size(); i++){
				Piece* p = this->board->getPieces().at(i);
				if(!p->getCaptured() && strcmp("white", p->getType())==0){
					string b = this->board->getCurrentBoardToString();
					int ok = this->prolog->possibleMove(this->turn, b, p->getCenterZ(), p->getCenterX());
					if(ok == 1)
						return 1;
				}
			}
			return -1;
		}
	}
	else if(!this->turn){//from white to black
		if(this->board->getCapBPieces() == 16){//all black pieces captured
			return -1;	
		}
		else{
			for(unsigned int i = 0; i < this->board->getPieces().size(); i++){
				Piece* p = this->board->getPieces().at(i);
				if(!p->getCaptured() && strcmp("black", p->getType())==0){
					string b = this->board->getCurrentBoardToString();
					int ok = this->prolog->possibleMove(this->turn, b, p->getCenterZ(), p->getCenterX());
					if(ok == 1)
						return 1;
				}
			}
			return -1;
		}
	}
	return 1;
}