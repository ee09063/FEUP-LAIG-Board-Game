#include "Geometry.h"

Tile :: Tile(char* type, float centerX, float centerY, float centerZ)
{
	vector<float> v;
	v.push_back(-0.5);
	v.push_back(-0.5);
	v.push_back(0.5);
	v.push_back(0.5);
	r = new Rectangle();
	r->setValues(v);
	this->centerX = centerX;
	this->centerY = centerY;
	this->centerZ = centerZ;
	this->type = type;
	this->isSelected = false;
}

void Tile :: draw(float texS, float texT)
{
	glPushMatrix();
	glTranslatef(getCenterX(), getCenterY(), getCenterZ());
	this->r->draw(texS, texT);
	glPopMatrix();
}

Piece :: Piece(char* type, float centerX, float centerY, float centerZ)
{
	vector<float> v;
	v.push_back(0.35);
	v.push_back(0.35);
	v.push_back(0.2);
	v.push_back(SIDES);
	v.push_back(5);
	this->c = new Cylinder();
	this->c->setValues(v);
		
	vector<float>f;
	f.push_back(0.15);
	f.push_back(SIDES);
	f.push_back(SIDES);
	this->bearing = new Sphere();
	this->bearing->setValues(f);
		
	this->thor = new Torus();
	vector<float>t;
	t.push_back(0.14);
	t.push_back(0.2);
	t.push_back(SIDES);
	t.push_back(SIDES);
	this->thor->setValues(t);
		
	this->ring = new Ring(0.35, 0.35, 0.4, 0.4, 0.25);
		
	this->centerX = centerX;
	this->centerY = centerY;
	this->centerZ = centerZ;
	this->type = type;
	this->isAnimated = false;
	this->isCaptured = false;
	this->finalX = 0;
	this->finalZ = 0;
	this->time = 0;
	this->isSelected = false;
}

void Piece :: draw(float texS, float texT, Appearance* red)
{
	glPushMatrix();
		glTranslatef(getCenterX(), getCenterY(), getCenterZ());
		glRotatef(-90, 1, 0, 0);
		if(this->isSelected)
			red->getApp()->apply();
		this->c->draw(texS, texT);
		glPushMatrix();
			glTranslatef(0.0, 0.0, 0.1);
			this->bearing->draw(texS, texT);
			this->thor->draw(texS, texT);
		glPopMatrix();
			this->ring->draw(texS, texT);
	glPopMatrix();
}

Board :: Board(vector<Appearance*>appe)
{
	bool white = true;
	this->currentWhitePiece = 0;
	this->currentBlackPiece = 0;
	this->currentWhiteTile = 0;
	this->currentBlackTile = 0;
	this->capturedBlackPieces = 0;
	this->capturedWhitePieces = 0;
	/**/
	this->plat = new Cube();
	this->square = new Rectangle();
	vector<float> r;
	r.push_back(0); r.push_back(0); r.push_back(4); r.push_back(4);
	this->square->setValues(r);
	/*INITIATE TILES*/
	for(int z = 0; z < 8; z++){ // rows
		for(int x = 0; x < 8; x++){ //columns
			Tile* t = NULL;
			if(white){
				t = new Tile("white", x, 0, z);
				addTile(t);
			}
			else if(!white){
				t = new Tile("black", x, 0, z);
				addTile(t);
			}
			white = !white;
		}
		white = !white;
	}
	
	//testVicNPMWW();/*TEST VICTORY CONDITON NO MORE POSSIBLE MOVES -> WHITE WIN*/
	//testVicNPMBW();/*TEST VICTORY CONDITON NO MORE POSSIBLE MOVES -> BLACK WIN*/
	//testVAPC();/*TEST VICTORY CONDITIONS ALL PIECES CAPTURED*/
	
	/*NOW FOR THE PIECES*/
	for(int z = 0; z < 8; z++){
		for(int x = 0; x < 8; x++){
			Piece* p = NULL;
			if((z == 0 || z == 1) && (x >= 1 && x <= 6)){
				p = new Piece("white", x, 0, z);
				addPiece(p);
			}
			else if(z == 2 && (x == 1 || x ==2 || x==5 || x==6)){
					p = new Piece("white", x, 0, z);
					addPiece(p);
			}
			else if((z == 6 || z == 7) && (x >= 1 && x <= 6)){
				p = new Piece("black", x, 0, z);
				addPiece(p);
			}
			else if(z == 5 && (x == 1 || x ==2 || x==5 || x==6)){
					p = new Piece("black", x, 0, z);
					addPiece(p);
			}
		}	
	}
	/*NOW FOR THE APPEARANCES*/
	for(unsigned int i = 0; i < appe.size(); i++)
	{
		Appearance* app = appe.at(i);
		if(strcmp("red", app->getId())==0){
			setSelApp(app);
		}
		else if(strcmp("plates", app->getId())==0){
			setPlateApp(app);
		}
		char* type = app->getType();
		if(strcmp("wp", type) == 0){
			addAppWhitePiece(app);
		}
		else if(strcmp("bp", type) == 0){
			addAppBlackPiece(app);
		}
		else if(strcmp("wt", type) == 0){
			addAppWhiteTile(app);
		}
		else if(strcmp("bt", type) == 0){
			addAppBlackTile(app);
		}
	}
}

void Board :: draw(float texS, float texT)
{
	glPushMatrix();
	
	glTranslatef(18, 10.6, 16);
	/*AREAS FOR CAPTURED PIECES*/
	glPushMatrix();
		glTranslatef(9.0, 0.0, 4.0);
		float texS2 = this->plateApp->getTexture()->getTexS(); 
		float texT2 = this->plateApp->getTexture()->getTexT();  
		this->plateApp->getApp()->apply();
		this->square->draw(texS2, texT2);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(9.0, 0.0, 8.0);
		this->square->draw(texS2, texT2);
	glPopMatrix();
	/*BOARD BASE*/
	glPushMatrix();
		glTranslatef(0.0, -0.4, 0.0);
		glScalef(8.0, 0.4, 8.0);
		getWhitePieceApp(this->currentWhitePiece)->getApp()->apply();
		this->plat->draw(0.0, 1.0);
	glPopMatrix();
	glTranslatef(0.5, 0.0, 0.5);
	glPushName(-1);
	/*DRAW THE PIECES*/
	for(unsigned int i = 0; i < getPieces().size(); i++)
	{
		glPushMatrix();
		Piece* p = this->getPieces().at(i);
		if(strcmp("white",p->getType())==0){
			getWhitePieceApp(this->currentWhitePiece)->getApp()->apply();
		}
		else if(strcmp("black", p->getType())==0){
			getBlackPieceApp(this->currentBlackPiece)->getApp()->apply();
		}
		if(p->getCaptured() == false){
			p->setName(i);
			glLoadName(i);
		}
		this->getPieces().at(i)->draw(texS, texT, this->selApp);
		glPopMatrix();
	}
	/*DRAW THE TILES*/
	int tile_count = 0;
	for(int z = 0; z < NUM_ROWS; z++){
		glPushMatrix();
		glLoadName(z);
		for(int x = 0; x < NUM_ROWS; x++){
			glPushMatrix();
			glPushName(x);
			Tile* t = getTiles().at(tile_count);
			if(strcmp("white", t->getType())==0){
				getWhiteTileApp(this->currentWhiteTile)->getApp()->apply();
			}
			else if(strcmp("black", t->getType())==0){
				getBlackTileApp(this->currentBlackTile)->getApp()->apply();
			}
			t->draw(texS, texT);
			glPopName();
			glPopMatrix();
			tile_count++;
		}
		glPopMatrix();
	}
	glPopMatrix();
}

Tile* Board :: getTile(int x, int z)
{
	for(unsigned int i = 0; i < this->pieces.size(); i++)
	{
		Tile* tl = this->tiles.at(i);
		if(tl->getCenterX() == x && tl->getCenterZ() == z)
		{
			return tl;
		}
	}
	return NULL;
}

Piece* Board :: getPiece(int x, int z)
{
	for(unsigned int i = 0; i < this->pieces.size(); i++)
	{
		Piece* p = this->pieces.at(i);
		if(p->getCenterX() == x && p->getCenterZ() == z)
		{	
			return p;
		}
	}
	return NULL;
}

Piece* Board :: getPieceByName(int name)
{
	for(unsigned int i = 0; i < this->pieces.size(); i++)
	{
		if(this->pieces.at(i)->getName() == name)
		{
			return this->pieces.at(i);
		}
	}
	return NULL;
}

string Board :: getCurrentBoardToString()
{
	int p[8][8];

	for(int z = 0; z < 8; z++){
		for(int x = 0; x < 8; x++){
			Piece* pi = this->getPiece(x, z);
			if(NULL != pi && strcmp("white", pi->getType())==0)
				p[z][x] = 1;
			else if(NULL != pi && strcmp("black", pi->getType())==0)
				p[z][x] = 2;
			else if(NULL == pi)
				p[z][x] = 0;
		}
	}

	string b = "";
	b += "[";
	for(int i = 0; i < NUM_ROWS; i++){
		b += "[";
		for(int k = 0; k < NUM_ROWS; k++){
			int j = p[i][k];
			b += SSTR(j);
			if(k != 7)
				b += ",";
		}
		b+="]";
		if(i != 7)
			b+=",";
	}
	b+="]-" + SSTR(getCapBPieces()) + "-" + SSTR(getCapWPieces());
	return b;

}

void Board :: CapturePiece(Piece* p)
{
	if(p->getSelected())
		p->setSelected(false);
	p->setCapture(true);
	if(strcmp("white", p->getType()) == 0){
		increaseCapWP();
		for(int z = 7; z >= 4; z--){
			for(int x = 9; x <= 12; x++){
				if(NULL == this->getPiece(x,z)){
					p->updateInitialPosition(x,z);
					return;
				}
			}
		}
	}
	else if(strcmp("black", p->getType()) == 0){
		increaseCapBP();
		for(int z = 0; z <= 3; z++){
			for(int x = 9; x <= 12; x++){
				if(NULL == this->getPiece(x,z)){
					p->updateInitialPosition(x,z);
					return;
				}
			}
		}
	}
}

void Board :: savePiece(bool player, int zi, int xi)
{
	if(player){
		decreaseCapWP();
		for(int z = 7; z >= 4; z--){
			for(int x = 9; x <= 12; x++){
				Piece* p = this->getPiece(x,z);
				if(NULL != p){
					p->setCapture(false);
					p->updateInitialPosition(xi,zi);
					return;
				}
			}
		}
	}
	else if(!player){
		decreaseCapBP();
		for(int z = 0; z <= 3; z++){
			for(int x = 9; x <= 12; x++){
				Piece* p = this->getPiece(x,z);
				if(NULL != p){
					p->setCapture(false);
					p->updateInitialPosition(xi,zi);
					return;
				}
			}
		}
	}
}

void Board :: captureMiddlePiece(Piece* pi, int xi, int yi)
{
	int xf = pi->getCenterZ();
	int yf = pi->getCenterX();

	Piece* p = NULL;

	/*UP*/
	if(yf == yi){
		if(strcmp("white", pi->getType())==0){
			p = getPiece(yi, xi+1);
		}
		else if(strcmp("black", pi->getType())==0){
			p = getPiece(yi, xi-1);
		}
	}
	/*SIDES*/
	if(xf == xi){
		if(yf < yi)//LEFT
			p = getPiece(yi-1,xi);

		else if(yf > yi)//RIGHT
			p = getPiece(yi + 1, xi);
	}
	/*DIAGONAL*/
	if(strcmp("white", pi->getType())==0){
		if(xf > xi && yf < yi){//DIAGONAL LEFT
			p = getPiece(yi-1, xi+1);
		}
		if(xf > xi && yf > yi){//DIAGONAL RIGHT
			p = getPiece(yi+1, xi+1);
		}
	}
	else if(strcmp("black", pi->getType())==0){
		if(xf < xi && yf < yi){//DIAGONAL LEFT
			p = getPiece(yi-1, xi-1);
		}
		if(xf < xi && yf > yi){//DIAGONAL RIGHT
			p = getPiece(yi+1, xi-1);
		}
	}
	if(NULL != p)
		CapturePiece(p);
	else printf("COULD NOT FIND MIDDLE PIECE\n");
}

void Board :: clearPieceSelection()
{
	for(unsigned int i = 0; i < this->pieces.size(); i++){
		if(this->pieces.at(i)->getSelected()){
			this->pieces.at(i)->setSelected(false);
			break;
		}
	}
}

void Board :: clearTileSelection()
{
	for(unsigned int i = 0; i < this->tiles.size(); i++){
		if(this->tiles.at(i)->getSelected()){
			this->tiles.at(i)->setSelected(false);
			break;
		}
	}
}

void Board :: testVicNPMWW()
{
	/*TEST VICTORY CONDITON NO MORE POSSIBLE MOVES -> WHITE WIN*/
	Piece *p = NULL;
	p = new Piece("black", 3, 0, 5);
	addPiece(p);
	p = new Piece("white", 1, 0, 3);
	addPiece(p);
	p = new Piece("white", 3, 0, 3);
	addPiece(p);
	p = new Piece("white", 5, 0, 3);
	addPiece(p);
	p = new Piece("white", 2, 0, 4);
	addPiece(p);
	p = new Piece("white", 3, 0, 4);
	addPiece(p);
	p = new Piece("white", 4, 0, 4);
	addPiece(p);
	p = new Piece("white", 1, 0, 1);
	addPiece(p);
	this->capturedBlackPieces = 15;
	this->capturedWhitePieces = 9;
}

void Board :: testVicNPMBW()
{
	/*TEST VICTORY CONDITON NO MORE POSSIBLE MOVES -> BLACK WIN*/
	Piece *p = NULL;
	p = new Piece("white", 3, 0, 1);
	addPiece(p);
	p = new Piece("black", 2, 0, 3);
	addPiece(p);
	p = new Piece("black", 3, 0, 3);
	addPiece(p);
	p = new Piece("black", 4, 0, 3);
	addPiece(p);
	p = new Piece("black", 1, 0, 4);
	addPiece(p);
	p = new Piece("black", 3, 0, 4);
	addPiece(p);
	p = new Piece("black", 5, 0, 4);
	addPiece(p);
	p = new Piece("black", 7, 0, 7);
	addPiece(p);
	this->capturedBlackPieces = 15;
	this->capturedWhitePieces = 9;
}

void Board :: testVAPC()
{
	/*TEST VICTORY CONDITIONS ALL PIECES CAPTURED*/
	Piece *p = NULL;
	p = new Piece("white", 3, 0, 3);
	addPiece(p);
	p = new Piece("black", 4, 0, 4);
	addPiece(p);
	this->capturedBlackPieces = 15;
	this->capturedWhitePieces = 15;
}

LivingRoom :: LivingRoom(vector<Appearance*>app, int set)
{
	this->set = set;
	this->table = new Table(24, 12, app);
	this->chair = new Chair(app);
	this->lamp = new Lamp(app);
	this->tea = new Tea(app);
	this->cabinet = new Cabinet(app);
	this->tv = new Television(app, 0);
	this->tv2 = new Television(app, 1);
	this->floor = new Rectangle();
	vector<float>v;
	v.push_back(0);
	v.push_back(0);
	v.push_back(1);
	v.push_back(1);
	this->floor->setValues(v);
	for(unsigned int i = 0; i < app.size(); i++)
	{
		Appearance* a = app.at(i);
		if(strcmp("red_carpet", a->getId()) == 0 || strcmp("blue_carpet", a->getId()) == 0){
			this->carpet.push_back(a);
		}
		else if(strcmp("wall_panel", a->getId()) == 0 || strcmp("wall_wood", a->getId()) == 0){
			this->wall.push_back(a);
		}
	}
}

void LivingRoom :: draw(){
	/*CABINETS*/
	glPushMatrix();
		glTranslatef(60, 0, 0);
		glRotatef(-90, 0, 1, 0);
		this->cabinet->draw(0);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(60, 0, 20);
		glRotatef(-90, 0, 1, 0);
		this->cabinet->draw(0);
	glPopMatrix();
	/*TEA SET*/
	glPushMatrix();
		glTranslatef(40, 5.6, 20);
		glScalef(0.7, 0.7, 0.7);	
		float t;
		if(this->set == 0) t = 1;
		else t = 0;
		this->tea->draw(t);
	glPopMatrix();
	/*LAMP NEAR TABLE*/
	glPushMatrix();
		glTranslatef(8.5, 0, 20);
		glRotatef(90, 0, 1, 0);
		this->lamp->draw(this->set);
	glPopMatrix();
	/*LAMP NEAR CABINETS*/
	glPushMatrix();
		glTranslatef(55, 0, 45);
		glRotatef(180, 0, 1, 0);
		this->lamp->draw(this->set);
	glPopMatrix();
	/*CHAIRS AWAY FROM TABLE*/
	glPushMatrix();
		glTranslatef(6.5, 6, 45);
		glRotatef(90, 0, 1, 0);
		this->chair->draw(this->set);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(6.5, 6, 54);
		glRotatef(90, 0, 1, 0);
		this->chair->draw(this->set);
	glPopMatrix();
	/*CHAIRS NEAR TABLE*/
	glPushMatrix();
		glTranslatef(22, 6, 8);
		this->chair->draw(this->set);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(22, 6, 32);
		glRotatef(180, 0, 1, 0);
		this->chair->draw(this->set);
	glPopMatrix();
	/*PAINTINGS*/
	glPushMatrix();
		glTranslatef(0.1, 8.925, 40.6875);
		glRotatef(90, 0, 1, 0);
		glRotatef(90, 1, 0, 0);
		this->tv->draw();
	glPopMatrix();
	glPushMatrix();
		glTranslatef(19.3125, 8.925, 0.1);
		glRotatef(90, 1, 0, 0);
		this->tv2->draw();
	glPopMatrix();
	/*FLOOR*/
	glPushMatrix();
		this->carpet.at(this->set)->getApp()->apply();
		float textS = this->carpet.at(this->set)->getTexture()->getTexS();
		float textT = this->carpet.at(this->set)->getTexture()->getTexT();
		glTranslatef(0.0, 0.0, 60.0);
		glScalef(60.0, 1.0, 60.0);
		this->floor->draw(textS, textT);//floor
	glPopMatrix();
	/*WALLS*/
	glPushMatrix();
		this->wall.at(this->set)->getApp()->apply();
		float texS = this->wall.at(this->set)->getTexture()->getTexS();
		float texT = this->wall.at(this->set)->getTexture()->getTexT();
		glTranslatef(0.0, 0.0, 60.0);
		glRotatef(90, 1, 0, 0);
		glRotatef(-90, 0, 0, 1);
		glScalef(60.0, 1.0, 30.0);
		this->floor->draw(texS, texT);//left wall
	glPopMatrix();
	glPushMatrix();
		this->wall.at(this->set)->getApp()->apply();
		glRotatef(90.0, 1, 0, 0);
		glScalef(60.0, 1.0, 30.0);
		this->floor->draw(texS, texT);//right wall
	glPopMatrix();
	/*TABLE*/
	glPushMatrix();
		glTranslatef(22.0, 8.0, 20.0);
		this->table->draw(this->set);
	glPopMatrix();
}


