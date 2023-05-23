#pragma once
#include "includes.h"
#include "obj-loader.h"
#include "texture-mapping.h"
#include "helicopter.h"

//typedef struct {
//	Terrain** ppTerrain;
//	int rows;
//	int cols;
//} TerrainGrid;

typedef enum {
	TERRAIN_1,
	TERRAIN_2,
	TERRAIN_3,
	TERRAIN_4
} TerrainType;

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

void initTerrain(Terrain* ter, TerrainType type, GLfloat x, GLfloat z);
void renderGround(Terrain* ter);
int checkCollisionTerrain(Terrain* ter, Helicopter* heli);
void freeTerrain();