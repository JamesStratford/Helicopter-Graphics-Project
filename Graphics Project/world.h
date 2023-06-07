#pragma once
#include "includes.h"
#include "obj-loader.h"
#include "texture-mapping.h"
#include "helicopter.h"

typedef struct {
	GLUquadric* quadric;
	Material mat;
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

typedef struct {
	Pos3 position;
	GLUquadric* quadric;
	meshObject* mesh;
	GLfloat rotation[3];
	GLuint texture;
	GLfloat baseSize;
	GLfloat scaleFactor;
} WorldObject;


void initObjTerrain(Terrain* ter, GLfloat x, GLfloat z);
void initGridTerrain(Terrain* ter, GLfloat x, GLfloat z);
void renderGround(Terrain* ter);
void renderGridGround(Terrain* ter);
Pos3* getCollisionTerrain(Terrain* ter, Pos3* object, Pos3* collision);

void initSkybox(Skybox* skybox);
void drawSkybox(Skybox* skybox);
int checkCollisionSkybox(Skybox* skybox, Pos3* object);

void initWorldObject(WorldObject* obj, Pos3 position, GLfloat size, GLfloat rotation[3], const char* meshFilePath, const char* mtlFilePath, const char* textureFilePath);
void drawWorldObject(WorldObject* obj);
void drawHelipad(WorldObject* helipad);
int getCollisionHelipad(WorldObject* helipad, Pos3* object, Pos3* collision);

void drawOasisScene(WorldObject** objs, int lenObjs);

void initMoon(Moon* moon);
void drawMoon(Moon* moon);