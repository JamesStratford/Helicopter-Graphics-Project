#include "terrain.h"


void initTerrain(Terrain* ter, GLfloat x, GLfloat z)
{
	ter->mesh = loadMeshObject("terrain.obj", "terrain.mtl");
	ter->baseSize = 20.0;
	ter->scaleFactor = 100.0f;
	ter->postSize = ter->baseSize * ter->scaleFactor;
	ter->displayListIndex = acquireNewDisplayListNum();
	ter->texture = NULL;
	ter->terrainPosition.x = x;
	ter->terrainPosition.z = z;
	ter->arrayIndexX = x / ter->postSize;
	ter->arrayIndexZ = z / ter->postSize;
	//ter->texture = loadTexture("desert_texture.bmp");

	glNewList(ter->displayListIndex, GL_COMPILE);
	renderGround(ter);
	glEndList();
}

void renderGround(Terrain* ter)
{
	glPushMatrix();
	glTranslated(ter->terrainPosition.x, 0.0, ter->terrainPosition.z);
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