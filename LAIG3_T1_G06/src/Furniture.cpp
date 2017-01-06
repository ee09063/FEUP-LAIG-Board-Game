#include "Furniture.h"

Leg :: Leg(vector<Appearance*> app)
{
	this->sph = new Sphere();
	vector<float>s;
	s.push_back(1.0);
	s.push_back(20);
	s.push_back(20);
	this->sph->setValues(s);

	this->cyl = new Cylinder();
	vector<float>c;
	c.push_back(0.8);
	c.push_back(0.6);
	c.push_back(6);
	c.push_back(20);
	c.push_back(5);
	this->cyl->setValues(c);

	for(unsigned int i = 0; i < app.size(); i++)
	{
		Appearance* a = app.at(i);
		if(strcmp("leg_1", a->getId())==0 || strcmp("leg_2", a->getId())==0)
			this->app.push_back(a);
	}
}

void Leg ::  draw(int set)
{
	float texS = this->app.at(set)->getTexture()->getTexS();
	float texT = this->app.at(set)->getTexture()->getTexT();
	this->app.at(set)->getApp()->apply();
	glPushMatrix();
		glTranslatef(1.0, this->cyl->getValues().at(2), 1.0);
		glRotatef(90, 1, 0, 0);
		this->cyl->draw(texS, texT);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(1.0, 7.0, 1.0);
		this->sph->draw(texS, texT);
	glPopMatrix();
}

Table :: Table(float length, float width, vector<Appearance*>app)
{
	this->unit = new Cube();
	this->leg = new Leg(app);
	this->length = length;
	this->width = width;
	for(unsigned int i = 0; i < app.size(); i++)
	{
		Appearance* a = app.at(i);
		if(strcmp("dark_marble", a->getId())==0 || strcmp("color_marble", a->getId())==0)
			this->table_top.push_back(a);
	}
}

void Table :: draw(int set){
		float len = this->length/2.0;
		float wid = this->width/2.0;
		glPushMatrix();
			glTranslatef(-len,-8,-wid);
			this->leg->draw(set);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-len,-8, wid-2);
			this->leg->draw(set);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(len-2,-8,-wid);
			this->leg->draw(set);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(len-2,-8, wid-2);
			this->leg->draw(set);
		glPopMatrix();
		glPushMatrix();//table top
			glTranslatef(-len, 0.0, -wid);
			glScalef(this->length, 2.0, this->width);
			this->table_top.at(set)->getApp()->apply();
			float texS = this->table_top.at(set)->getTexture()->getTexS();
			float texT = this->table_top.at(set)->getTexture()->getTexT();
			this->unit->draw(texS,texT);
		glPopMatrix();
}

Television :: Television(vector<Appearance*> app, int set)
{
	this->set = set;
	this->screen = new Rectangle();
	vector<float> rect;
	rect.push_back(0);
	rect.push_back(0);
	rect.push_back(21.375);
	rect.push_back(12.15);
	this->screen->setValues(rect);
	for(unsigned int i = 0; i < app.size(); i++)
	{
		Appearance* a = app.at(i);
		if(strcmp("television", a->getId())==0 || strcmp("television2", a->getId())==0)
			this->tv.push_back(a);
	}
}

void Television :: draw()
{
	glPushMatrix();
		float texS = this->tv.at(set)->getTexture()->getTexS();
		float texT = this->tv.at(set)->getTexture()->getTexT();
		this->tv.at(this->set)->getApp()->apply();
		this->screen->draw(texS, texT);
	glPopMatrix();
}

Chair :: Chair(vector<Appearance*> app)
{
	this->unit = new Cube();
	this->leg = new Leg(app);
	this->cyl = new Cylinder();
	vector<float>c;
	c.push_back(0.25);
	c.push_back(0.25);
	c.push_back(3);
	c.push_back(20);
	c.push_back(5);
	this->cyl->setValues(c);

	for(unsigned int i = 0; i < app.size(); i++)
	{
		Appearance* a = app.at(i);
		if(strcmp("chair_1", a->getId()) == 0 ||strcmp("chair_2", a->getId()) == 0)
			this->fabric.push_back(a);
	}
}

void Chair :: draw(int set)
{
	glPushMatrix();
		/*LEGS*/
		glScalef(0.75, 0.75, 0.75);
		glPushMatrix();
			glTranslatef(-4,-8,-6);
			this->leg->draw(set);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-4,-8, 2);
			this->leg->draw(set);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(3,0,3);
			glTranslatef(-1, -8, -1);
			this->leg->draw(set);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(3,0,-5);
			glTranslatef(-1, -8, -1);
			this->leg->draw(set);
		glPopMatrix();
		/*ARMREST 1*/
		glPushMatrix();
			glTranslatef(3.5,5,3);
			glRotatef(90, 1, 0, 0);
			this->cyl->draw(1.0, 1.0);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(3, 5, -4);
			glScalef(1, 1, 9);
			this->unit->draw(1.0, 1.0);
		glPopMatrix();
		/*ARMREST 2*/
		glPushMatrix();
			glTranslatef(-3.5,5,3);
			glRotatef(90, 1, 0, 0);
			this->cyl->draw(1.0, 1.0);
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-4, 5, -4);
			glScalef(1, 1, 9);
			this->unit->draw(1.0, 1.0);
		glPopMatrix();
		/*CHAIR SEAT*/
		glPushMatrix();//chair top
			glTranslatef(-4.0, 0.0, -4.0);
			glScalef(8.0, 2.0, 8.0);
			this->fabric.at(set)->getApp()->apply();
			float texS = this->fabric.at(set)->getTexture()->getTexS();
			float texT = this->fabric.at(set)->getTexture()->getTexT();
			this->unit->draw(texS,texT);
		glPopMatrix();
		/*CHAIR BACK*/
		glPushMatrix();
			glTranslatef(-4, 0, -4);
			glRotatef(90, 0, 1, 0);
			glScalef(2.0, 10.0, 8.0);
			this->unit->draw(texS,texT);
		glPopMatrix();
	glPopMatrix();
}

Lamp :: Lamp(vector<Appearance*>app)
{
	this->unit = new Cube();
	this->base = new Cylinder();
	this->pin = new Cylinder();
	this->ring = new Ring(1.2, 2.2, 1.5, 2.5, 2.0);
	vector<float> b;
	b.push_back(1.5);
	b.push_back(2);
	b.push_back(1);
	b.push_back(20);
	b.push_back(5);
	this->base->setValues(b);
	vector<float> p;
	p.push_back(0.25);
	p.push_back(0.25);
	p.push_back(2.5);
	p.push_back(20);
	p.push_back(5);
	this->pin->setValues(p);

	for(unsigned int i  = 0; i < app.size(); i++){
		if(strcmp("black_metal", app.at(i)->getId()) == 0)
			this->black = app.at(i);
		else if(strcmp("silver_metal", app.at(i)->getId()) == 0)
			this->metal = app.at(i);
	}
}

void Lamp :: draw(int set)
{
	/*BASE*/
	this->black->getApp()->apply();
	glPushMatrix();
		glTranslatef(0, 1, 0);
		glRotatef(90, 1, 0, 0);
		this->base->draw(1.0, 1.0);
	glPopMatrix();
	/*SUPPORTS 1 and 2*/
	glPushMatrix();
		glTranslatef(0.25, 1, -0.5);	
		glScalef(0.5, 15, 1);
		this->unit->draw(1.0, 1.0);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(-0.75, 1, -0.5);
		glScalef(0.5, 15, 1);
		this->unit->draw(1.0, 1.0);
	glPopMatrix();
	/*MAIN SUPPORT*/
	glPushMatrix();
		glTranslatef(-0.25, 14, -1.5);
		glRotatef(-30, 1, 0, 0);
		glScalef(0.5, 1.0, 10);
		this->unit->draw(1.0, 1.0);
	glPopMatrix();
	/*HEAD*/
	glPushMatrix();
		glTranslatef(0, 19, 6.5);
		glRotatef(60, 1, 0, 0);
		this->ring->draw(1.0, 1.0);
	glPopMatrix();
	/*PIN*/
	this->metal->getApp()->apply();
	glPushMatrix();
		glTranslatef(-1.25, 15.5, 0);
		glRotatef(90, 0, 1, 0);
		this->pin->draw(1.0, 1.0);
	glPopMatrix();
}

Tea :: Tea(vector<Appearance*>app)
{
	this->table = new Table(10.0, 10.0, app);
	this->cup = new Ring(0.5, 0.5, 0.7, 0.7,2.0);
	this->bot = gluNewQuadric();
	gluQuadricDrawStyle(this->bot, GLU_FILL);
	gluQuadricNormals(this->bot, GLU_SMOOTH);
	gluQuadricOrientation(this->bot, GLU_OUTSIDE);
	for(unsigned int i  = 0; i < app.size(); i++){
		if(strcmp("black_metal", app.at(i)->getId()) == 0 || strcmp("silver_metal", app.at(i)->getId()) == 0)
			this->apps.push_back(app.at(i));
	}
}

void Tea :: draw(int set)
{
	this->apps.at(set)->getApp()->apply();
	glPushMatrix();
		
		glRotatef(-90, 1, 0, 0);
		glTranslatef(-3, -3, 2);
		this->cup->draw(1.0, 1.0);	
		glTranslatef(0,0,0.1);
		gluDisk(this->bot, 0.0, 0.7, 20, 20);
	glPopMatrix();
	glPushMatrix();
		glRotatef(-90, 1, 0, 0);
		glTranslatef(-3, 3, 2);
		this->cup->draw(1.0, 1.0);
		glTranslatef(0,0,0.1);
		gluDisk(this->bot, 0.0, 0.7, 20, 20);
	glPopMatrix();
	glPushMatrix();
		glTranslatef(2.0, 3.75, 2.0);
		glRotatef(180, 0, 1, 0);
		glFrontFace(GL_CW);
		glutSolidTeapot(2.0);
		glFrontFace(GL_CCW);
	glPopMatrix();
	glPushMatrix();
		this->table->draw(set);
	glPopMatrix();
}

Cabinet :: Cabinet(vector<Appearance*>app)
{
	this->unit = new Cube();
	this->face = new Rectangle();
	vector<float>v;
	v.push_back(0);
	v.push_back(0);
	v.push_back(20);
	v.push_back(12);
	face->setValues(v);
	for(unsigned int i = 0; i < app.size(); i++){
		Appearance* a = app.at(i);
		if(strcmp("ornate_panel",a->getId())==0)
			this->front = a;
		else if(strcmp("ornate_rest",a->getId())==0)
			this->rest = a;
	}
}

void Cabinet :: draw(int set)
{
	glPushMatrix();
	glTranslatef(0,12,0);
	glRotatef(90, 1, 0, 0);
	glPushMatrix();
		glTranslatef(0, 12.1, 12);
		this->front->getApp()->apply();
		this->face->draw(20, 12);
	glPopMatrix();
	glPushMatrix();
		this->rest->getApp()->apply();
		glScalef(20, 12, 12);
		this->unit->draw(1.0, 1.0);
	glPopMatrix();
	glPopMatrix();
}