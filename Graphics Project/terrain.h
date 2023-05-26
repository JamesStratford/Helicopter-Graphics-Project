#pragma once
#include "includes.h"
#include "obj-loader.h"
#include "texture-mapping.h"
#include "helicopter.h"

typedef struct {
	GLUquadric* quadric;
	GLuint texture;
	Pos3 coordinates;
	GLfloat size;
	GLfloat scaleFactor;
} Skybox;

typedef struct {
	Pos3 terrainPosition;
	meshObject* mesh;
	GLuint texture;
	GLfloat baseSize;
	GLfloat scaleFactor;
	GLfloat postSize;
	int arrayIndexX;
	int arrayIndexZ;
	GLuint displayListIndex;
} Terrain;

typedef struct {
	Pos3 coordinates;
	GLUquadric* quadric;
	Material mat;
	GLuint texture;
	GLfloat baseSize;
	GLfloat scaleFactor;
} Moon;

void initTerrain(Terrain* ter, GLfloat x, GLfloat z);
void renderGround(Terrain* ter);
int checkCollisionTerrain(Terrain* ter, Helicopter* heli);
void freeTerrain();

void initSkybox(Skybox* skybox);

void drawSkybox(Skybox* skybox);
int checkCollisionSkybox(Skybox* skybox, Helicopter* heli);

void initMoon(Moon* moon);
void drawMoon(Moon* moon);
