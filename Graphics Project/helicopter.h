#pragma once
#include "includes.h"

typedef struct
{
	GLUquadric* quadric;
	Pos3 coordinates;
	GLdouble size;
	GLdouble rotorRotation;
	GLint heading; // yaw
	GLdouble pitch;
	GLdouble roll;

} Helicopter;


void initHelicopter();
void drawHelicopter();
