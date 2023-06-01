#pragma once
#include <stdio.h>
#include "glew.h"
#include "freeglut.h"
#include <Windows.h>
#include <math.h>

#define PI					3.14159265359
#define TERRAIN_GRID_SIZE	100
#define TERRAIN_GRID_LEGNTH 20
#define SCALE				5.0f

#define CLAMP(x, lower, upper) (min(upper, max(x, lower)))

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

