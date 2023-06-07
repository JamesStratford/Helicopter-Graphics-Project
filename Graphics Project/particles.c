#include "particles.h"
#include <math.h>

extern const float FRAME_TIME_SEC;

void initParticleSystem(ParticleSystem* pSystem)
{
	GLfloat ambient[] = { 0.76, 0.69, 0.57, 0.5 };
	GLfloat diffuse[] = { 0.76, 0.69, 0.57, 0.5 };
	GLfloat specular[] = { 0.0, 0.0, 0.0, 0.5 };
	GLfloat shininess = 0.0;
	
	pSystem->kingQuadric = gluNewQuadric();
	pSystem->numOfParticles = MAX_PARTICLES;
	GLuint dustTex = loadTexture("dust.bmp");
	for (int i = 0; i < pSystem->numOfParticles; i++)
	{
		pSystem->particles[i].quadric = pSystem->kingQuadric;
		pSystem->particles[i].sprite = dustTex;
		memcpy_s(pSystem->particles[i].mat.ambient, sizeof(float[4]), ambient, sizeof(float[4]));
		memcpy_s(pSystem->particles[i].mat.diffuse, sizeof(float[4]), diffuse, sizeof(float[4]));
		memcpy_s(pSystem->particles[i].mat.specular, sizeof(float[4]), specular, sizeof(float[4]));
		pSystem->particles[i].mat.shininess = 0;
	}

	pSystem->windDirection.x = -1.0;
	pSystem->windDirection.y = -1.0;
}

void createParticleInRandomRadius(ParticleSystem* pSystem, Terrain* ter, Pos3* coordinates, GLfloat radius)
{
	for (int i = 0; i < pSystem->numOfParticles; i++)
	{
		Particle* particle = &pSystem->particles[i];
		if (particle->life <= 0)
		{
			particle->size = (float)(rand() % 5) + 2;
			particle->scaleFactor = SCALE;
			particle->life = 100.0f;
			particle->velocity = (float)(rand() % 100) + 100.0f;
			particle->alpha = (float)(rand() % 100) / 100.0f;

			int angle = rand() % 360;
			double distance = rand() % (int)radius;

			particle->coordinates.x = coordinates->x + distance * cos((angle * PI) / 180.0);
			particle->coordinates.z = coordinates->z + distance * sin((angle * PI) / 180.0);
			particle->coordinates.y = 0;

			particle->collision.x = particle->size / 2.0f;
			particle->collision.y = particle->size / 2.0f;
			particle->collision.z = particle->size;

			if (ter->mesh)
			{
				for (int i = 0; i < ter->mesh->vertexCount; i++)
				{
					if (particle->coordinates.x < ter->mesh->vertices[i].x + ter->terrainPosition.x + 1 && particle->coordinates.x > ter->mesh->vertices[i].x + ter->terrainPosition.x - 1)
						if (particle->coordinates.z < ter->mesh->vertices[i].z + ter->terrainPosition.z + 1 && particle->coordinates.z > ter->mesh->vertices[i].z + ter->terrainPosition.z - 1)
						{
							particle->coordinates.y = ter->mesh->vertices[i].y + 5.0f;
							break;
						}
				}
			}
			break;
		}
	}
}

void drawParticle(Particle* particle)
{
	if (particle && particle->life > 0)
	{
		glPushMatrix();
		glTranslated(particle->coordinates.x, particle->coordinates.y, particle->coordinates.z);
		glScaled(particle->scaleFactor, particle->scaleFactor, particle->scaleFactor);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		glEnable(GL_TEXTURE_2D);
		
		// Sand Mat
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, particle->mat.ambient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, particle->mat.diffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, particle->mat.specular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, particle->mat.shininess);

		gluQuadricTexture(particle->quadric, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D, particle->sprite);
		
		// Wind direction
		glRotated(45, 0, 1, 0);
		glScaled(1, 1, 2);

		gluSphere(particle->quadric, particle->size * particle->scaleFactor, 7, 7);
		
		glDisable(GL_TEXTURE_2D);
		glDisable(GL_BLEND);
		glPopMatrix();
	}
}

void drawParticles(ParticleSystem* pSystem)
{
	for (int i = 0; i < pSystem->numOfParticles; i++)
	{
		drawParticle(&pSystem->particles[i]);
	}
}


void thinkParticles(ParticleSystem* pSystem, Terrain* ter)
{
	for (int i = 0; i < pSystem->numOfParticles; i++)
	{
		Particle* particle = &pSystem->particles[i];
		if (particle->life > 0.0)
		{
			particle->life -= 1.0f;
			particle->coordinates.x += pSystem->windDirection.x * particle->velocity * SCALE * FRAME_TIME_SEC;
			particle->coordinates.z += pSystem->windDirection.y * particle->velocity * SCALE * FRAME_TIME_SEC;

			if (ter->mesh)
			{
				Pos3* pos = getCollisionTerrain(ter, &particle->coordinates, &particle->collision);
				if (pos)
					particle->coordinates.y = pos->y + particle->size * particle->scaleFactor;
			}
		}
	}
}