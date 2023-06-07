#include "helicopter.h"
#include "shader-loader.h"

extern int renderFillEnabled;

void drawSpotlightCone(Helicopter* heli);

void initHelicopter(Helicopter* heli)
{
	heli->quadric = gluNewQuadric();
	heli->size = 10.0;
	heli->scaleFactor = SCALE;

	heli->shaderProgramID = setShaders("shaders/heliShader.vert", "shaders/heliShader.frag");

	heli->coordinates.x = (TERRAIN_GRID_SIZE * TERRAIN_GRID_LEGNTH / 2) * heli->scaleFactor;
	heli->coordinates.z = (TERRAIN_GRID_SIZE * TERRAIN_GRID_LEGNTH / 2) * heli->scaleFactor;
	heli->coordinates.y = 25 * heli->scaleFactor;

	heli->collisionBox.x = (heli->size * heli->scaleFactor) / 2.0;
	heli->collisionBox.y = (heli->size * heli->scaleFactor) * 1.3;
	heli->collisionBox.z = (heli->size * heli->scaleFactor);

	heli->yaw = 0;
	heli->rotorRotation = 0.0;
	heli->rotorVelocity = 25.0;

	heli->direction = 0;
	heli->velocity = 0.0;
	heli->strafeVelocity = 0.0;
	heli->liftVelocity = 0.0;

	heli->maxSpeed = heli->size * heli->scaleFactor * 50.0;

	heli->spotlightOn = 1;
}

void drawTopRotor(Helicopter* heli)
{
	glPushMatrix();

	// Rotor spin!
	glRotated(heli->rotorRotation, 0.0, 1.0, 0.0);

	glTranslated(0.0, heli->size / 2.0, 0.0);
	glRotated(-90.0, 1.0, 0.0, 0.0);
	GLdouble rotorHeight = heli->size / 4.0;
	gluCylinder(heli->quadric, heli->size / 10.0, heli->size / 10.0, rotorHeight, 32, 32);
	glRotated(90.0, 1.0, 0.0, 0.0);
	glTranslated(0.0, rotorHeight, 0.0);

	int numBlades = 7;
	for (int i = 0; i < numBlades; i++)
	{
		gluCylinder(heli->quadric, heli->size / 20.0, heli->size / 30.0, heli->size, 4, 1);
		glRotated(360.0/ numBlades, 0.0, 1.0, 0.0);
	}

	glPopMatrix();
}

void drawTailRotor(Helicopter* heli)
{
	glPushMatrix();

	glTranslated(0.0, -heli->size * 0.1, -heli->size * 0.9);
	glRotated(180.0, 0.0, 1.0, 0.0);
	glRotated(-15.0, 1.0, 0.0, 0.0);

	GLdouble rotorHeight = heli->size * 0.9;
	gluCylinder(heli->quadric, heli->size / 10.0, heli->size / 15.0, rotorHeight, 32, 32);

	glTranslated(heli->size * 0.1, 0.0, heli->size * 0.9);
	
	// Rotor spin!
	glRotated(heli->rotorRotation, 1.0, 0.0, 0.0);
	int numBlades = 4;
	for (int i = 0; i < numBlades; i++)
	{
		gluCylinder(heli->quadric, heli->size / 20.0, heli->size / 30.0, heli->size * 0.4, 4, 1);
		glRotated(360.0 / numBlades, 1.0, 0.0, 0.0);
	}

	glPopMatrix();
}

void drawFeet(Helicopter* heli)
{
	// Front right leg
	glPushMatrix();
	glTranslated(heli->size * 0.25, -heli->size * 0.3, heli->size * 0.6);
	glRotated(70.0, 0.75, 0.5, 0.0);
	gluCylinder(heli->quadric, heli->size / 20.0, heli->size / 20.0, heli->size * 0.4, 32, 32);
	glPopMatrix();

	// Front left leg
	glPushMatrix();
	glTranslated(-heli->size * 0.25, -heli->size * 0.3, heli->size * 0.6);
	glRotated(70.0, 0.75, -0.5, 0.0);
	gluCylinder(heli->quadric, heli->size / 20.0, heli->size / 20.0, heli->size * 0.4, 32, 32);
	glPopMatrix();

	// Back left leg
	glPushMatrix();
	glTranslated(-heli->size * 0.25, -heli->size * 0.3, -heli->size * 0.6);
	glRotated(70.0, 0.75, -0.5, 0.0);
	gluCylinder(heli->quadric, heli->size / 20.0, heli->size / 20.0, heli->size * 0.4, 32, 32);
	glPopMatrix();

	// Back right leg
	glPushMatrix();
	glTranslated(heli->size * 0.25, -heli->size * 0.3, -heli->size * 0.6);
	glRotated(70.0, 0.75, 0.5, 0.0);
	gluCylinder(heli->quadric, heli->size / 20.0, heli->size / 20.0, heli->size * 0.4, 32, 32);
	glPopMatrix();

	// Left base
	glPushMatrix();
	glTranslated(heli->size * 0.25 + heli->size * 0.20, -heli->size * 0.3 - heli->size * 0.3, heli->size * 0.6 + heli->size * 0.4);
	glRotated(180.0, 0.0, 1.0, 0.0);
	gluCylinder(heli->quadric, heli->size / 20.0, heli->size / 20.0, heli->size *2.0, 32, 32);
	glPopMatrix();

	// Right base
	glPushMatrix();
	glTranslated(-heli->size * 0.25 + -heli->size * 0.20, -heli->size * 0.3 - heli->size * 0.3, heli->size * 0.6 + heli->size * 0.4);
	glRotated(180.0, 0.0, 1.0, 0.0);
	gluCylinder(heli->quadric, heli->size / 20.0, heli->size / 20.0, heli->size * 2.0, 32, 32);
	glPopMatrix();
}

void drawBody(Helicopter* heli)
{
	glPushMatrix();

	glRotated(270.0, 1.0, 0.0, 0.0);

	glRotated(90.0, 0.0, 0.0, 1.0);

	glScaled(1.0, 0.5, 0.5);
	gluSphere(heli->quadric, heli->size, 32, 32);
	glPopMatrix();
}

void drawHelicopter(Helicopter* heli)
{
	glPushMatrix();

	glTranslated(heli->coordinates.x, heli->coordinates.y, heli->coordinates.z);
	glScaled(heli->scaleFactor, heli->scaleFactor, heli->scaleFactor);

	// Yaw
	glRotated(-heli->yaw, 0.0, 1.0, 0.0);

	// Pitch
	glRotated(heli->pitch, 1.0, 0.0, 0.0);

	// Roll
	glRotated(heli->roll, 0.0, 0.0, 1.0);

	glUseProgram(heli->shaderProgramID);
	drawBody(heli);
	drawFeet(heli);
	drawTopRotor(heli);
	drawTailRotor(heli);
	glUseProgram(0);

	if (heli->spotlightOn)
		drawSpotlightCone(heli);

	glPopMatrix();
}

void drawSpotlightCone(Helicopter* heli)
{
	glPushMatrix();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	GLfloat ambient[] = { 0.8, 0.8, 0.0, 0.5 };
	GLfloat diffuse[] = { 0.8, 0.8, 0.0, 0.5 };
	GLfloat specular[] = { 1.0, 1.0, 1.0, 0.5 };
	GLfloat shininess = 100.0f;

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

	glTranslated(0, -0.5 * heli->scaleFactor, 2 * heli->scaleFactor);
	glRotated(45, 1, 0, 0);

	// Cull back face so no strange dark light beam behind (opacity issue)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	gluCylinder(heli->quadric, 0, 500 * SCALE, TERRAIN_HEIGHT_WIDTH, 360, 1);
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glPopMatrix();
}