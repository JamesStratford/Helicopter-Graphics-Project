#pragma once
#include <stdio.h>
#include <Windows.h>
#include "freeglut.h"

#define PI		3.14159265359

typedef struct
{
	GLdouble x;
	GLdouble y;
} Pos2;

typedef struct
{
	GLdouble x;
	GLdouble y;
	GLdouble z;
} Pos3;

typedef struct
{
	GLfloat ambient[4];
	GLfloat diffuse[4];
	GLfloat specular[4];
	GLfloat shininess;
} Material;

/**
*	Assigns display list value and increments global.
*/
inline GLuint acquireNewDisplayListNum()
{
	extern GLuint g_displayListIndex;
	return ++g_displayListIndex;
}