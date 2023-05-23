#include "terrain.h"
meshObject* terrainTestMesh = NULL;

meshObject* terrainOneMesh = NULL;
meshObject* terrainTwoMesh = NULL;
meshObject* terrainThreeMesh = NULL;
meshObject* terrainFourMesh = NULL;
GLint desertTexture = NULL;


void initTerrain(Terrain* ter, TerrainType type, GLfloat x, GLfloat z)
{
	if (!terrainTestMesh)
		terrainTestMesh = loadMeshObject("terrain.obj", "terrain.mtl");
	if (!terrainOneMesh)
		terrainOneMesh = loadMeshObject("terrain_1.obj", "terrain_1.mtl");
	if (!terrainTwoMesh)
		terrainTwoMesh = loadMeshObject("terrain_2.obj", "terrain_2.mtl");
	if (!terrainThreeMesh)
		terrainThreeMesh = loadMeshObject("terrain_3.obj", "terrain_3.mtl");
	if (!terrainFourMesh)
		terrainFourMesh = loadMeshObject("terrain_4.obj", "terrain_4.mtl");
	if (!desertTexture)
		desertTexture = loadTexture("desert_texture.bmp");

	switch(type)
	{
	case TERRAIN_1:
		ter->mesh = terrainTestMesh;
		break;
	case TERRAIN_2:
		ter->mesh = terrainTestMesh;
		break;
	case TERRAIN_3:
		ter->mesh = terrainTestMesh;
		break;
	case TERRAIN_4:
		ter->mesh = terrainTestMesh;
		break;
	}

	ter->baseSize = 100.0;
	ter->scaleFactor = 1.0;
	ter->postSize = ter->baseSize * ter->scaleFactor;
	ter->displayListIndex = acquireNewDisplayListNum();
	ter->texture = NULL;
	ter->terrainPosition.x = x;
	ter->terrainPosition.y = 0.0;
	ter->terrainPosition.z = z;
	ter->arrayIndexX = x / ter->postSize;
	ter->arrayIndexZ = z / ter->postSize;
	ter->texture = desertTexture;

	glNewList(ter->displayListIndex, GL_COMPILE);
	renderGround(ter);
	glEndList();
}

void renderGround(Terrain* ter)
{
	glPushMatrix();
	glTranslated(ter->terrainPosition.x, ter->terrainPosition.y, ter->terrainPosition.z);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ter->texture);
	glScaled(ter->scaleFactor, ter->scaleFactor, ter->scaleFactor);
	renderMeshObject(ter->mesh);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

int checkCollisionTerrain(Terrain* ter, Helicopter* heli)
{
	// Check if the object's z coordinate (height) is less than or equal to the height of the terrain at its x and y coordinates
	float x = heli->coordinates.x;
	float y = heli->coordinates.y;
	float z = heli->coordinates.z;
	float xDelta = heli->collisionBox.x / 2.0;
	float yDelta = heli->collisionBox.y / 2.0;
	float zDelta = heli->collisionBox.z / 2.0;
	
	//x -= ter->arrayIndexX * ter->postSize;
	//z -= ter->arrayIndexZ * ter->postSize;
	x -= ter->terrainPosition.x;
	z -= ter->terrainPosition.z;
	y -= ter->terrainPosition.y;


	//float collisionDistance = ter->scaleFactor * 0.15f;
	for (int i = 0; i < ter->mesh->vertexCount; i++)
	{
		if (x + xDelta > ter->mesh->vertices[i].x * ter->scaleFactor - xDelta && x - xDelta < ter->mesh->vertices[i].x * ter->scaleFactor + xDelta)
			if (z + zDelta > ter->mesh->vertices[i].z * ter->scaleFactor - zDelta && z - zDelta < ter->mesh->vertices[i].z * ter->scaleFactor + zDelta)
				if (y - yDelta <= ter->mesh->vertices[i].y * ter->scaleFactor)
					return 1;
	}

	return 0;
}

void freeTerrain()
{
	freeMeshObject(terrainTestMesh);
}