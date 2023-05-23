#pragma once
#include "includes.h"
#include "texture-mapping.h"
#include "terrain.h"

#define MAX_PARTICLES	2000

typedef struct
{
	GLUquadric* quadric;
	GLuint sprite;
	GLfloat size;
	GLfloat scaleFactor;
	Pos3 coordinates;
	GLfloat life;
	GLfloat velocity;
	GLfloat alpha;
	Material mat;
} Particle;

typedef struct
{
	GLUquadric* kingQuadric;
	GLuint numOfParticles;
	Particle particles[MAX_PARTICLES];
	GLuint maxParticles;
	GLuint livingParticles;
	Pos2 windDirection;
} ParticleSystem;

void initParticleSystem(ParticleSystem* pSystem);

void createParticleInRandomRadius(ParticleSystem* pSystem, Terrain terrain[][TERRAIN_GRID_LEGNTH], Pos3* coordinates, GLfloat radius);
void drawParticles(ParticleSystem* pSystem);
void thinkParticles(ParticleSystem* pSystem, Terrain terrain[][TERRAIN_GRID_LEGNTH]);
