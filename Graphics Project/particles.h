#pragma once
#include "includes.h"
#include "texture-mapping.h"
#include "world.h"

#define MAX_PARTICLES	1000

typedef struct
{
	GLUquadric* quadric;
	GLuint sprite;
	GLfloat size;
	GLfloat scaleFactor;
	Pos3 coordinates;
	Pos3 collision;
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

void createParticleInRandomRadius(ParticleSystem* pSystem, Terrain* ter, Pos3* coordinates, GLfloat radius);
void drawParticles(ParticleSystem* pSystem);
void thinkParticles(ParticleSystem* pSystem, Terrain* ter);
