#pragma once
#include "includes.h"
#include "obj-loader.h"

typedef struct
{
	meshObject* mesh;
	Material mat;
	GLUquadric* quadric;
	GLdouble size;
	GLfloat scaleFactor;
	Pos3 coordinates;
	Pos3 collisionBox;
	GLdouble rotorRotation;
	GLdouble rotorVelocity;
	GLdouble yaw;
	GLdouble pitch;
	GLdouble roll;
	GLint direction;
	GLdouble velocity;
	GLdouble strafeVelocity;
	GLdouble liftVelocity;
	GLdouble maxSpeed;
	GLuint texture;
} Helicopter;


void initHelicopter(Helicopter* heli);
void drawHelicopter(Helicopter* heli);
