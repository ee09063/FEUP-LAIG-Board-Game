#include "SceneGraph.h"
#include<numeric>
#include<string>

#define PI 3.14159265358979323846

void Rectangle::draw(float texS, float texT)
{
	float x1, x2, y1, y2;
	x1 = v.at(0); x2 = v.at(2);
	y1 = v.at(1); y2 = v.at(3);

	float _xDim = abs(x2 - x1);
	float _yDim = abs(y2 - y1);

	glRotatef(-90, 1, 0, 0);

	glBegin(GL_POLYGON);
		glTexCoord2f(0,0);
		glNormal3f(0,0,1);
		glVertex2f(x1, y1);

		glTexCoord2f(_xDim/texS,0);
		glNormal3f(0,0,1);
		glVertex2f(x2, y1);

		glTexCoord2f(_xDim/texS, _yDim/texT);
		glNormal3f(0,0,1);
		glVertex2f(x2, y2);

		glTexCoord2f(0, _yDim/texT);
		glNormal3f(0,0,1);
		glVertex2f(x1, y2);
	glEnd();
}

void Cube::draw(float texS, float texT){
	if(texS != 0){
		glPushMatrix();
			glTranslatef(0.0, 1.0, 1.0);
			this->face->draw(1.0, 1.0);//top
		glPopMatrix();
	}
	glPushMatrix();
		glTranslatef(0.0, 0.0, 0.0);
		glRotatef(180.0, 1, 0, 0);
		this->face->draw(1.0, 1.0);//bottom
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0.0, 0.0, 1.0);
		glRotatef(90.0, 1, 0, 0);
		this->face->draw(1.0, 1.0);//front
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0.0, 1.0, 0.0);
		glRotatef(-90.0, 1, 0, 0);
		this->face->draw(1.0, 1.0);//front
	glPopMatrix();
	glPushMatrix();
		glTranslatef(0.0, 0.0, 1.0);
		glRotatef(90.0, 0, 0, 1);
		this->face->draw(1.0, 1.0);//left
	glPopMatrix();
	glPushMatrix();
		glTranslatef(1.0, 0.0, 1.0);
		glRotatef(-90.0, 0, 0, 1);
		glTranslatef(-1.0, 0.0, 0.0);
		this->face->draw(1.0, 1.0);//right
	glPopMatrix();
}

void Cylinder::draw(float texS, float texT)
{
	GLUquadric* quad = gluNewQuadric();
	gluQuadricDrawStyle(quad, GLU_FILL);
	gluQuadricNormals(quad, GLU_SMOOTH);
	gluQuadricTexture(quad, GL_TRUE);
	/*BASE*/
	glPushMatrix();
	glRotatef(180.0f, 1, 0, 0);
	gluDisk(quad, 0, base, this->slices, this->stacks);
	glPopMatrix();
	/*TOP*/
	glPushMatrix();
	glTranslatef(0.0, 0.0, this->height);
	gluDisk(quad, 0, top, this->slices, this->stacks);
	glPopMatrix();
	/*CYLINDER*/
	gluCylinder(quad, this->base, this->top, this->height, this->slices, this->stacks);
}

void Sphere::draw(float texS, float texT)
{
	glPushMatrix();
	//	glRotatef(-90, 1, 0, 0);
		GLUquadric* quad = gluNewQuadric();
		gluQuadricDrawStyle(quad, GLU_FILL);
		gluQuadricNormals(quad, GLU_SMOOTH);
		gluQuadricTexture(quad, GL_TRUE);
		gluSphere(quad, this->radius, this->slices, this->stacks);
	glPopMatrix();
}

void Torus :: draw(float texS, float texT)
{
 	//glutSolidTorus(this->inner, this->outer, this->slices, this->loops);
	int numt = (int)this->loops;
	int numc = (int)this->slices;

	int i, j, k;
	double s, t, x, y, z,x_norm,y_norm,z_norm,norm, twopi;
 
	twopi = 2 * PI;
	for (i = 0; i < numc; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (j = 0; j <= numt*1; j++)
		{
			x_norm=(outer+inner*cos((i+1)*twopi/numc))*cos((j+0.5)*twopi/numt)-outer*cos((j+0.5)*twopi/numt);
			y_norm=(outer+inner*cos((i+1)*twopi/numc))*sin((j+0.5)*twopi/numt)-outer*sin((j+0.5)*twopi/numt);
			z_norm=inner * sin((i+1)*twopi / numc);
			norm = sqrt(x_norm*x_norm+y_norm*y_norm+z_norm*z_norm);
			x_norm=x_norm/norm;
			y_norm=y_norm/norm;
			z_norm=z_norm/norm;
 
			glNormal3f(x_norm,y_norm,z_norm);
 
			for (k = 1; k >= 0; k--)
			{
				s = (i + k) % numc + 0.5;
				t = j % numt;
 
				x =  (outer+inner*cos(s*twopi/numc))*cos(t*twopi/numt);
				y =  (outer+inner*cos(s*twopi/numc))*sin(t*twopi/numt);
				z =  (inner * sin(s * twopi / numc));

				float texs = (i + k) / (float)numc;
				float text = (j % (numt+1)) / (float)numt;
				
				glTexCoord2f(text,texs);
				glVertex3f(x, y, z);
			}
		}
		glEnd();
	}
}

void Ring :: draw(float texS, float texT)
{
	glPushMatrix();
		gluCylinder(inner, this->innerBase, this->innerTop, this->height, 20, 20);
		gluCylinder(outer, this->outerBase, this->outerTop, this->height, 20, 20);
		gluDisk(inner, this->innerBase, this->outerBase, 20, 20);
		glTranslatef(0.0, 0.0, this->height);
		gluDisk(outer, this->innerTop, this->outerTop, 20, 20);
	glPopMatrix();
}

Texture* InitVar::getTexById(char* id)
{
	for(unsigned int i = 0; i < this->textures.size(); i++)
	{
		if(strcmp(id, this->textures.at(i)->getId())==0)
			return this->textures.at(i);
	}
	return NULL;
}

Appearance* InitVar::getAppById(char* id)
{
	for(unsigned int i = 0; i < this->appearances.size(); i++)
	{
		if(strcmp(id, this->appearances.at(i)->getId())==0)
			return this->appearances.at(i);
	}
	return NULL;
}

int InitVar::getCameraById(char* id)
{
	unsigned int i;
	for(i=0; i < this->cameras.size(); i++)
		if(strcmp(id, this->cameras.at(i)->getId()) == 0)
			return i;
	return -1;
}

void CamPerspective::setView()
{
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	int width = glutGet(GLUT_WINDOW_WIDTH);
	float aspect =  (float)width / (float)height;
	gluPerspective(angle, aspect, near, far);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(posx, posy, posz,
				targx, targy, targz,
				0.0, 1.0, 0.0);
}

void CamOrtho::setView()
{
	int height = glutGet(GLUT_WINDOW_HEIGHT);
	int width = glutGet(GLUT_WINDOW_WIDTH);
	float aspect =  (float)width / (float)height;

	glOrtho(left, right, bottom, top, near, far);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if(direction == 0){
		glRotatef(-90, 0 , 1, 0); //x
	}
	else if(direction == 1){
		glRotatef(90, 1, 0, 0); //y
	}
}