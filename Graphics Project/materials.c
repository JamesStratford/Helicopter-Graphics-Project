/******************************************************************************
 *
 * Materials v1.0 (23/02/2021)
 *
 * This code provides a program to demo using materials and lighting in OpenGL
 *
 ******************************************************************************/

#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include <stdio.h>


 /******************************************************************************
  * Animation & Timing Setup
  ******************************************************************************/

  // Target frame rate (number of Frames Per Second).
#define TARGET_FPS 60				

// Ideal time each frame should be displayed for (in milliseconds).
const unsigned int FRAME_TIME = 1000 / TARGET_FPS;

// Frame time in fractional seconds.
// Note: This is calculated to accurately reflect the truncated integer value of
// FRAME_TIME, which is used for timing, rather than the more accurate fractional
// value we'd get if we simply calculated "FRAME_TIME_SEC = 1.0f / TARGET_FPS".
const float FRAME_TIME_SEC = (1000 / TARGET_FPS) / 1000.0f;

// Time we started preparing the current frame (in milliseconds since GLUT was initialized).
unsigned int frameStartTime = 0;

/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

 // Define all character keys used for input (add any new key definitions here).
 // Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
 // characters typed by the user to lowercase, so the SHIFT key is ignored.

#define KEY_EXIT			27 // Escape key.
#define KEY_SMOOTH_TOGGLE	's' // toggle between smooth and flat shading

/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void idle(void);

/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/

void main(int argc, char **argv);
void init(void);
void think(void);


/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/
 // current camera position
GLfloat cameraPosition[] = {0, 0, 7};

// window dimensions
GLint windowWidth = 800;
GLint windowHeight = 400;

// are we using smooth shading or flat?
int smoothOn = 1;

//  position the light source 
GLfloat lightPosition[] = { 10.0, 10.0, 10.0, 1.0 };

// a material that is all zeros
GLfloat zeroMaterial[] = { 0.0, 0.0, 0.0, 1.0 };

// a red ambient material
GLfloat redAmbient[] = { 0.5, 0.0, 0.0, 1.0 };

// a blue diffuse material
GLfloat blueDiffuse[] = { 0.1f, 0.5f, 0.8f, 1.0f };

// a red diffuse material
GLfloat redDiffuse[] = { 1.0, 0.0, 0.0, 1.0 };

// a white specular material
GLfloat whiteSpecular[] = { 1.0, 1.0, 1.0, 1.0 };

// the degrees of shinnines (size of the specular highlight, bigger number means smaller highlight)
GLfloat noShininess = 0.0;
GLfloat highShininess = 100.0;


 /******************************************************************************
  * Entry Point (don't put anything except the main function here)
  ******************************************************************************/

void main(int argc, char **argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	// set window position on screen
	glutInitWindowPosition(100, 100);

	glutCreateWindow("Materials");

	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutIdleFunc(idle);

	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);

	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}

/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/

 /*
	 Called when GLUT wants us to (re)draw the current animation frame.
 */
void display(void)
{
	// sphere resolution
	int resolution = 50;

	if (smoothOn)
		glShadeModel(GL_SMOOTH);
	else
		glShadeModel(GL_FLAT);

	// clear the screen and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// load the identity matrix into the model view matrix
	glLoadIdentity();

	// set the camera position so we can "see" the cube drawn at (0,0,0)
	gluLookAt(cameraPosition[0], cameraPosition[1], cameraPosition[2],
		0, 0, 0,
		0, 1, 0);

	// position light 0
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);


	// draw the left sphere, blue with no hightlight 
	glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blueDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, noShininess);

	glPushMatrix();
	glTranslatef(-3.75, 0, 0);
	// glutSolidSphere(radius, slices - lines of longitude, stacks - lines of latitude);
	glutSolidSphere(1.0, resolution, resolution);  // glutSolidSphere is a convenience function that sets up a gluSphere,  
												  //     and...automatically computes normals for us
	glPopMatrix();

	// draw the middle sphere, blue with a white highlight and no ambient
	glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blueDiffuse); //whiteTranslucent);
	glMaterialfv(GL_FRONT, GL_SPECULAR, whiteSpecular);
	glMaterialf(GL_FRONT, GL_SHININESS, highShininess);
	glutSolidSphere(1.0, resolution, resolution);		  

	// draw the right sphere, blue with red ambient
	glMaterialfv(GL_FRONT, GL_AMBIENT, redAmbient);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, blueDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, noShininess);

	glPushMatrix();
	glTranslatef(3.75, 0, 0);
	glutSolidSphere(1.0, resolution, resolution);   
	glPopMatrix();

	// draw a red floor
	glMaterialfv(GL_FRONT, GL_AMBIENT, zeroMaterial);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, redDiffuse);
	glMaterialfv(GL_FRONT, GL_SPECULAR, zeroMaterial);
	glMaterialf(GL_FRONT, GL_SHININESS, noShininess);

	glNormal3d(0, 1, 0);  // normal of the floor is pointing up

	glPushMatrix();
	glTranslatef(1.5, -1, 0);
	glScalef(3, 0, 3);
	glBegin(GL_POLYGON);
	glVertex3f(0, 0, 0);
	glVertex3f(0, 0, 1);
	glVertex3f(1, 0, 1);
	glVertex3f(1, 0, 0);
	glEnd();
	glPopMatrix();

	// swap the drawing buffers
	glutSwapBuffers();
}

/*
	Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{
	// update the new width
	windowWidth = width;
	// update the new height
	windowHeight = h;

	// update the viewport to still be all of the window
	glViewport(0, 0, windowWidth, windowHeight);

	// change into projection mode so that we can change the camera properties
	glMatrixMode(GL_PROJECTION);

	// load the identity matrix into the projection matrix
	glLoadIdentity();

	// gluPerspective(fovy, aspect, near, far)
	gluPerspective(45, (float)windowWidth / (float)windowHeight, 1, 20);

	// change into model-view mode so that we can change the object positions
	glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {
	case KEY_SMOOTH_TOGGLE:
		smoothOn = !smoothOn;
		break;
	case KEY_EXIT:
		exit(0);
		break;
	}
}

/*
	Called by GLUT when it's not rendering a frame.

	Note: We use this to handle animation and timing. You shouldn't need to modify
	this callback at all. Instead, place your animation logic (e.g. moving or rotating
	things) within the think() method provided with this template.
*/
void idle(void)
{
	// Wait until it's time to render the next frame.

	unsigned int frameTimeElapsed = (unsigned int)glutGet(GLUT_ELAPSED_TIME) - frameStartTime;
	if (frameTimeElapsed < FRAME_TIME)
	{
		// This frame took less time to render than the ideal FRAME_TIME: we'll suspend this thread for the remaining time,
		// so we're not taking up the CPU until we need to render another frame.
		unsigned int timeLeft = FRAME_TIME - frameTimeElapsed;
		Sleep(timeLeft);
	}

	// Begin processing the next frame.

	frameStartTime = glutGet(GLUT_ELAPSED_TIME); // Record when we started work on the new frame.

	think(); // Update our simulated world before the next call to display().

	glutPostRedisplay(); // Tell OpenGL there's a new frame ready to be drawn.
}

/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

 /*
	 Initialise OpenGL and set up our scene before we begin the render loop.
 */
void init(void)
{
	// define the light color and intensity
	GLfloat ambientLight[] = { 0.0, 0.0, 0.0, 1.0 };  // relying on global ambient
	GLfloat diffuseLight[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };

	//  the global ambient light level
	GLfloat globalAmbientLight[] = { 0.4f, 0.4f, 0.4f, 1.0f };

	// set the global ambient light level
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);

	// define the color and intensity for light 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, diffuseLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, specularLight);

	// enable lighting 
	glEnable(GL_LIGHTING);
	// enable light 0
	glEnable(GL_LIGHT0);


	// turn on depth testing so that polygons are drawn in the correct order
	glEnable(GL_DEPTH_TEST);

	// make sure the normals are unit vectors
	glEnable(GL_NORMALIZE);

}

/*
	Advance our animation by FRAME_TIME milliseconds.

	Note: Our template's GLUT idle() callback calls this once before each new
	frame is drawn, EXCEPT the very first frame drawn after our application
	starts. Any setup required before the first frame is drawn should be placed
	in init().
*/
void think(void)
{
	//no animation in this demo

}




/******************************************************************************/