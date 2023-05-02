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

#include <stdio.h>
#include <Windows.h>
#include <freeglut.h>
#include <math.h>
#include "helicopter.h"
#include "texture-mapping.h"
#include "tex-obj-loader.h"

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

int checkCollision(Pos3 positionOne, Pos3 collisionBoxOne, Pos3 positionTwo, Pos3 collisionBoxTwo);

void thinkHelicopter();
void drawFog();



/******************************************************************************
 * Animation-Specific Setup (Add your own definitions, constants, and globals here)
 ******************************************************************************/

int windowWidth = 1000;
int windowHeight = 1000;
int worldDimensions[] = { 200, 200, 200 };
Pos3 cameraPosition;
int cameraZoomLevel = 0;
GLdouble orthoLevel = 50.0;
int freeCam = 0;
GLdouble freeCamYaw = 0.0;
GLdouble freeCamPitch = 0.0;
int freeCamMouseXStart = 0;
int freeCamMouseYStart = 0;

// Render objects as filled polygons (1) or wireframes (0). Default filled.
int renderFillEnabled = 1;

Helicopter heli;
meshObject* zebra_obj;
meshObject* tree_obj;

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

void display(void)
{
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR DRAWING CODE

		Separate reusable pieces of drawing code into functions, which you can add
		to the "Animation-Specific Functions" section below.

		Remember to add prototypes for any new functions to the "Animation-Specific
		Function Prototypes" section near the top of this template.
	*/
	glShadeModel(GL_SMOOTH);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	diagnostics();
	setCamera();


	drawHelicopter(&heli);
	//drawFog();
	basicGround();

	glPushMatrix();
	glTranslated(0, 10, 0);
	glScaled(10, 10, 10);
	//renderMeshObject(zebra_obj);

	renderMeshObject(tree_obj);
	glPopMatrix();

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

	glOrtho(-orthoLevel * aspect, orthoLevel * aspect, -orthoLevel, orthoLevel, -orthoLevel * 10.0, max(orthoLevel * 10.0, 200.0));

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
			if (orthoLevel > 1.0) orthoLevel -= 10.0;
		if (button == 4 && state == GLUT_UP)
			if (orthoLevel < 1000.0) orthoLevel += 10.0;

		glViewport(0, 0, windowWidth, windowHeight);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-orthoLevel * aspect, orthoLevel * aspect, -orthoLevel, orthoLevel, -orthoLevel * 10.0, max(orthoLevel * 10.0, 200.0));
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
		freeCamPitch = 0;
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
	// srand(time(NULL));

	glClearColor(0.0, 0.0, 0.0, 1.0); // Set the background color to black
	glMatrixMode(GL_PROJECTION); // Select the projection matrix
	glLoadIdentity(); // Load the identity matrix
	//glOrtho(-30.0, 30.0, -30.0, 30.0, 0.1, 5000.0); // Set the orthographic projection
	//glEnable(GL_FOG);

	glMatrixMode(GL_MODELVIEW); // Select the model view matrix
	glLoadIdentity(); // Load the identity matrix

	initLights();


	// Anything that relies on lighting or specifies normals must be initialised after initLights.
	initHelicopter(&heli);
	zebra_obj = loadMeshObject("zebra.obj", "zebra.mtl");
	tree_obj = loadMeshObject("Palm_Tree.obj", "Palm_Tree.mtl");
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
	/*
		TEMPLATE: REPLACE THIS COMMENT WITH YOUR ANIMATION/SIMULATION CODE

		In this function, we update all the variables that control the animated
		parts of our simulated world. For example: if you have a moving box, this is
		where you update its coordinates to make it move. If you have something that
		spins around, here's where you update its angle.

		NOTHING CAN BE DRAWN IN HERE: you can only update the variables that control
		how everything will be drawn later in display().

		How much do we move or rotate things? Because we use a fixed frame rate, we
		assume there's always FRAME_TIME milliseconds between drawing each frame. So,
		every time think() is called, we need to work out how far things should have
		moved, rotated, or otherwise changed in that period of time.

		Movement example:
		* Let's assume a distance of 1.0 GL units is 1 metre.
		* Let's assume we want something to move 2 metres per second on the x axis
		* Each frame, we'd need to update its position like this:
			x += 2 * (FRAME_TIME / 1000.0f)
		* Note that we have to convert FRAME_TIME to seconds. We can skip this by
		  using a constant defined earlier in this template:
			x += 2 * FRAME_TIME_SEC;

		Rotation example:
		* Let's assume we want something to do one complete 360-degree rotation every
		  second (i.e. 60 Revolutions Per Minute, or RPM).
		* Each frame, we'd need to update our object's angle like this (we'll use the
		  FRAME_TIME_SEC constant as per the example above):
			a += 360 * FRAME_TIME_SEC;

		This works for any type of "per second" change: just multiply the amount you'd
		want to move in a full second by FRAME_TIME_SEC, and add or subtract that
		from whatever variable you're updating.

		You can use this same approach to animate other things like color, opacity,
		brightness of lights, etc.
	*/

	/*
		Keyboard motion handler: complete this section to make your "player-controlled"
		object respond to keyboard input.
	*/
	thinkHelicopter();
}

/*
	Initialise OpenGL lighting before we begin the render loop.

	Note (advanced): If you're using dynamic lighting (e.g. lights that move around, turn on or
	off, or change colour) you may want to replace this with a drawLights function that gets called
	at the beginning of display() instead of init().
*/
void initLights(void)
{
	// define the light color and intensity
	GLfloat ambientLight[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat diffuseLight[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 };
	// the global ambient light level
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

/******************************************************************************/

void diagnostics()
{
	glPushMatrix();
	//glColor3f(1.0, 0.0, 0.0);
	glLineWidth(5);
	char heliX[200];
	char heliY[200];
	char heliZ[200];

	sprintf_s(heliX, 200, "x : %.2f", heli.coordinates.x);
	sprintf_s(heliY, 200, "y : %.2f", heli.coordinates.y);
	sprintf_s(heliZ, 200, "z : %.2f", heli.coordinates.z);
	glTranslated(-orthoLevel, orthoLevel, 0);
	glRasterPos2d(orthoLevel / 20.0, -orthoLevel / 20.0);
	glutBitmapString(GLUT_BITMAP_8_BY_13, heliX);
	glRasterPos2d(orthoLevel / 20.0, -orthoLevel / 20.0 * 2.0);
	glutBitmapString(GLUT_BITMAP_8_BY_13, heliY);
	glRasterPos2d(orthoLevel / 20.0, -orthoLevel / 20.0 * 3.0);
	glutBitmapString(GLUT_BITMAP_8_BY_13, heliZ);

	glPopMatrix();
}

void setCamera()
{
	GLdouble eye_x, eye_y, eye_z;
	if (!freeCam)
	{
		eye_x = heli.coordinates.x + sin(heli.yaw * PI / 180.0) * 10.0;
		eye_y = (heli.coordinates.y + heli.size * 0.2) + sin(heli.pitch * PI / 180.0) * 3.0;
		eye_z = (heli.coordinates.z) + cos((heli.yaw + 180) * PI / 180.0) * 10.0;
	}
	else
	{
		POINT cursorPos;
		GetCursorPos(&cursorPos);
		if (freeCamPitch == 0 && freeCamYaw == 0)
		{
			freeCamYaw = heli.yaw;
			freeCamPitch = heli.pitch;
			freeCamMouseXStart = cursorPos.x;
			freeCamMouseYStart = cursorPos.y;
		}
		freeCamYaw -= (freeCamMouseXStart - cursorPos.x) / 360.0;
		freeCamPitch += (freeCamMouseYStart - cursorPos.y) / 360.0;

		eye_x = heli.coordinates.x + sin(freeCamYaw * PI / 180.0) * 10.0;
		eye_y = (heli.coordinates.y + heli.size * 0.2) + sin(freeCamPitch * PI / 180.0) * 3.0;
		eye_z = (heli.coordinates.z) + cos((freeCamYaw + 180) * PI / 180.0) * 10.0;

		// Free cam slowdown
		if (cursorPos.x > freeCamMouseXStart)
		{
			freeCamMouseXStart += fabsf(cursorPos.x - freeCamMouseXStart) * 0.02f;
		}
		else
		{
			freeCamMouseXStart -= fabsf(cursorPos.x - freeCamMouseXStart) * 0.02f;
		}
		if (cursorPos.y > freeCamMouseYStart)
		{
			freeCamMouseYStart += fabsf(cursorPos.y - freeCamMouseYStart) * 0.02f;
		}
		else
		{
			freeCamMouseYStart -= fabsf(cursorPos.y - freeCamMouseYStart) * 0.02f;
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

	cameraPosition.x = eye_x;
	cameraPosition.y = eye_y;
	cameraPosition.z = eye_z;
}

/*
  A simple ground plane in the XZ plane with vertex normals specified for lighting
  the top face of the ground. The bottom face is not lit.
*/
GLint desert_tex;
void basicGround(void)
{
	if (!desert_tex)
		desert_tex = loadTexture("desert_texture.bmp", 1400, 1050);

	glPushMatrix();

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, desert_tex);

	// map the texture to the quad

	glPushMatrix();
	for (int i = 0; i < 10; i++)
	{
		//glTranslated(0.0, 0.0, 400.0);
		for (int k = 0; k < 10; k++)
		{
			glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f); glVertex3f(-200.0f, 0.0f, -200.0f);
			glTexCoord2f(1.0f, 0.0f); glVertex3f(200.0f, 0.0f, -200.0f);
			glTexCoord2f(1.0f, 1.0f); glVertex3f(200.0f, 0.0f, 200.0f);
			glTexCoord2f(0.0f, 1.0f); glVertex3f(-200.0f, 0.0f, 200.0f);
			glEnd();
			glTranslated(400.0, 0.0, 0.0);
		}
		glTranslated(-400.0 * 10.0, 0.0, 400.0);
	}
	glPopMatrix();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}

/*
https://learnopengl.com/In-Practice/2D-Game/Collisions/Collision-detection
https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
*/
int checkCollision(Pos3 positionOne, Pos3 collisionBoxOne, Pos3 positionTwo, Pos3 collisionBoxTwo)
{
	return (
		positionOne.x - (collisionBoxOne.x / 2.0) <= 200.0 &&
		positionOne.x + (collisionBoxOne.x / 2.0) >= -200.0 &&
		positionOne.y - (collisionBoxOne.y / 2.0) <= 0.0 &&
		positionOne.y + (collisionBoxOne.y / 2.0) >= 0.0 &&
		positionOne.z - (collisionBoxOne.z / 2.0) <= 200.0 &&
		positionOne.z + (collisionBoxOne.z / 2.0) >= -200.0
		);
}

void thinkHelicopter()
{
	int collidedWithTerrain = 0;
	if (checkCollision(heli.coordinates, heli.collisionBox, heli.collisionBox, heli.collisionBox))
	{
		collidedWithTerrain = 1;
		heli.pitch *= (1.0 - 0.1);
		heli.velocity *= (1.0 - 0.25);

		heli.roll *= (1.0 - 0.1);
		heli.strafeVelocity *= (1.0 - 0.25);

		heli.liftVelocity *= (1.0 - 0.75);
		heli.rotorVelocity -= 0.05;
		heli.rotorVelocity = max(heli.rotorVelocity, 0.0);
	}
	else
	{
		heli.rotorVelocity += 0.05;
		heli.rotorVelocity = min(heli.rotorVelocity, 25.0);
	}

	if (keyboardMotion.Yaw != MOTION_NONE && !collidedWithTerrain) {
		heli.yaw -= keyboardMotion.Yaw;

		if (heli.yaw >= 360) heli.yaw = 0;
		if (heli.yaw < 0) heli.yaw = 360;
	}

	if (keyboardMotion.Surge != MOTION_NONE && !collidedWithTerrain) {
		if (heli.velocity <= 100.0 && heli.velocity >= -100.0)
			heli.velocity += keyboardMotion.Surge * FRAME_TIME_SEC;

		heli.direction = heli.yaw;


		if (keyboardMotion.Surge > 0 && heli.pitch < 25.0) heli.pitch += keyboardMotion.Surge;
		if (keyboardMotion.Surge < 0 && heli.pitch > -25.0) heli.pitch += keyboardMotion.Surge;
	}
	else
	{
		// Approaching 0.0
		// pitch *= (1.0 - halflifeRate)
		heli.pitch *= (1.0 - 0.1);
		heli.velocity *= (1.0 - 0.01);
	}

	if (keyboardMotion.Sway != MOTION_NONE && !collidedWithTerrain) {

		if (heli.strafeVelocity <= 100.0 && heli.strafeVelocity >= -100.0)
			heli.strafeVelocity += keyboardMotion.Sway * FRAME_TIME_SEC;

		heli.direction = heli.yaw;

		if (keyboardMotion.Sway > 0 && heli.roll < 25.0) heli.roll += keyboardMotion.Sway;
		if (keyboardMotion.Sway < 0 && heli.roll > -25.0) heli.roll += keyboardMotion.Sway;
	}
	else
	{
		heli.roll *= (1.0 - 0.1);
		heli.strafeVelocity *= (1.0 - 0.01);
	}

	if (keyboardMotion.Heave != MOTION_NONE) {
		if (heli.rotorVelocity < 15.0)
		{
			heli.rotorVelocity += 0.10;
		}
		else
		{
			if (heli.liftVelocity <= 45.0 && heli.liftVelocity >= -45.0)
				heli.liftVelocity += keyboardMotion.Heave * FRAME_TIME_SEC;

			if (checkCollision(heli.coordinates, heli.collisionBox, heli.collisionBox, heli.collisionBox))
				if (keyboardMotion.Heave < 0)
					heli.liftVelocity = 0.0;
		}
	}
	else
	{
		heli.liftVelocity *= (1.0 - 0.1);
	}

	heli.coordinates.x -= sin(heli.direction * PI / 180.0) * heli.velocity;
	heli.coordinates.z += cos(heli.direction * PI / 180.0) * heli.velocity;

	heli.coordinates.x -= cos(heli.direction * PI / 180.0) * heli.strafeVelocity;
	heli.coordinates.z -= sin(heli.direction * PI / 180.0) * heli.strafeVelocity;

	heli.coordinates.y += heli.liftVelocity;



	heli.rotorRotation += heli.rotorVelocity;
	if (heli.rotorRotation > 360.0) heli.rotorRotation = 0.0;
}

void drawFog()
{
	GLfloat fogColor[4] = { 0.8, 0.4, 0.0, 1.0 }; // orange
	glFogi(GL_FOG_MODE, GL_LINEAR);
	glFogfv(GL_FOG_COLOR, fogColor);
	glFogf(GL_FOG_DENSITY, 0.25);
	glHint(GL_FOG_HINT, GL_NICEST);
	glFogf(GL_FOG_START, heli.size * 6); // start distance of the fog
	glFogf(GL_FOG_END, heli.size * 50); // end distance of the fog
}