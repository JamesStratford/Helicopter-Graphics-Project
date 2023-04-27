#pragma once
#include "includes.h"

typedef struct
{
	GLUquadric* quadric;
	GLdouble size;
	Pos3 coordinates;
	Pos3 collisionBox;
	GLdouble rotorRotation;
	GLdouble rotorVelocity;
	GLint yaw;
	GLdouble pitch;
	GLdouble roll;
	GLint direction;
	GLdouble velocity;
	GLdouble strafeVelocity;
	GLdouble liftVelocity;
} Helicopter;


void initHelicopter(Helicopter* heli);
void drawHelicopter(Helicopter* heli);
