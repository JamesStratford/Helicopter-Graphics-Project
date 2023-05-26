#include "obj-loader.h"

/******************************************************************************
 * Mesh Object Loader Implementation
 ******************************************************************************/

 /*
	 Loads a Mesh Object from the specified file. If the file cannot be opened, this returns a null reference.

	 Note: Any object loaded via this function must eventually be freed via freeMeshObject(): a mesh object
	 returned by this function CANNOT be released with free().
 */
int findMtlIndex(meshObject* object, char* mtlName)
{
	int mtlIndex = -1;
	for (int i = 0; i < object->numMtlObjects; i++)
	{
		if (strcmp(object->mtlObjects[i]->mtlName, mtlName) == 0)
		{
			mtlIndex = i;
			break;
		}
	}
	return mtlIndex;
}

meshObject* loadMeshObject(char* fileName, char* mtlFileName)
{
	FILE* inFile;
	meshObject* object;
	char line[512];					// Line currently being parsed 
	char keyword[10];				// Keyword currently being parsed
	int currentVertexIndex = 0;		// 0-based index of the vertex currently being parsed
	int currentTexCoordIndex = 0;	// 0-based index of the texure coordinate currently being parsed
	int currentNormalIndex = 0;		// 0-based index of the normal currently being parsed
	int currentFaceIndex = 0;		// 0-based index of the face currently being parsed

	fopen_s(&inFile, fileName, "r");

	if (inFile == NULL) {
		return NULL;
	}

	// Allocate and initialize a new Mesh Object.
	object = malloc(sizeof(meshObject));
	object->vertexCount = 0;
	object->vertices = NULL;
	object->texCoordCount = 0;
	object->texCoords = NULL;
	object->normalCount = 0;
	object->normals = NULL;
	//object->faceCount = 0;
	//object->faces = NULL;
	object->numMtlObjects = 0;
	object->mtlObjects = calloc(10, sizeof(mtlObject*));
	for (int i = 0; i < 10; i++) {
		object->mtlObjects[i] = malloc(sizeof(mtlObject));
		object->mtlObjects[i]->faceCount = 0;
		object->mtlObjects[i]->faces = NULL;
	}

	// Pre-parse the file to determine how many vertices, texture coordinates, normals, and faces we have.
	while (fgets(line, (unsigned)_countof(line), inFile))
	{
		if (sscanf_s(line, "%9s", keyword, (unsigned)_countof(keyword)) == 1) 
		{
			keyword[9] = '\0';  // ensure null termination
			if (strcmp(keyword, "v") == 0) {
				object->vertexCount++;
			}
			else if (strcmp(keyword, "vt") == 0) {
				object->texCoordCount++;
			}
			else if (strcmp(keyword, "vn") == 0) {
				object->normalCount++;
			}
			// Faces
			else if (strcmp(keyword, "usemtl") == 0) {
				char name[50];
				sscanf_s(line, "%*s %s\0", name, sizeof(name));
				if (findMtlIndex(object, name) == -1)
				{
					strcpy_s(object->mtlObjects[object->numMtlObjects]->mtlName, sizeof(object->mtlObjects[object->numMtlObjects]->mtlName), name);
					++object->numMtlObjects;
				}
			}
			else if (strcmp(keyword, "f") == 0) {
				object->mtlObjects[object->numMtlObjects-1]->faceCount++;
			}
		}
	}

	if (object->vertexCount > 0)object->vertices = malloc(sizeof(vec3d) * object->vertexCount);
	if (object->texCoordCount > 0) object->texCoords = malloc(sizeof(vec2d) * object->texCoordCount);
	if (object->normalCount > 0) object->normals = malloc(sizeof(vec3d) * object->normalCount);
	for (int i = 0; i < object->numMtlObjects; i++)
	{
		if (object->mtlObjects[i]->faceCount > 0) {
			object->mtlObjects[i]->faces = malloc(sizeof(meshObjectFace) * object->mtlObjects[i]->faceCount);
		}

	}

	// Parse the file again, reading the actual vertices, texture coordinates, normals, and faces.
	rewind(inFile);

	int mtlIndex = -1;
	while (fgets(line, (unsigned)_countof(line), inFile))
	{
		if (sscanf_s(line, "%9s", keyword, (unsigned)_countof(keyword)) == 1) 
		{
			keyword[9] = '\0';  // ensure null termination
			if (strcmp(keyword, "v") == 0) {
				vec3d vertex = { 0, 0, 0 };
				sscanf_s(line, "%*s %f %f %f", &vertex.x, &vertex.y, &vertex.z);
				memcpy_s(&object->vertices[currentVertexIndex], sizeof(vec3d), &vertex, sizeof(vec3d));
				currentVertexIndex++;
			}
			else if (strcmp(keyword, "vt") == 0) {
				vec2d texCoord = { 0, 0 };
				sscanf_s(line, "%*s %f %f", &texCoord.x, &texCoord.y);
				memcpy_s(&object->texCoords[currentTexCoordIndex], sizeof(vec2d), &texCoord, sizeof(vec2d));
				currentTexCoordIndex++;
			}
			else if (strcmp(keyword, "vn") == 0) {
				vec3d normal = { 0, 0, 0 };
				sscanf_s(line, "%*s %f %f %f", &normal.x, &normal.y, &normal.z);
				memcpy_s(&object->normals[currentNormalIndex], sizeof(vec3d), &normal, sizeof(vec3d));
				currentNormalIndex++;
			}
			else if (strcmp(keyword, "usemtl") == 0) {
				//++mtlIndex;
				currentFaceIndex = 0;
				char name[50];
				sscanf_s(line, "%*s %s\0", name, sizeof(name));

				mtlIndex = findMtlIndex(object, name);
			}
			else if (strcmp(keyword, "f") == 0) {
				initMeshObjectFace(&(object->mtlObjects[mtlIndex]->faces[currentFaceIndex]), line, _countof(line));
				currentFaceIndex++;
			}
		}
	}

	fclose(inFile);

	if (!mtlFileName)
	{
		object->hasMtlChildren = 0;
		return object;
	}

	FILE* mtlFile;
	fopen_s(&mtlFile, mtlFileName, "r");

	if (mtlFile == NULL) 
	{
		return object;
	}
	object->hasMtlChildren = 1;
	mtlIndex = -1;

	while (fgets(line, (unsigned)_countof(line), mtlFile))
	{
		if (sscanf_s(line, "%9s", keyword, (unsigned)_countof(keyword)) == 1) 
		{
			keyword[9] = '\0';  // ensure null termination
			if (strcmp(keyword, "newmtl") == 0) {
				char name[50];
				sscanf_s(line, "%*s %s\0", name, sizeof(name));
				mtlIndex = findMtlIndex(object, name);
				if (mtlIndex == -1) break;
			}
			else if (strcmp(keyword, "Ns") == 0) {
				GLfloat shininess = 0.0f;
				sscanf_s(line, "%*s %f", &shininess);
				memcpy_s(&object->mtlObjects[mtlIndex]->shininess, sizeof(GLfloat), &shininess, sizeof(GLfloat));
			}
			else if (strcmp(keyword, "Ka") == 0) {
				GLfloat ambientCol[4] = {0.0, 0.0, 0.0, 1.0};
				sscanf_s(line, "%*s %f %f %f", &ambientCol[0], &ambientCol[1], &ambientCol[2]);
				memcpy_s(&object->mtlObjects[mtlIndex]->ambientCol, sizeof(GLfloat[4]), &ambientCol, sizeof(GLfloat[4]));
			}
			else if (strcmp(keyword, "Kd") == 0) {
				GLfloat diffuseCol[4] = { 0.0, 0.0, 0.0, 1.0 };
				sscanf_s(line, "%*s %f %f %f", &diffuseCol[0], &diffuseCol[1], &diffuseCol[2]);
				memcpy_s(&object->mtlObjects[mtlIndex]->diffuseCol, sizeof(GLfloat[4]), &diffuseCol, sizeof(GLfloat[4]));
			}
			else if (strcmp(keyword, "Ks") == 0) {
				GLfloat specularCol[4] = { 0.0, 0.0, 0.0, 1.0 };
				sscanf_s(line, "%*s %f %f %f", &specularCol[0], &specularCol[1], &specularCol[2]);
				memcpy_s(&object->mtlObjects[mtlIndex]->specularCol, sizeof(GLfloat[4]), &specularCol, sizeof(GLfloat[4]));
			}
			else if (strcmp(keyword, "illum") == 0) {
				GLint illum = 0;
				sscanf_s(line, "%*s %d", &illum);
				memcpy_s(&object->mtlObjects[mtlIndex]->illum, sizeof(GLint), &illum, sizeof(GLint));
			}
		}
	}
	fclose(mtlFile);

	return object;
}

/*
	Render the faces of the specified Mesh Object in OpenGL.
*/
void renderMeshObject(meshObject* object) 
{
	for (int mtlNo = 0; mtlNo < object->numMtlObjects; mtlNo++) 
	{
		if (object->hasMtlChildren)
		{
			glShadeModel(GL_SMOOTH);
			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, object->mtlObjects[mtlNo]->ambientCol);
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, object->mtlObjects[mtlNo]->diffuseCol);
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, object->mtlObjects[mtlNo]->specularCol);
			glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, object->mtlObjects[mtlNo]->shininess);
		}
		for (int faceNo = 0; faceNo < object->mtlObjects[mtlNo]->faceCount; faceNo++) {
			meshObjectFace face = object->mtlObjects[mtlNo]->faces[faceNo];
			if (face.pointCount >= 3) {
				glBegin(GL_POLYGON);

				for (int pointNo = 0; pointNo < face.pointCount; pointNo++) {
					meshObjectFacePoint point = face.points[pointNo];

					if (point.normalIndex >= 0) {
						vec3d normal = object->normals[point.normalIndex];
						glNormal3d(normal.x, normal.y, normal.z);
					}

					if (point.texCoordIndex >= 0) {
						vec2d texCoord = object->texCoords[point.texCoordIndex];
						glTexCoord2d(texCoord.x, texCoord.y);
					}

					vec3d vertex = object->vertices[point.vertexIndex];

					glVertex3f(vertex.x, vertex.y, vertex.z);
				}

				glEnd();
			}
		}
	}
}

/*
	Initialise the specified Mesh Object Face from a string of face data in the Wavefront OBJ file format.
*/
void initMeshObjectFace(meshObjectFace* face, char* faceData, int maxFaceDataLength) 
{
	int maxPoints = 0;
	int inWhitespace = 0;
	const char* delimiter = " ";
	char* token = NULL;
	char* context = NULL;

	// Do a quick scan of the input string to determine the maximum number of points in this face by counting
	// blocks of whitespace (each point must be preceded by at least one space). Note that we may end up with
	// fewer points than this if one or more prove to be invalid.
	for (int i = 0; i < maxFaceDataLength; i++)
	{
		char c = faceData[i];
		if (c == '\0') {
			break;
		}
		else if ((c == ' ') || (c == '\t')) {
			if (!inWhitespace) {
				inWhitespace = 1;
				maxPoints++;
			}
		}
		else {
			inWhitespace = 0;
		}
	}

	// Parse the input string to extract actual face points (if we're expecting any).
	face->pointCount = 0;
	if (maxPoints > 0) 
	{
		face->points = malloc(sizeof(meshObjectFacePoint) * maxPoints);

		token = strtok_s(faceData, delimiter, &context);
		while ((token != NULL) && (face->pointCount < maxPoints)) 
		{
			meshObjectFacePoint parsedPoint = { 0, 0, 0 }; // At this point we're working with 1-based indices from the OBJ file.

			if (strcmp(token, "f") != 0) {
				// Attempt to parse this face point in the format "v/t[/n]" (vertex, texture, and optional normal).
				if (sscanf_s(token, "%d/%d/%d", &parsedPoint.vertexIndex, &parsedPoint.texCoordIndex, &parsedPoint.normalIndex) < 2) {
					// That didn't work out: try parsing in the format "v[//n]" instead (vertex, no texture, and optional normal).
					sscanf_s(token, "%d//%d", &parsedPoint.vertexIndex, &parsedPoint.normalIndex);
				}

				// If we parsed a valid face point (i.e. one that at least contains the index of a vertex), add it.
				if (parsedPoint.vertexIndex > 0) {
					// Adjust all indices down by one: Wavefront OBJ uses 1-based indices, but our arrays are 0-based.
					parsedPoint.vertexIndex--;

					// Discard any negative texture coordinate or normal indices while adjusting them down by one.
					parsedPoint.texCoordIndex = (parsedPoint.texCoordIndex > 0) ? parsedPoint.texCoordIndex - 1 : -1;
					parsedPoint.normalIndex = (parsedPoint.normalIndex > 0) ? parsedPoint.normalIndex - 1 : -1;

					memcpy_s(&face->points[face->pointCount], sizeof(meshObjectFacePoint), &parsedPoint, sizeof(meshObjectFacePoint));
					face->pointCount++;
				}
			}

			token = strtok_s(NULL, delimiter, &context);
		}

		// If we have fewer points than expected, free the unused memory.
		if (face->pointCount == 0) 
		{
			free(face->points);
			face->points = NULL;
		}
		else if (face->pointCount < maxPoints) {
			meshObjectFacePoint* temp = realloc(face->points, sizeof(meshObjectFacePoint) * face->pointCount);
			if (temp == NULL) 
			{
				// Handle the error, e.g., by freeing memory and exiting
				free(face->points);
				fprintf(stderr, "Error reallocating memory.\n");
				exit(1);
			}
			else 
			{
				face->points = temp;
			}
		}
	}
	else
	{
		face->points = NULL;
	}
}

/*
	Free the specified Mesh Object, including all of its vertices, texture coordinates, normals, and faces.
*/
void freeMeshObject(meshObject* object)
{
	if (object != NULL) {
		free(object->vertices);
		object->vertices = NULL;

		free(object->texCoords);
		object->texCoords = NULL;

		free(object->normals);
		object->normals = NULL;

		for (int mtlNo = 0; mtlNo < object->numMtlObjects; mtlNo++) 
		{
			if (object->mtlObjects[mtlNo]->faces != NULL) {
				for (int i = 0; i < object->mtlObjects[mtlNo]->faceCount; i++) 
				{
					if (object->mtlObjects[mtlNo]->faces[i].points != NULL) 
					{
						free(object->mtlObjects[mtlNo]->faces[i].points);
						object->mtlObjects[mtlNo]->faces[i].points = NULL;
					}
				}

				free(object->mtlObjects[mtlNo]->faces);
				object->mtlObjects[mtlNo]->faces = NULL;
			}
		}

		free(object);
		object = NULL;
	}
}
