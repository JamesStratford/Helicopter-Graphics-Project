#include "world.h"
#include <math.h>

GLint desertTexture = 0;
extern int renderFillEnabled;
Pos3 g_CollisionCheckOutput;


void initObjTerrain(Terrain* ter, GLfloat x, GLfloat z)
{
	if (!ter->mesh)
		ter->mesh = loadMeshObject("terrain_1.obj", "terrain_1.mtl");

	ter->baseSize = 2;
	ter->scaleFactor = TERRAIN_GRID_SIZE * 10 * SCALE; // Blender obj file is 2x2 units
	ter->postSize = ter->baseSize * ter->scaleFactor;
	ter->displayListIndex = acquireNewDisplayListNum();
	ter->terrainPosition.x = x;
	ter->terrainPosition.y = 0.0;
	ter->terrainPosition.z = z;
	ter->texture = desertTexture;

	glNewList(ter->displayListIndex, GL_COMPILE);
	renderGround(ter);
	glEndList();
}

void initGridTerrain(Terrain* ter, GLfloat x, GLfloat z)
{
	ter->baseSize = TERRAIN_GRID_SIZE;
	ter->scaleFactor = SCALE;
	ter->postSize = ter->baseSize * ter->scaleFactor;
	ter->terrainPosition.x = x;
	ter->terrainPosition.y = 0.0;
	ter->terrainPosition.z = z;
	ter->arrayIndexX = (int)(x / ter->postSize);
	ter->arrayIndexZ = (int)(z / ter->postSize);
	ter->texture = desertTexture;
}

void initSkybox(Skybox* skybox)
{
	skybox->quadric = gluNewQuadric();
	skybox->texture = loadTexture("night.bmp");
	skybox->size = 1000.0f;
	skybox->scaleFactor = SCALE;
	skybox->coordinates.x = skybox->size * skybox->scaleFactor;
	skybox->coordinates.y = skybox->size * skybox->scaleFactor;
	skybox->coordinates.z = 0;

	GLfloat ambient[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };

	memcpy_s(skybox->mat.ambient, sizeof(float[4]), ambient, sizeof(float[4]));
	memcpy_s(skybox->mat.diffuse, sizeof(float[4]), diffuse, sizeof(float[4]));
	memcpy_s(skybox->mat.specular, sizeof(float[4]), specular, sizeof(float[4]));
	skybox->mat.shininess = 100.0f;
}

void initMoon(Moon* moon)
{
	moon->coordinates.x = 550 * SCALE;
	moon->coordinates.y = 750 * SCALE;
	moon->coordinates.z = 1000 * SCALE;

	moon->baseSize = 100.0f;
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

void initWorldObject(WorldObject* obj, Pos3 position, GLfloat size, GLfloat rotation[3], const char* meshFilePath, const char* mtlFilePath, const char* textureFilePath)
{
	if (meshFilePath)
		obj->mesh = loadMeshObject(meshFilePath, mtlFilePath);

	obj->quadric = gluNewQuadric();

	obj->baseSize = size;
	memcpy_s(obj->rotation, 3 * sizeof(GLfloat), rotation, 3 * sizeof(GLfloat));
	obj->scaleFactor = SCALE;

	obj->position = position;

	if (textureFilePath)
		obj->texture = loadTexture(textureFilePath);
}

void drawWorldObject(WorldObject* obj)
{
	glPushMatrix();
	glTranslated(obj->position.x, obj->position.y, obj->position.z);
	glRotated(obj->rotation[0], 1.0, 0.0, 0.0);
	glRotated(obj->rotation[1], 0.0, 1.0, 0.0);
	glRotated(obj->rotation[2], 0.0, 0.0, 1.0);
	glScaled(obj->baseSize, obj->baseSize, obj->baseSize);
	glScaled(obj->scaleFactor, obj->scaleFactor, obj->scaleFactor);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, obj->texture);
	renderMeshObject(obj->mesh);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
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
	gluQuadricTexture(moon->quadric, GL_TRUE);
	gluSphere(moon->quadric, moon->baseSize * moon->scaleFactor, 360, 360);
	glDisable(GL_TEXTURE_2D);

	glPopMatrix();
}

void drawSkybox(Skybox* skybox)
{
	glPushMatrix();
	float xyPoint = (TERRAIN_GRID_SIZE * TERRAIN_GRID_LEGNTH / 2) * skybox->scaleFactor;
	glTranslated(xyPoint, 0, xyPoint);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_FRONT);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, skybox->mat.ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, skybox->mat.diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, skybox->mat.specular);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, skybox->mat.shininess);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, skybox->texture);
	gluQuadricTexture(skybox->quadric, TRUE);
	glRotated(90, -1, 0, 0);
	gluCylinder(skybox->quadric,
		skybox->size * skybox->scaleFactor,
		skybox->size * skybox->scaleFactor,
		skybox->size * skybox->scaleFactor * 2,
		360,
		360);

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

void renderGridGround(Terrain* ter)
{
	glPushMatrix();
	glTranslated(ter->terrainPosition.x, ter->terrainPosition.y, ter->terrainPosition.z);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, ter->texture);
	glScaled(ter->scaleFactor, ter->scaleFactor, ter->scaleFactor);

	glBegin(GL_QUADS);
	GLfloat terrainDelta = ter->postSize / 2.0f;
	glVertex3d(-terrainDelta, 0, -terrainDelta);
	glVertex3d(-terrainDelta, 0, terrainDelta);
	glVertex3d(terrainDelta, 0, terrainDelta);
	glVertex3d(terrainDelta, 0, -terrainDelta);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

void drawOasisScene(WorldObject** objs, int lenObjs)
{
	for (int i = 0; i < lenObjs; i++)
	{
		drawWorldObject(objs[i]);
	}
}

int checkCollisionSkybox(Skybox* skybox, Pos3* object)
{
	double collisionRadius = skybox->size * skybox->scaleFactor - skybox->size * skybox->scaleFactor * 0.05; // small buffer before edge
	double distance = sqrt(
		pow(object->x - skybox->coordinates.x, 2) +
		pow(object->z - skybox->coordinates.y, 2)
	);
	double topCollision = collisionRadius * 2.0 <= object->y;

	return (distance >= collisionRadius || topCollision);
}

Pos3* getCollisionTerrain(Terrain* ter, Pos3* object, Pos3* collision)
{
	if (!ter || !object)
		return NULL;
	
	double x = object->x;
	double y = object->y;
	double z = object->z;
	double xDelta = 0.0;
	double yDelta = 0.0;
	double zDelta = 0.0;

	if (collision)
	{
		xDelta = collision->x / 2.0;
		yDelta = collision->y / 2.0;
		zDelta = collision->z / 2.0;
	}


	if (ter->mesh)
	{
		float searchDist = 1 / sqrtf((float)ter->mesh->vertexCount) * ter->scaleFactor;
		for (int i = 0; i < ter->mesh->vertexCount; i++)
		{
			if (x + xDelta > (ter->mesh->vertices[i].x * ter->scaleFactor) + ter->terrainPosition.x - searchDist && x - xDelta < (ter->mesh->vertices[i].x * ter->scaleFactor) + ter->terrainPosition.x + searchDist)
					if (z + zDelta > (ter->mesh->vertices[i].z * ter->scaleFactor) + ter->terrainPosition.z - searchDist && z - zDelta < (ter->mesh->vertices[i].z * ter->scaleFactor) + ter->terrainPosition.z + searchDist)
						if (y - yDelta <= (ter->mesh->vertices[i].y * ter->scaleFactor) + ter->terrainPosition.y)
						{
							g_CollisionCheckOutput.x = (ter->mesh->vertices[i].x * ter->scaleFactor) + ter->terrainPosition.x;
							g_CollisionCheckOutput.y = (ter->mesh->vertices[i].y * ter->scaleFactor) + ter->terrainPosition.y;
							g_CollisionCheckOutput.z = (ter->mesh->vertices[i].z * ter->scaleFactor) + ter->terrainPosition.z;

							return &g_CollisionCheckOutput;
						}
		}
	}

	return NULL;
}

void drawHelipad(WorldObject* helipad)
{
	glPushMatrix();
	glTranslated(helipad->position.x, helipad->position.y, helipad->position.z);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, helipad->texture);
	gluQuadricTexture(helipad->quadric, TRUE);
	glRotated(helipad->rotation[0], 1.0, 0.0, 0.0);
	glRotated(helipad->rotation[1], 0.0, 1.0, 0.0);
	glRotated(helipad->rotation[2], 0.0, 0.0, 1.0);

	gluDisk(helipad->quadric, 0.0, helipad->baseSize * helipad->scaleFactor, 180, 180);
	gluCylinder(helipad->quadric, helipad->baseSize * helipad->scaleFactor * 0.75, helipad->baseSize * helipad->scaleFactor * 0.75, helipad->baseSize * helipad->scaleFactor, 8, 1);
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

	glPushMatrix();
	glTranslated(helipad->position.x, helipad->position.y + 2.0, helipad->position.z);
	glScaled(100 * helipad->scaleFactor, 100 * helipad->scaleFactor, 100 * helipad->scaleFactor);
	renderMeshObject(helipad->mesh);
	glPopMatrix();

}

int getCollisionHelipad(WorldObject* helipad, Pos3* object, Pos3* collision)
{
	if (!helipad || !object)
		return 0;
	// Check if the object's z coordinate (height) is less than or equal to the height of the terrain at its x and y coordinates
	double x = object->x;
	double y = object->y;
	double z = object->z;
	double xDelta = 0.0;
	double yDelta = 0.0;
	double zDelta = 0.0;

	if (collision)
	{
		xDelta = collision->x / 2.0;
		yDelta = collision->y / 2.0;
		zDelta = collision->z / 2.0;
	}

	GLfloat radius = helipad->baseSize * helipad->scaleFactor;
	Pos3 helipadPos = helipad->position;

	double distance = sqrt(pow(helipadPos.x - x, 2) + pow(helipadPos.y - y, 2));
	if (distance <= radius)
	{
		if (object->y - yDelta <= helipadPos.y)
			return 1;
	}

	return 0;
}