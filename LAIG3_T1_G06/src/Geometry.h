#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "Furniture.h"
#include "SceneGraph.h"
#include <ostream>

#define SSTR( x ) dynamic_cast< std::ostringstream & >( \
        ( std::ostringstream() << std::dec << x ) ).str()

#define NUM_ROWS 8
#define SIDES 25

class Tile
{	
	float centerX;
	float centerY;
	float centerZ;
	char* type;
	Primitive* r;
	bool isSelected;
public:
	Tile(char* type, float centerX, float centerY, float centerZ);
	float getCenterX(){return this->centerX;}
	float getCenterY(){return this->centerY;}
	float getCenterZ(){return this->centerZ;}
	char* getType(){return this->type;}
	void draw(float texS, float texT);
	bool getSelected(){return this->isSelected;}
	void setSelected(bool selected){this->isSelected = selected;}
};

class Piece
{
	float centerX;
	float centerY;
	float centerZ;
	char* type;
	Primitive* c;
	Primitive* bearing;
	Primitive* thor;
	Primitive* ring;
	int name;
	bool isAnimated;
	bool isCaptured;
	bool isSelected;
	float finalX;
	float finalZ;
	unsigned long time;
public:
	Piece(char* type, float centerX, float centerY, float centerZ);
	float getCenterX(){return this->centerX;}
	float getCenterY(){return this->centerY;}
	float getCenterZ(){return this->centerZ;}
	float getFinalX(){return this->finalX;}
	float getFinalZ(){return this->finalZ;}
	unsigned long getTime(){return this->time;}
	void setTime(unsigned long time){this->time = time;}
	char* getType(){return this->type;}
	void setName(int name){this->name = name;}
	int getName(){return this->name;}
	
	void draw(float texS, float texT, Appearance* red);
	
	void updateFinalPosition(float x, float z){this->finalX = x; this->finalZ = z;}
	void updateInitialPosition(float x, float z){this->centerX = x; this->centerZ = z;}
	void updateY(float y){this->centerY = y;}
	bool getAnimated(){return this->isAnimated;}
	bool getCaptured(){return this->isCaptured;}
	void setAnimated(bool animated){this->isAnimated = animated;}
	void setCapture(bool captured){this->isCaptured = captured;}
	void setSelected(bool selected){this->isSelected = selected;}
	bool getSelected(){return this->isSelected;}
};

class Board
{
	vector<Tile*> tiles;
	vector<Piece*> pieces;
	vector<Appearance*> whitePiecesApp;
	vector<Appearance*> blackPiecesApp;
	vector<Appearance*> whiteTilesApp;
	vector<Appearance*> blackTilesApp;
	Appearance* selApp;
	Appearance* plateApp;
	int currentWhitePiece;
	int currentBlackPiece;
	int currentWhiteTile;
	int currentBlackTile;

	int capturedWhitePieces;
	int capturedBlackPieces;

	Piece* selectedPiece;
	Tile* selectedTile;
	
	Primitive* plat;
	Primitive* square;
public:
	Board(vector<Appearance*>appe);
	void draw(float texS, float texT);

	void addTile(Tile* t){this->tiles.push_back(t);}
	void addPiece(Piece* p){this->pieces.push_back(p);}
	Piece* getPieceByName(int name);
	
	void addAppWhitePiece(Appearance* app){this->whitePiecesApp.push_back(app);}
	void addAppBlackPiece(Appearance* app){this->blackPiecesApp.push_back(app);}
	void addAppWhiteTile(Appearance* app){this->whiteTilesApp.push_back(app);}
	void addAppBlackTile(Appearance* app){this->blackTilesApp.push_back(app);}
	void setSelApp(Appearance* app){this->selApp = app;}
	void setPlateApp(Appearance* app){this->plateApp = app;}

	Appearance* getWhiteTileApp(int app){return this->whiteTilesApp.at(app);}
	Appearance* getBlackTileApp(int app){return this->blackTilesApp.at(app);}
	Appearance* getWhitePieceApp(int app){return this->whitePiecesApp.at(app);}
	Appearance* getBlackPieceApp(int app){return this->blackPiecesApp.at(app);}
	Appearance* getSelApp(){return this->selApp;}
	Appearance* getPlateApp(){return this->plateApp;}
	
	void setCurrentWhitePiece(int i){this->currentWhitePiece = i;}
	void setCurrentBlackPiece(int i){this->currentBlackPiece = i;}
	void setCurrentWhiteTile(int i){this->currentWhiteTile = i;}
	void setCurrentBlackTile(int i){this->currentBlackTile = i;}

	int getCurrentWhitePiece(){return this->currentWhitePiece;}
	int getCurrentBlackPiece(){return this->currentBlackPiece;}
	int getCurrentWhiteTile(){return this->currentWhiteTile;}
	int getCurrentBlackTile(){return this->currentBlackTile;}

	vector<Tile*> getTiles(){return this->tiles;}
	Tile* getTile(int x, int z);
	vector<Piece*> getPieces(){return this->pieces;}
	Piece* getPiece(int x, int z);

	void setSelectedPiece(Piece* p){
		clearPieceSelection();
		this->selectedPiece = p;
	}

	void clearPieceSelection();
	void clearTileSelection();

	void setSelectedTile(Tile* t){this->selectedTile = t;}
	Piece* getSelectedPiece(){return this->selectedPiece;}
	Tile* getSelectedTile(){return this->selectedTile;}

	string getCurrentBoardToString();

	void CapturePiece(Piece* p);
	void savePiece(bool player, int z, int x);
	void captureMiddlePiece(Piece* pi, int xi, int yi);
	int getCapWPieces(){return this->capturedWhitePieces;}
	int getCapBPieces(){return this->capturedBlackPieces;}
	void setCapWPieces(int i){this->capturedWhitePieces = i;}
	void setCapBPieces(int i){this->capturedBlackPieces = i;}
	void increaseCapWP(){this->capturedWhitePieces++;}
	void increaseCapBP(){this->capturedBlackPieces++;}
	void decreaseCapWP(){this->capturedWhitePieces--;}
	void decreaseCapBP(){this->capturedBlackPieces--;}

	void testVicNPMWW();/*TEST VICTORY CONDITON NO MORE POSSIBLE MOVES -> WHITE WIN*/
	void testVicNPMBW();/*TEST VICTORY CONDITON NO MORE POSSIBLE MOVES -> BLACK WIN*/
	void testVAPC();/*TEST VICTORY CONDITIONS ALL PIECES CAPTURED*/
};

class Room{
public:
	virtual void draw() = 0;
};

class LivingRoom : public Room{
	Table* table;
	Primitive* floor;
	Television* tv;
	Television* tv2;
	Chair* chair;
	Lamp* lamp;
	Tea* tea;
	Cabinet* cabinet;
	vector<Appearance*> wall;
	vector<Appearance*> carpet;
	int set;
public:
	LivingRoom(vector<Appearance*>app, int set);
	void draw();
};

#endif