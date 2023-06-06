#include "shader-loader.h"

/*
   Prints any errors from shader compilation
*/
void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

/*
	Prints any errors from program linking
*/
void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char* infoLog;

	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);

	if (infologLength > 0)
	{
		infoLog = (char*)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("%s\n", infoLog);
		free(infoLog);
	}
}

/*
	Reads in shaders compiles them and then attaches them to a newly created program object.
	This program object is then linked to this application.
*/
GLuint setShaders(const char* vertFilePath, const char* fragFilePath)
{
	GLuint programObjectID = 0;
	char* vertexShaderSource = NULL;
	char* fragmentShaderSource = NULL;

	// create the openGL shaders
	GLuint vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// read in the shader text from files
	vertexShaderSource = textFileRead(vertFilePath);
	fragmentShaderSource = textFileRead(fragFilePath);

	// pass in the shader strings
	glShaderSource(vertexShaderID, 1, &vertexShaderSource, NULL);
	glShaderSource(fragmentShaderID, 1, &fragmentShaderSource, NULL);

	// release the files
	free(vertexShaderSource);
	free(fragmentShaderSource);

	// compile the shaders
	glCompileShader(vertexShaderID);
	glCompileShader(fragmentShaderID);

	// print out any errors from the shader compilations
	printShaderInfoLog(vertexShaderID);
	printShaderInfoLog(fragmentShaderID);

	// create a program object to contain the shaders
	programObjectID = glCreateProgram();

	// attach the shaders to the program object
	glAttachShader(programObjectID, vertexShaderID);
	glAttachShader(programObjectID, fragmentShaderID);

	// link the program object to this application
	glLinkProgram(programObjectID);

	// print any errors from the linking
	printProgramInfoLog(programObjectID);

	return programObjectID;
}