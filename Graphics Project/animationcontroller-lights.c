/******************************************************************************
 *
 * Computer Graphics Programming 2020 Project Template v1.0 (11/04/2021)
 *
 * Based on: Animation Controller v1.0 (11/04/2021)
 *
 * This template provides a basic FPS-limited render loop for an animated scene,
 * plus keyboard handling for smooth game-like control of an object such as a
 * character or vehicle.
 *
 * A simple static lighting setup is provided via initLights(), which is not
 * included in the animationalcontrol.c template. There are no other changes.
 *
 ******************************************************************************/

#include "includes.h"
#include "helicopter.h"
#include "texture-mapping.h"
#include "obj-loader.h"
#include "world.h"
#include "particles.h"
#include "textfile.h"
#include "glew.h"


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
 * Some Simple Definitions of Motion
 ******************************************************************************/

#define MOTION_NONE 0				// No motion.
#define MOTION_CLOCKWISE -1			// Clockwise rotation.
#define MOTION_ANTICLOCKWISE 1		// Anticlockwise rotation.
#define MOTION_BACKWARD -1			// Backward motion.
#define MOTION_FORWARD 1			// Forward motion.
#define MOTION_LEFT -1				// Leftward motion.
#define MOTION_RIGHT 1				// Rightward motion.
#define MOTION_DOWN -1				// Downward motion.
#define MOTION_UP 1					// Upward motion.

 // Represents the motion of an object on four axes (Yaw, Surge, Sway, and Heave).
 // 
 // You can use any numeric values, as specified in the comments for each axis. However,
 // the MOTION_ definitions offer an easy way to define a "unit" movement without using
 // magic numbers (e.g. instead of setting Surge = 1, you can set Surge = MOTION_FORWARD).
 //
typedef struct {
	int Yaw;		// Turn about the Z axis	[<0 = Clockwise, 0 = Stop, >0 = Anticlockwise]
	int Surge;		// Move forward or back		[<0 = Backward,	0 = Stop, >0 = Forward]
	int Sway;		// Move sideways (strafe)	[<0 = Left, 0 = Stop, >0 = Right]
	int Heave;		// Move vertically			[<0 = Down, 0 = Stop, >0 = Up]
} motionstate4_t;

/******************************************************************************
 * Keyboard Input Handling Setup
 ******************************************************************************/

 // Represents the state of a single keyboard key.Represents the state of a single keyboard key.
typedef enum {
	KEYSTATE_UP = 0,	// Key is not pressed.
	KEYSTATE_DOWN		// Key is pressed down.
} keystate_t;

// Represents the states of a set of keys used to control an object's motion.
typedef struct {
	keystate_t MoveForward;
	keystate_t MoveBackward;
	keystate_t MoveLeft;
	keystate_t MoveRight;
	keystate_t MoveUp;
	keystate_t MoveDown;
	keystate_t TurnLeft;
	keystate_t TurnRight;
} motionkeys_t;

// Current state of all keys used to control our "player-controlled" object's motion.
motionkeys_t motionKeyStates = {
	KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP,
	KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP, KEYSTATE_UP
};

// How our "player-controlled" object should currently be moving, solely based on keyboard input.
//
// Note: this may not represent the actual motion of our object, which could be subject to
// other controls (e.g. mouse input) or other simulated forces (e.g. gravity).
motionstate4_t keyboardMotion = { MOTION_NONE, MOTION_NONE, MOTION_NONE, MOTION_NONE };

// Define all character keys used for input (add any new key definitions here).
// Note: USE ONLY LOWERCASE CHARACTERS HERE. The keyboard handler provided converts all
// characters typed by the user to lowercase, so the SHIFT key is ignored.

#define KEY_MOVE_FORWARD	'w'
#define KEY_MOVE_BACKWARD	's'
#define KEY_MOVE_LEFT		'a'
#define KEY_MOVE_RIGHT		'd'
#define KEY_RENDER_FILL		'l'
#define KEY_OBJTERRAIN		'o'
#define KEY_TOGGLE_FOG		'f'

#define KEY_EXIT			27 // Escape key.

// Define all GLUT special keys used for input (add any new key definitions here).

#define SP_KEY_MOVE_UP		GLUT_KEY_UP
#define SP_KEY_MOVE_DOWN	GLUT_KEY_DOWN
#define SP_KEY_TURN_LEFT	GLUT_KEY_LEFT
#define SP_KEY_TURN_RIGHT	GLUT_KEY_RIGHT

/******************************************************************************
 * GLUT Callback Prototypes
 ******************************************************************************/

void display(void);
void reshape(int width, int h);
void keyPressed(unsigned char key, int x, int y);
void specialKeyPressed(int key, int x, int y);
void keyReleased(unsigned char key, int x, int y);
void specialKeyReleased(int key, int x, int y);
void idle(void);
void mouse(int button, int state, int x, int y);

/******************************************************************************
 * Animation-Specific Function Prototypes (add your own here)
 ******************************************************************************/

void main(int argc, char** argv);
void init(void);
void think(void);
void initLights(void);

void diagnostics();
void basicGround(void);
void setCamera();

void thinkHelicopter();

void printShaderInfoLog(GLuint obj);
void printProgramInfoLog(GLuint obj);
void setShaders(void);

/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/

int windowWidth = 1000;
int windowHeight = 1000;
int worldDimensions[] = { 200, 200, 200 };
Pos3 cameraPosition;
int cameraZoomLevel = 0;
GLdouble orthoLevel = 20.0;
int freeCam = 0;
GLdouble freeCamYaw = 0.0;
GLdouble freeCamPitch = 0.0;
GLdouble camPitch = 45.0f;
int freeCamMouseXStart = 0;
int freeCamMouseYStart = 0;

// Render objects as filled polygons (1) or wireframes (0). Default filled.
int renderFillEnabled = 1;

int objTerrainEnabled = 1;
int fogEnabled = 1;

GLuint g_displayListIndex = 0;

Helicopter heli;
meshObject* zebra_obj;
meshObject* tree_obj;
Terrain terrain[TERRAIN_GRID_LEGNTH][TERRAIN_GRID_LEGNTH];
Terrain objTerrain;
Skybox skybox;
Moon moon;
ParticleSystem pSystem;

GLuint vertexShaderID;
GLuint fragmentShaderID;
GLuint programObjectID;

/******************************************************************************
 * Entry Point (don't put anything except the main function here)
 ******************************************************************************/

void main(int argc, char** argv)
{
	// Initialize the OpenGL window.
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Animation");
	glEnable(GL_DEPTH_TEST);

	// Set up the scene.
	init();

	// Disable key repeat (keyPressed or specialKeyPressed will only be called once when a key is first pressed).
	glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);

	// Register GLUT callbacks.
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyPressed);
	glutSpecialFunc(specialKeyPressed);
	glutKeyboardUpFunc(keyReleased);
	glutSpecialUpFunc(specialKeyReleased);
	glutIdleFunc(idle);
	glutMouseFunc(mouse);
	

	// Record when we started rendering the very first frame (which should happen after we call glutMainLoop).
	frameStartTime = (unsigned int)glutGet(GLUT_ELAPSED_TIME);
	
	glewInit();
	if (glewIsSupported("GL_VERSION_3_0"))
		printf("Ready for OpenGL 3.0\n");
	else
	{
		printf("OpenGL 3.0 not supported\n");
		exit(1);
	}

	setShaders();

	// Enter the main drawing loop (this will never return).
	glutMainLoop();
}

/******************************************************************************
 * GLUT Callbacks (don't add any other functions here)
 ******************************************************************************/

 /*
	 Called when GLUT wants us to (re)draw the current animation frame.

	 Note: This function must not do anything to update the state of our simulated
	 world. Animation (moving or rotating things, responding to keyboard input,
	 etc.) should only be performed within the think() function provided below.
 */
int zebra_tex = 0;
void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	diagnostics();
	setCamera();
	renderFillEnabled ? glPolygonMode(GL_FRONT_AND_BACK, GL_FILL) : glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	fogEnabled ? glEnable(GL_FOG) : glDisable(GL_FOG);

	glUseProgram(programObjectID);
	drawHelicopter(&heli);
	glUseProgram(0);
	

	for (unsigned int i = 1; i <= g_displayListIndex; i++)
	{
		if (!objTerrainEnabled)
		{
			if (i == objTerrain.displayListIndex)
				continue;
		}
		glCallList(i);
	}

	if (!objTerrainEnabled)
	{
		for (int i = 0; i < TERRAIN_GRID_LEGNTH; i++)
		{
			for (int k = 0; k < TERRAIN_GRID_LEGNTH; k++)
			{
				renderGridGround(&terrain[i][k]);
			}
		}
	}

	drawMoon(&moon);
	drawSkybox(&skybox);
	drawParticles(&pSystem);

	glutSwapBuffers();
}

/*
	Called when the OpenGL window has been resized.
*/
void reshape(int width, int h)
{
	windowWidth = width;
	windowHeight = h;
	glViewport(0, 0, width, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	GLdouble aspect = (double)windowWidth / (double)windowHeight;

	glOrtho(-orthoLevel * aspect, orthoLevel * aspect, -orthoLevel, orthoLevel, -orthoLevel * 10.0, 2000.0 * SCALE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is pressed.
*/
void keyPressed(unsigned char key, int x, int y)
{
	switch (tolower(key)) {

		/*
			Keyboard-Controlled Motion Handler - DON'T CHANGE THIS SECTION

			Whenever one of our movement keys is pressed, we do two things:
			(1) Update motionKeyStates to record that the key is held down. We use
				this later in the keyReleased callback.
			(2) Update the relevant axis in keyboardMotion to set the new direction
				we should be moving in. The most recent key always "wins" (e.g. if
				you're holding down KEY_MOVE_LEFT then also pressed KEY_MOVE_RIGHT,
				our object will immediately start moving right).
		*/
	case KEY_MOVE_FORWARD:
		motionKeyStates.MoveForward = KEYSTATE_DOWN;
		keyboardMotion.Surge = MOTION_FORWARD;
		break;
	case KEY_MOVE_BACKWARD:
		motionKeyStates.MoveBackward = KEYSTATE_DOWN;
		keyboardMotion.Surge = MOTION_BACKWARD;
		break;
	case KEY_MOVE_LEFT:
		motionKeyStates.MoveLeft = KEYSTATE_DOWN;
		keyboardMotion.Sway = MOTION_LEFT;
		break;
	case KEY_MOVE_RIGHT:
		motionKeyStates.MoveRight = KEYSTATE_DOWN;
		keyboardMotion.Sway = MOTION_RIGHT;
		break;

		/*
			Other Keyboard Functions (add any new character key controls here)

			Rather than using literals (e.g. "t" for spotlight), create a new KEY_
			definition in the "Keyboard Input Handling Setup" section of this file.
			For example, refer to the existing keys used here (KEY_MOVE_FORWARD,
			KEY_MOVE_LEFT, KEY_EXIT, etc).
		*/
	case KEY_RENDER_FILL:
		renderFillEnabled = !renderFillEnabled;
		break;
	case KEY_OBJTERRAIN:
		objTerrainEnabled = !objTerrainEnabled;
		break;
	case KEY_TOGGLE_FOG:
		fogEnabled = !fogEnabled;
		break;
	case KEY_EXIT:
		exit(0);
		break;
	}
}

/*
	Called each time a "special" key (e.g. an arrow key) is pressed.
*/
void specialKeyPressed(int key, int x, int y)
{
	switch (key) {

		/*
			Keyboard-Controlled Motion Handler - DON'T CHANGE THIS SECTION

			This works as per the motion keys in keyPressed.
		*/
	case SP_KEY_MOVE_UP:
		motionKeyStates.MoveUp = KEYSTATE_DOWN;
		keyboardMotion.Heave = MOTION_UP;
		break;
	case SP_KEY_MOVE_DOWN:
		motionKeyStates.MoveDown = KEYSTATE_DOWN;
		keyboardMotion.Heave = MOTION_DOWN;
		break;
	case SP_KEY_TURN_LEFT:
		motionKeyStates.TurnLeft = KEYSTATE_DOWN;
		keyboardMotion.Yaw = MOTION_ANTICLOCKWISE;
		break;
	case SP_KEY_TURN_RIGHT:
		motionKeyStates.TurnRight = KEYSTATE_DOWN;
		keyboardMotion.Yaw = MOTION_CLOCKWISE;
		break;

		/*
			Other Keyboard Functions (add any new special key controls here)

			Rather than directly using the GLUT constants (e.g. GLUT_KEY_F1), create
			a new SP_KEY_ definition in the "Keyboard Input Handling Setup" section of
			this file. For example, refer to the existing keys used here (SP_KEY_MOVE_UP,
			SP_KEY_TURN_LEFT, etc).
		*/
	}
}

/*
	Called each time a character key (e.g. a letter, number, or symbol) is released.
*/
void keyReleased(unsigned char key, int x, int y)
{
	switch (tolower(key)) {

		/*
			Keyboard-Controlled Motion Handler - DON'T CHANGE THIS SECTION

			Whenever one of our movement keys is released, we do two things:
			(1) Update motionKeyStates to record that the key is no longer held down;
				we need to know when we get to step (2) below.
			(2) Update the relevant axis in keyboardMotion to set the new direction
				we should be moving in. This gets a little complicated to ensure
				the controls work smoothly. When the user releases a key that moves
				in one direction (e.g. KEY_MOVE_RIGHT), we check if its "opposite"
				key (e.g. KEY_MOVE_LEFT) is pressed down. If it is, we begin moving
				in that direction instead. Otherwise, we just stop moving.
		*/
	case KEY_MOVE_FORWARD:
		motionKeyStates.MoveForward = KEYSTATE_UP;
		keyboardMotion.Surge = (motionKeyStates.MoveBackward == KEYSTATE_DOWN) ? MOTION_BACKWARD : MOTION_NONE;
		break;
	case KEY_MOVE_BACKWARD:
		motionKeyStates.MoveBackward = KEYSTATE_UP;
		keyboardMotion.Surge = (motionKeyStates.MoveForward == KEYSTATE_DOWN) ? MOTION_FORWARD : MOTION_NONE;
		break;
	case KEY_MOVE_LEFT:
		motionKeyStates.MoveLeft = KEYSTATE_UP;
		keyboardMotion.Sway = (motionKeyStates.MoveRight == KEYSTATE_DOWN) ? MOTION_RIGHT : MOTION_NONE;
		break;
	case KEY_MOVE_RIGHT:
		motionKeyStates.MoveRight = KEYSTATE_UP;
		keyboardMotion.Sway = (motionKeyStates.MoveLeft == KEYSTATE_DOWN) ? MOTION_LEFT : MOTION_NONE;
		break;

		/*
			Other Keyboard Functions (add any new character key controls here)

			Note: If you only care when your key is first pressed down, you don't have to
			add anything here. You only need to put something in keyReleased if you care
			what happens when the user lets go, like we do with our movement keys above.
			For example: if you wanted a spotlight to come on while you held down "t", you
			would need to set a flag to turn the spotlight on in keyPressed, and update the
			flag to turn it off in keyReleased.
		*/
	}
}

/*
	Called each time a "special" key (e.g. an arrow key) is released.
*/
void specialKeyReleased(int key, int x, int y)
{
	switch (key) {
		/*
			Keyboard-Controlled Motion Handler - DON'T CHANGE THIS SECTION

			This works as per the motion keys in keyReleased.
		*/
	case SP_KEY_MOVE_UP:
		motionKeyStates.MoveUp = KEYSTATE_UP;
		keyboardMotion.Heave = (motionKeyStates.MoveDown == KEYSTATE_DOWN) ? MOTION_DOWN : MOTION_NONE;
		break;
	case SP_KEY_MOVE_DOWN:
		motionKeyStates.MoveDown = KEYSTATE_UP;
		keyboardMotion.Heave = (motionKeyStates.MoveUp == KEYSTATE_DOWN) ? MOTION_UP : MOTION_NONE;
		break;
	case SP_KEY_TURN_LEFT:
		motionKeyStates.TurnLeft = KEYSTATE_UP;
		keyboardMotion.Yaw = (motionKeyStates.TurnRight == KEYSTATE_DOWN) ? MOTION_CLOCKWISE : MOTION_NONE;
		break;
	case SP_KEY_TURN_RIGHT:
		motionKeyStates.TurnRight = KEYSTATE_UP;
		keyboardMotion.Yaw = (motionKeyStates.TurnLeft == KEYSTATE_DOWN) ? MOTION_ANTICLOCKWISE : MOTION_NONE;
		break;

		/*
			Other Keyboard Functions (add any new special key controls here)

			As per keyReleased, you only need to handle the key here if you want something
			to happen when the user lets go. If you just want something to happen when the
			key is first pressed, add you code to specialKeyPressed instead.
		*/
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
	// Get uniform locations
	GLint lightPosLoc = glGetUniformLocation(programObjectID, "lightPos");

	// Set uniform values
	GLfloat lightPos[] = {moon.coordinates.x, moon.coordinates.y, moon.coordinates.z};
	glUniform3fv(lightPosLoc, 1, lightPos);

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


/* https://stackoverflow.com/questions/14378/using-the-mouse-scrollwheel-in-glut */
void mouse(int button, int state, int x, int y)
{
	GLdouble aspect = (double)windowWidth / (double)windowHeight;
	if (button == 3 || button == 4)
	{
		if (button == 3 && state == GLUT_UP)
			if (orthoLevel > 10.0) orthoLevel -= 10.0;
		if (button == 4 && state == GLUT_UP)
			if (orthoLevel < 1000.0) orthoLevel += 10.0;

		glViewport(0, 0, windowWidth, windowHeight);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-orthoLevel * aspect, orthoLevel * aspect, -orthoLevel, orthoLevel, -orthoLevel * 10.0, 2000.0 * SCALE);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
	}

	if (button == 1 && state == GLUT_DOWN)
	{
		freeCam = 1;
	}
	else if (button == 1 && state == GLUT_UP)
	{
		freeCam = 0;
		freeCamPitch = 0.0;
		freeCamYaw = 0;
	}
}

/******************************************************************************
 * Animation-Specific Functions (Add your own functions at the end of this section)
 ******************************************************************************/

 /*
	 Initialise OpenGL and set up our scene before we begin the render loop.
 */



void init(void)
{
	// Setting random seed
	srand(time(NULL));

	glClearColor(0.0, 0.0, 0.0, 1.0); // Set the background color to black
	glMatrixMode(GL_PROJECTION); // Select the projection matrix
	glLoadIdentity(); // Load the identity matrix
	
	glEnable(GL_FOG);
	GLfloat fogColor[4] = { 0.8, 0.4, 0.0, 1.0 }; // orange
	glFogi(GL_FOG_MODE, GL_EXP);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.002 / SCALE);

	glMatrixMode(GL_MODELVIEW); // Select the model view matrix
	glLoadIdentity(); // Load the identity matrix

	// Anything that relies on lighting or specifies normals must be initialised after initLights.
	initHelicopter(&heli);

	initParticleSystem(&pSystem);
	//zebra_obj = loadMeshObject("zebra.obj", "zebra.mtl");
	//tree_obj = loadMeshObject("Palm_Tree.obj", "Palm_Tree.mtl");
	//terrain = loadMeshObject("untitled.obj", "untitled.mtl");
	//initTerrain(&terrain);
	//tree_obj = loadMeshObject("Palm_Tree.obj", NULL);

	initTerrain(&objTerrain, TERRAIN_GRID_SIZE * SCALE * TERRAIN_GRID_LEGNTH / 2, TERRAIN_GRID_SIZE * SCALE * TERRAIN_GRID_LEGNTH / 2);

	GLdouble terrainSize = TERRAIN_GRID_SIZE * SCALE;

	int x = terrainSize / 2.0;
	int z = terrainSize / 2.0;

	for (int i = 0; i < TERRAIN_GRID_LEGNTH; i++)
	{
		for (int k = 0; k < TERRAIN_GRID_LEGNTH; k++)
		{
			initGridTerrain(&terrain[i][k], x, z);
			z += terrainSize;
		}
		z = terrainSize / 2.0;
		x += terrainSize;
	}


	initMoon(&moon);
	initSkybox(&skybox);
	initLights();
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
	thinkHelicopter();
	//int i = (heli.coordinates.x) / terrain[0][0].postSize;
	//int k = (heli.coordinates.z) / terrain[0][0].postSize;
	//Terrain* terrain_t = &terrain[i][k];
	createParticleInRandomRadius(&pSystem, &objTerrain, &heli.coordinates, heli.size * 200);
	thinkParticles(&pSystem, &objTerrain);

}

/*
	Initialise OpenGL lighting before we begin the render loop.

	Note (advanced): If you're using dynamic lighting (e.g. lights that move around, turn on or
	off, or change colour) you may want to replace this with a drawLights function that gets called
	at the beginning of display() instead of init().
*/
void initLights(void)
{
	// Define the light color and intensity
	GLfloat ambientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	GLfloat diffuseLight[] = { 0.8f, 0.8f, 0.2f, 1.0f };    // Orange-ish diffuse light
	GLfloat specularLight[] = { 1.0f, 0.5f, 0.2f, 1.0f };   // White specular light

	// Set the global ambient light level
	GLfloat globalAmbientLight[] = { 0.1f, 0.1f, 0.1f, 1.0f }; // Dimmed ambient light


	// set the global ambient light level
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);
	// define the color and intensity for light 0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	// enable lighting
	glEnable(GL_LIGHTING);
	// enable light 0
	glEnable(GL_LIGHT0);

	// set light position
	GLfloat lightPosition[] = { moon.coordinates.x, moon.coordinates.y, moon.coordinates.z, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

	// turn on depth testing so that polygons are drawn in the correct order
	glEnable(GL_DEPTH_TEST);
	// make sure the normals are unit vectors
	glEnable(GL_NORMALIZE);
}

/******************************************************************************/

void diagnostics()
{
	glPushMatrix();

	glLineWidth(5);
	char heliX[200];
	char heliY[200];
	char heliZ[200];

	sprintf_s(heliX, 200, "x : %.2f", heli.coordinates.x);
	sprintf_s(heliY, 200, "y : %.2f", heli.coordinates.y);
	sprintf_s(heliZ, 200, "z : %.2f", heli.coordinates.z);
	GLdouble aspect = (double)windowWidth / (double)windowHeight;

	glTranslated(-orthoLevel * aspect, orthoLevel, 0);
	glRasterPos2d(orthoLevel / 20.0, -orthoLevel / 20.0);
	glutBitmapString(GLUT_BITMAP_8_BY_13, heliX);
	glRasterPos2d(orthoLevel / 20.0, -orthoLevel / 20.0 * 2.0);
	glutBitmapString(GLUT_BITMAP_8_BY_13, heliY);
	glRasterPos2d(orthoLevel / 20.0, -orthoLevel / 20.0 * 3.0);
	glutBitmapString(GLUT_BITMAP_8_BY_13, heliZ);

	if (checkCollisionSkybox(&skybox, &heli.coordinates))
	{
		// Print a "OUT OF BOUNDS" Message
		glRasterPos2d(orthoLevel / 20.0, -orthoLevel / 20.0 * 4);
		glutBitmapString(GLUT_BITMAP_8_BY_13, "OUT OF BOUNDS");
		glRasterPos2d(orthoLevel / 20.0, -orthoLevel / 20.0 * 5);
		glutBitmapString(GLUT_BITMAP_8_BY_13, "Press SPACE to reset");
	}

	glPopMatrix();
}

void setCamera()
{
	GLdouble eye_x, eye_y, eye_z;
	if (!freeCam)
	{
		eye_x = heli.coordinates.x + sin(heli.yaw * PI / 180.0) * 10.0;
		//eye_y = (heli.coordinates.y + heli.size * 0.2) + sin(heli.pitch * PI / 180.0) * 3.0;
		eye_y = (heli.coordinates.y + heli.size * 0.2) + sin(camPitch * PI / 180.0) * 3.0;

		//eye_y = (heli.coordinates.y + heli.size * 0.2) + sin(45.0 * PI / 180.0) * 3.0;
		eye_z = (heli.coordinates.z) + cos((heli.yaw + 180) * PI / 180.0) * 10.0;
	}
	else
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		if (freeCamPitch == 0 && freeCamYaw == 0)
		{
			freeCamYaw = heli.yaw;
			freeCamPitch = camPitch;
			freeCamMouseXStart = cursorPos.x;
			freeCamMouseYStart = cursorPos.y;
		}
		freeCamYaw -= (freeCamMouseXStart - cursorPos.x) / 360.0;
		freeCamPitch += (freeCamMouseYStart - cursorPos.y) / 360.0;
		camPitch = freeCamPitch;
		eye_x = heli.coordinates.x + sin(freeCamYaw * PI / 180.0) * 10.0;
		eye_y = (heli.coordinates.y + heli.size * 0.2) + sin(freeCamPitch * PI / 180.0) * 3.0;
		eye_z = (heli.coordinates.z) + cos((freeCamYaw + 180) * PI / 180.0) * 10.0;

		// Free cam slowdown
		if (cursorPos.x > freeCamMouseXStart)
		{
			freeCamMouseXStart += (int)fabsf(cursorPos.x - freeCamMouseXStart) * 0.02f;
		}
		else
		{
			freeCamMouseXStart -= (int)fabsf(cursorPos.x - freeCamMouseXStart) * 0.02f;
		}
		if (cursorPos.y > freeCamMouseYStart)
		{
			freeCamMouseYStart += (int)fabsf(cursorPos.y - freeCamMouseYStart) * 0.02f;
		}
		else
		{
			freeCamMouseYStart -= (int)fabsf(cursorPos.y - freeCamMouseYStart) * 0.02f;
		}
	}

	gluLookAt(
		eye_x,
		eye_y,
		eye_z,
		heli.coordinates.x,
		heli.coordinates.y,
		heli.coordinates.z,
		0.0,
		1.0,
		0.0
	);

	cameraPosition.x = (float)eye_x;
	cameraPosition.y = (float)eye_y;
	cameraPosition.z = (float)eye_z;
}

void thinkHelicopterCollision(int* collidedWithTerrain, int* collidedWithSkybox)
{
	if (getCollisionTerrain(&objTerrain, &heli.coordinates, &heli.collisionBox))
	{
		*collidedWithTerrain = 1;

		// Degrade angles and speed quickly
		heli.pitch *= (1.0 - 0.1);
		heli.velocity *= (1.0 - 1);
		heli.roll *= (1.0 - 0.1);
		heli.strafeVelocity *= (1.0 - 1);

		if (keyboardMotion.Heave <= 0)
		{
			// Slow down rotors
			heli.rotorVelocity -= 0.05;
		}
		else
		{
			// Speed up rotors
			heli.rotorVelocity += 0.05;
		}

		// Prevent negative rotor spin.
		heli.rotorVelocity = max(heli.rotorVelocity, 0.0);

		//heli.coordinates.y = k + heli.collisionBox.y / 2;
	}
	else
	{
		// Speed up rotors
		heli.rotorVelocity += 0.05;
		// Rotor maxium value of 25.0
		heli.rotorVelocity = min(heli.rotorVelocity, 25.0);
	}

	if (checkCollisionSkybox(&skybox, &heli.coordinates))
	{
		*collidedWithSkybox = 1;

		// Degrade angles and speed quickly
		heli.pitch *= (1.0 - 0.1);
		heli.velocity *= (1.0 - 1);
		heli.roll *= (1.0 - 0.1);
		heli.strafeVelocity *= (1.0 - 1);
	}
}

void thinkHelicopter()
{
	int collidedWithTerrain = 0;
	int collidedWithSkybox = 0;

	thinkHelicopterCollision(&collidedWithTerrain, &collidedWithSkybox);

	// Spin / rotate helicopter
	if (keyboardMotion.Yaw != MOTION_NONE && !collidedWithTerrain) {
		heli.yaw -= keyboardMotion.Yaw / 2.0;

		if (heli.yaw >= 360.0) heli.yaw = 0.0;
		if (heli.yaw < 0.0) heli.yaw = 360.0;
	}

	// Forward / backward velocity
	if (keyboardMotion.Surge != MOTION_NONE && !collidedWithTerrain) {
		if (heli.velocity <= heli.maxSpeed && heli.velocity >= -heli.maxSpeed)
			heli.velocity += keyboardMotion.Surge * FRAME_TIME_SEC;

		heli.direction = (int)heli.yaw;

		if (keyboardMotion.Surge > 0 && heli.pitch < 25.0) heli.pitch += keyboardMotion.Surge;
		if (keyboardMotion.Surge < 0 && heli.pitch > -25.0) heli.pitch += keyboardMotion.Surge;
	}
	else
	{
		// No velocity, drag to stop
		heli.pitch *= (1.0 - 0.1);
		heli.velocity *= (1.0 - 0.01);
	}

	// Side / strafe velocity
	if (keyboardMotion.Sway != MOTION_NONE && !collidedWithTerrain) {

		if (heli.strafeVelocity <= heli.maxSpeed && heli.strafeVelocity >= -heli.maxSpeed)
			heli.strafeVelocity += keyboardMotion.Sway * FRAME_TIME_SEC;

		heli.direction = (int)heli.yaw;

		if (keyboardMotion.Sway > 0 && heli.roll < 25.0) heli.roll += keyboardMotion.Sway;
		if (keyboardMotion.Sway < 0 && heli.roll > -25.0) heli.roll += keyboardMotion.Sway;
	}
	else
	{
		// No velocity, drag to stop
		heli.roll *= (1.0 - 0.1);
		heli.strafeVelocity *= (1.0 - 0.01);
	}

	// Vertical velocity
	if (keyboardMotion.Heave != MOTION_NONE) {
		if (heli.rotorVelocity > 5.0)
		{
			// Maximum lift velocity
			if (heli.liftVelocity <= heli.maxSpeed / 2.0 && heli.liftVelocity >= -heli.maxSpeed / 2.0)
				heli.liftVelocity += keyboardMotion.Heave * FRAME_TIME_SEC;

			// Don't allow going above skybox
			if (collidedWithSkybox && keyboardMotion.Heave > 0)
				heli.liftVelocity = 0.0;

			// Don't allow going below terrain
			if (collidedWithTerrain && keyboardMotion.Heave < 0)
				heli.liftVelocity = 0.0;
		}
	}
	else
	{
		// No velocity, drag to stop
		heli.liftVelocity *= (1.0 - 0.1);
	}

	// Update heli coordinates
	heli.coordinates.x -= sin(heli.direction * PI / 180.0f) * (float)heli.velocity;
	heli.coordinates.z += cos(heli.direction * PI / 180.0f) * (float)heli.velocity;

	heli.coordinates.x -= cos(heli.direction * PI / 180.0f) * (float)heli.strafeVelocity;
	heli.coordinates.z -= sin(heli.direction * PI / 180.0f) * (float)heli.strafeVelocity;

	heli.coordinates.y += (float)heli.liftVelocity;

	//heli.coordinates.x = CLAMP(heli.coordinates.x, 100, 0);

	heli.rotorRotation += heli.rotorVelocity;
	if (heli.rotorRotation > 360.0) heli.rotorRotation = 0.0;
}

void cleanUp()
{
	freeTerrain();
}

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
void setShaders(void)
{
	char* vertexShaderSource = NULL;
	char* fragmentShaderSource = NULL;

	// create the openGL shaders
	vertexShaderID = glCreateShader(GL_VERTEX_SHADER);
	fragmentShaderID = glCreateShader(GL_FRAGMENT_SHADER);

	// read in the shader text from files
	vertexShaderSource = textFileRead("shaders/heliShader.vert");
	fragmentShaderSource = textFileRead("shaders/heliShader.frag");
	//vertexShaderSource = textFileRead("DesertTerrain.vert");
	//fragmentShaderSource = textFileRead("DesertTerrain.frag");

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

	// use this program object (and the shaders attached to it)
	//glUseProgram(programObjectID);
}