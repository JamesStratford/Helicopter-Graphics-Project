#pragma once
#include "includes.h"

typedef struct
{
	GLUquadric* quadric;
	GLdouble size;
	Pos3 coordinates;
	Pos3 collisionBox;
	GLdouble rotorRotation;
	GLint heading; // yaw
	GLdouble pitch;
	GLdouble roll;

} Helicopter;


void initHelicopter();
void drawHelicopter();
