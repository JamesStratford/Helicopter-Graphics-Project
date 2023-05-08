#pragma once
#include "includes.h"

/******************************************************************************
 * Mesh Object Loader Setup and Prototypes
 ******************************************************************************/

typedef struct {
	GLfloat x;
	GLfloat y;
	GLfloat z;
} vec3d;

typedef struct {
	GLfloat x;
	GLfloat y;
} vec2d;

typedef struct {
	int vertexIndex;	// Index of this vertex in the object's vertices array
	int texCoordIndex; // Index of the texture coordinate for this vertex in the object's texCoords array
	int normalIndex;	// Index of the normal for this vertex in the object's normals array
} meshObjectFacePoint;

typedef struct {
	int pointCount;
	meshObjectFacePoint* points;
} meshObjectFace;

typedef struct {
	char mtlName[50];
	int faceCount;
	meshObjectFace* faces;

	GLfloat shininess;
	GLfloat ambientCol[4];
	GLfloat diffuseCol[4];
	GLfloat specularCol[4];
	GLint illum;
} mtlObject;

typedef struct {
	int vertexCount;
	vec3d* vertices;
	int texCoordCount;
	vec2d* texCoords;
	int normalCount;
	vec3d* normals;
	//int faceCount;
	//meshObjectFace* faces;
	int hasMtlChildren;
	int numMtlObjects;
	mtlObject** mtlObjects;
} meshObject;

meshObject* loadMeshObject(char* fileName, char* mtlFileName);
void renderMeshObject(meshObject* object);
void initMeshObjectFace(meshObjectFace* face, char* faceData, int faceDataLength);
void freeMeshObject(meshObject* object);

// ----------------------------------------------------------------------------------
