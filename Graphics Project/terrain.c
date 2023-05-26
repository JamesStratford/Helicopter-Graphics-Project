#include "terrain.h"
#include <math.h>

meshObject* terrainTestMesh = NULL;
GLint desertTexture = NULL;


void initTerrain(Terrain* ter, GLfloat x, GLfloat z)
{
	if (!terrainTestMesh)
		terrainTestMesh = loadMeshObject("terrain_1.obj", "terrain_1.mtl");

	ter->mesh = terrainTestMesh;

	ter->baseSize = 100.0;
	ter->scaleFactor = SCALE;
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

void initSkybox(Skybox* skybox)
{
	skybox->quadric = gluNewQuadric();
	skybox->texture = loadTexture("night.bmp");
	skybox->size = 1000;
	skybox->scaleFactor = SCALE;
	skybox->coordinates.x = 1000;
	skybox->coordinates.y = 1000;
	skybox->coordinates.z = 0;
}

void initMoon(Moon* moon)
{
	moon->coordinates.x = 550;
	moon->coordinates.y = 800;
	moon->coordinates.z = 1000;

	moon->baseSize = 50;
	moon->scaleFactor = SCALE;
	moon->quadric = gluNewQuadric();
	moon->texture = loadTexture("moon.bmp");

	GLfloat ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };

	memcpy_s(moon->mat.ambient, sizeof(float[4]), ambient, sizeof(float[4]));
	memcpy_s(moon->mat.diffuse, sizeof(float[4]), diffuse, sizeof(float[4]));
	memcpy_s(moon->mat.specular, sizeof(float[4]), specular, sizeof(float[4]));
	moon->mat.shininess = 100.0f;
}

void drawMoon(Moon* moon)
{
	glPushMatrix();
	glTranslated(moon->coordinates.x, moon->coordinates.y, moon->coordinates.z);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, moon->mat.ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, moon->mat.diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, moon->mat.specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, moon->mat.shininess);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, moon->texture);
	glScaled(moon->scaleFactor, moon->scaleFactor, moon->scaleFactor);
	gluQuadricTexture(moon->quadric, GL_TRUE);
	gluSphere(moon->quadric, moon->baseSize * moon->scaleFactor, 360, 360);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void drawSkybox(Skybox* skybox)
{
	glPushMatrix();
	float xyPoint = TERRAIN_GRID_SIZE * TERRAIN_GRID_LEGNTH / 2;
	glTranslated(xyPoint, 0, xyPoint);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skybox->texture);
	gluQuadricTexture(skybox->quadric, TRUE);
	gluSphere(skybox->quadric, skybox->size, 360, 360);
	glDisable(GL_TEXTURE_2D);
	glDisable(GL_CULL_FACE);

	glPopMatrix();
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

int checkCollisionSkybox(Skybox* skybox, Helicopter* heli)
{
	double collisionRadius = skybox->size - skybox->size * 0.01;
	double distance = sqrt(
		pow(heli->coordinates.x - skybox->coordinates.x, 2) +
		pow(heli->coordinates.y - skybox->coordinates.z, 2) +
		pow(heli->coordinates.z - skybox->coordinates.y, 2)
	);

	return (distance >= collisionRadius);
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