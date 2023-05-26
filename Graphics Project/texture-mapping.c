#include "texture-mapping.h"

GLint loadTexture(const char* filename)
{
	GLint texture;
	unsigned char* data;

	// Part of the wingdi.h file
	BITMAPFILEHEADER fileHeader;
	BITMAPINFOHEADER infoHeader;
	// -------------------------

	FILE* file;
	fopen_s(&file, filename, "rb");

	if (file == NULL) 
		return 0;

	fread(&fileHeader, sizeof(BITMAPFILEHEADER), 1, file);
	fread(&infoHeader, sizeof(BITMAPINFOHEADER), 1, file);
	int width = infoHeader.biWidth;
	int height = infoHeader.biHeight;

	int bitsPerPixel = infoHeader.biBitCount;
	int szData = width * height * (bitsPerPixel / 8);

	data = (unsigned char*)malloc(szData);

	if (data == NULL)
	{
		fclose(file);
		return 0;
	}

	// read texture data

	if (fread(data, szData, 1, file) != 1)
	{
		free(data);
		fclose(file);
		return 0;
	}
	fclose(file);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Wrap over edges
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_BGR_EXT, GL_UNSIGNED_BYTE, data);
	free(data);

	return texture;
}
