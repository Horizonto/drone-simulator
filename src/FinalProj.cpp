/*
 * FinalProj.cpp - March 17, 2018
 *
 * This project is based on a lot of starting code written by Sam Buss.
 * 
 * Author: Xiudi Tang
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 */

// These libraries are needed to link the program.
// First five are usually provided by the system.
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")
#pragma comment(lib,"glfw3.lib")
#pragma comment(lib,"glew32s.lib")
#pragma comment(lib,"glew32.lib")


// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

#include "LinearR3.h"
#include "LinearR4.h"
#include "EduPhong.h"
#include "PhongData.h"
#include "ShaderBuild.h"
#include "GlGeomSphere.h"
#include "GlGeomCylinder.h"

// Enable standard input and output via printf(), etc.
// Put this include *after* the includes for glew and GLFW!
#include <stdio.h>

#include "FinalProj.h"
#include "MyGeometries.h"
#include "MyDrone.h"
#include "DrawScene.h"

// ********************
// Animation controls and state infornation
// ********************

// These variables control the view direction.
//    The arrow keys are used to change these values.
double viewAzimuth = 0.25;	// Angle of view up/down (in radians)
double viewDirection = 0.0; // Rotation of view around y-axis (in radians)
double deltaAngle = 0.01;	// Change in view angle for each up/down/left/right arrow key press
LinearMapR4 viewMatrix;		// The current view matrix, based on viewAzimuth and viewDirection.

// This variable controls whether running or paused.
bool spinMode = true;

// This variable controls whether printing the test info or not.
bool testInfo = true;

// Control Phong lighting modes
// Use Gouraud or not.  (true == use Gouraud).
bool UsePhongGouraud = false;
phGlobal globalPhongData;

// These two variables control how triangles are rendered.
bool wireframeMode = false;	// Equals true for polygon GL_LINE mode. False for polygon GL_FILL mode.
bool cullBackFaces = true;

// The next variable controls the resolution of the meshes for cylinders and spheres.
int meshRes=4;             // Resolution of the meshes (slices, stacks, and rings all equal)

// These variables control the animation's state and speed.
// YOU PROBABLY WANT TO RE-DO THIS FOR YOUR CUSTOM ANIMATION.  
double animateIncrement = 0.01;   // Make bigger to speed up animation, smaller to slow it down.
double currentTime = 0.0;         // Current "time" for the animation.
double currentDelta = 0.0;        // Current state of the animation (YOUR CODE MAY NOT WANT TO USE THIS.)

bool mouseLeftButtonPressed = false;
double lastPressXPos = 0.0, lastPressYPos = 0.0;
double lastViewAzimuth = 0.0, lastViewDirection = 0.0;

// ************************
// General data helping with setting up VAO (Vertex Array Objects)
//    and Vertex Buffer Objects.
// ***********************

unsigned int projMatLocation;						// Location of the projectionMatrix in the currently active shader program
unsigned int modelviewMatLocation;					// Location of the modelviewMatrix in the currently active shader program
unsigned int applyTextureLocation; 					// Location of the applyTexture bool in the currently active shader program

//  The Projection matrix: Controls the "camera view/field-of-view" transformation
//     Generally is the same for all objects in the scene.
LinearMapR4 theProjectionMatrix;		//  The Projection matrix: Controls the "camera/view" transformation

// *****************************
// These variables set the dimensions of the perspective region we wish to view.
// They are used to help form the projection matrix and the view matrix
// All rendered objects lie in the rectangular prism centered on the z-axis
//     equal to (-Xmax,Xmax) x (-Ymax,Ymax) x (Zmin,Zmax)
// Be sure to leave some slack in these values, to allow for rotations, etc.
// The model/view matrix can be used to move objects to this position
// THESE VALUES MAY NEED AD-HOC ADJUSTMENT TO GET THE SCENE TO BE VISIBLE.
const double Xmax = 8.0;                // Control x dimensions of viewable scene
const double Ymax = 6.0;                // Control y dimensions of viewable scene
const double Zmin = -9.0, Zmax = 9.0;   // Control z dimensions of the viewable scene

// zDistance equals the initial distance from the camera to the z = Zmax plane
const double zDistance = 20.0;              // Make this value larger or smaller to affect field of view.

double ZextraDistance = 0.0;              // Extra distance we have moved to/from the scene
double ZextraDelta = 0.2;                // Pressing HOME/END moves closer/farther by this amount
const double ZextraDistanceMin = -15.0;
const double ZextraDistanceMax = 50.0;
int screenWidth = 800, screenHeight = 600;     // Width and height in pixels. Initially 800x600


// *************************
// mySetupGeometries defines the scene data, especially vertex  positions and colors.
//    - It also loads all the data into the VAO's (Vertex Array Objects) and
//      into the VBO's (Vertex Buffer Objects).
// This routine is only called once to initialize the data.
// *************************
void mySetupGeometries() {
 
    MySetupSurfaces();

     mySetViewMatrix();

    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void mySetViewMatrix() {
    // Set the view matrix. Sets view distance, and view direction.
    // The final translation is done because the ground plane lies in the xz-plane,
    //    se the center of the scene is about 3 or 4 units above the origin.
    // YOU MAY NEED TO ADJUST THE FINAL TRANSLATION AND?OR ADD A SCALING TO MAKE THE SCENE VISIBLE.
    viewMatrix.Set_glTranslate(0.0, 0.0, -(Zmax + zDistance + ZextraDistance));      // Translate to be in front of the camera
    viewMatrix.Mult_glRotate(viewAzimuth, 1.0, 0.0, 0.0);	    // Rotate viewAzimuth radians around x-axis
    viewMatrix.Mult_glRotate(-viewDirection, 0.0, 1.0, 0.0);    // Rotate -viewDirection radians around y-axis
    viewMatrix.Mult_glTranslate(0.0, -3.5, 0.0);                // Translate the scene down the y-axis so the center is near the origin.
}

// *************************************
// Main routine for rendering the scene
// MyRenderScene() is called every time the scene needs to be redrawn.
// mySetupGeometries() has already created the vertex and buffer objects
//    and the model view matrices.
// The EduPhong shaders are already setup.
// *************************************
void MyRenderScene() {
   
    // Clear the rendering window
    static const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    const float clearDepth = 1.0f;
    glClearBufferfv(GL_COLOR, 0, black);
    glClearBufferfv(GL_DEPTH, 0, &clearDepth);	// Must pass in a *pointer* to the depth

    glUseProgram(UsePhongGouraud ? phShaderPhongGouraud : phShaderPhongPhong);

    MyRenderGeometries();
	MyRenderDrone();
	MyRenderSpheresForLights();

    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void my_setup_SceneData() {

	setup_phong_shaders();
	mySetupGeometries();
	MySetupInitialGeometries();
    SetupForTextures();

    // Initially, the Phong-Gouraud shader is used 
    projMatLocation = UsePhongGouraud ? projMatLocationPG : projMatLocationPP;
    modelviewMatLocation = UsePhongGouraud ? modelviewMatLocationPG : modelviewMatLocationPP;
    applyTextureLocation = UsePhongGouraud ? applyTextureLocationPG : applyTextureLocationPP;

    MySetupGlobalLight();
    MySetupLights();
    LoadAllLights();
    MySetupMaterials();

	check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

// *******************************************************
// Process all key press events.
// This routine is called each time a key is pressed or released.
// *******************************************************

inline void UpdateView() {
	mySetViewMatrix();
	setProjectionMatrix();
	LoadAllLights();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    static const double Pi = 3.1415926535f;
    if (action == GLFW_RELEASE) {
        return;			// Ignore key up (key release) events
    }
    bool viewChanged = false;
    switch (key) {
    case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, true);
        return;
    case GLFW_KEY_KP_4:
    case GLFW_KEY_KP_1:
    case GLFW_KEY_KP_2:
    case GLFW_KEY_KP_3:
    {
        phLight& theLight = myLights[key - GLFW_KEY_KP_1];
        theLight.IsEnabled = !theLight.IsEnabled;   // Toggle whether the light is enabled.
        LoadAllLights();
        return;
    }
	case 'T':
		testInfo = !testInfo;	// Test information on and off.
		return;
	case 'R':
        spinMode = !spinMode;	// Toggle animation on and off.
        return;
    case 'W':		// Toggle wireframe mode
        if (wireframeMode) {
            wireframeMode = false;
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else {
            wireframeMode = true;
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        return;
    case 'C':		// Toggle backface culling
        cullBackFaces = !cullBackFaces;     // Negate truth value of cullBackFaces
        if (cullBackFaces) {
            glEnable(GL_CULL_FACE);
        }
        else {
            glDisable(GL_CULL_FACE);
        }
        return;
    case 'M':
        if (mods & GLFW_MOD_SHIFT) {
            meshRes = meshRes < 79 ? meshRes + 1 : 80;  // Uppercase 'M'
        }
        else {
            meshRes = meshRes > 4 ? meshRes - 1 : 3;    // Lowercase 'm'
        }
        MyRemeshGeometries();
        return;
    case 'F':
        if (mods & GLFW_MOD_SHIFT) {                // If upper case 'F'
            animateIncrement *= sqrt(2.0);			// Double the animation time step after two key presses
        }
        else {                                      // Else lose case 'f',
            animateIncrement *= sqrt(0.5);			// Halve the animation time step after two key presses
        }
        return;
    case GLFW_KEY_P:
        UsePhongGouraud = !UsePhongGouraud;
        projMatLocation = UsePhongGouraud ? projMatLocationPG : projMatLocationPP;
        modelviewMatLocation = UsePhongGouraud ? modelviewMatLocationPG : modelviewMatLocationPP;
        applyTextureLocation = UsePhongGouraud ? applyTextureLocationPG : applyTextureLocationPP;
        return;
    case GLFW_KEY_UP:
		viewAzimuth = Min(viewAzimuth + 0.01, PIhalves - 0.05);
		viewChanged = true;
		break;
    case GLFW_KEY_DOWN:
        viewAzimuth = Max(viewAzimuth - 0.01, -PIhalves + 0.05);
        viewChanged = true;
        break;
    case GLFW_KEY_RIGHT:
        viewDirection += 0.01;
        if (viewDirection > PI) {
            viewDirection -= PI2;
        }
        viewChanged = true;
        break;
    case GLFW_KEY_LEFT:
        viewDirection -= 0.01;
        if (viewDirection < -PI) {
            viewDirection += PI2;
        }
        viewChanged = true;
        break;
    case GLFW_KEY_A:
        globalPhongData.EnableAmbient = !globalPhongData.EnableAmbient;
        break;
    case GLFW_KEY_E:
        globalPhongData.EnableEmissive = !globalPhongData.EnableEmissive;
        break;
    case GLFW_KEY_D:
        globalPhongData.EnableDiffuse = !globalPhongData.EnableDiffuse;
        break;
    case GLFW_KEY_S:
        globalPhongData.EnableSpecular = !globalPhongData.EnableSpecular;
        break;
    case GLFW_KEY_V:
        globalPhongData.LocalViewer = !globalPhongData.LocalViewer;
        break;
	case '1':
		spinVelocity[0] += angularVelocityIncrement;
		return;
	case '2':
		spinVelocity[0] -= angularVelocityIncrement;
		return;
	case '3':
		spinVelocity[1] += angularVelocityIncrement;
		return;
	case '4':
		spinVelocity[1] -= angularVelocityIncrement;
		return;
	case '7':
		spinVelocity[2] += angularVelocityIncrement;
		return;
	case '8':
		spinVelocity[2] -= angularVelocityIncrement;
		return;
	case '9':
		spinVelocity[3] += angularVelocityIncrement;
		return;
	case '0':
		spinVelocity[3] -= angularVelocityIncrement;
		return;
    }

    if (viewChanged) {
        mySetViewMatrix();
        setProjectionMatrix();
        LoadAllLights();        // Have to call this since it affects the position of the lights!
    }
    else {
        // Updated the global phong data above: upload it to the shader program.
        globalPhongData.LoadIntoShaders();
    }
}

void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (testInfo) printf("Mouse location is at (%i, %i).\n", int(xpos), int(ypos));
	if (mouseLeftButtonPressed) {
		double lastReleaseXPos, lastReleaseYPos;
		glfwGetCursorPos(window, &lastReleaseXPos, &lastReleaseYPos);
		double deltaXPos = lastReleaseXPos - lastPressXPos, deltaYPos = lastReleaseYPos - lastPressYPos;
		viewDirection = lastViewDirection - deltaXPos * 0.01f;      // Rotate view left or right, adjust 0.01f to change the sensitivity   
		viewAzimuth = lastViewAzimuth + deltaYPos * 0.01f;			// Rotate view up or down, adjust 0.01f to change the sensitivity. 
																	// You may realize instability when you drag across over top view or bottom view. 
																	// This phenomenon has deep mathematical reasons. 
		if (viewAzimuth > PI) viewAzimuth -= PI2;
		if (viewAzimuth < -PI) viewAzimuth += PI2;
		if (viewDirection > PI) viewDirection -= PI2;
		if (viewDirection < -PI) viewDirection += PI2;
		UpdateView();
	}
}

void cursor_enter_callback(GLFWwindow* window, int entered)
{
	if (entered)
	{
		if (testInfo) printf("Mouse entered.\n");
	}
	else
	{
		if (testInfo) printf("Mouse left.\n");
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
		if (testInfo) printf("Left button of the mouse was pressed.\n");
		mouseLeftButtonPressed = true;
		glfwGetCursorPos(window, &lastPressXPos, &lastPressYPos);
		lastViewAzimuth = viewAzimuth; lastViewDirection = viewDirection;
	}
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		if (testInfo) printf("Left button of the mouse was released.\n");
		mouseLeftButtonPressed = false;
	}
	if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
		if (testInfo) printf("Right button of the mouse was pressed. I want to do popup_menu() but I do not know how to realize it.\n");
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (testInfo) printf("Mouse scrool (%i, %i).\n", int(xoffset), int(yoffset));
	ZextraDistance -= ZextraDelta * yoffset * 5.0f;         // Move closer or farther, adjust 5.0f to change the sensitivity   
	ClampMin(&ZextraDistance, ZextraDistanceMin);
	UpdateView();
}
// *************************************************
// This function is called with the graphics window is first created,
//    and again whenever it is resized.
// The Projection View Matrix is typically set here.
//    But this program does not use any transformations or matrices.
// *************************************************
void window_size_callback(GLFWwindow* window, int width, int height) {
    // Define the portion of the window used for OpenGL rendering.
    glViewport(0, 0, width, height);
    screenWidth = width == 0 ? 1 : width;
    screenHeight = height==0 ? 1 : height;
    setProjectionMatrix();
}

void setProjectionMatrix() {
	// Setup the projection matrix as a perspective view.
	// The complication is that the aspect ratio of the window may not match the
	//		aspect ratio of the scene we want to view.
	double w = (double)screenWidth;
	double h = (double)screenHeight;
	double windowXmax, windowYmax;
    double aspectFactor = w * Ymax / (h * Xmax);   // == (w/h)/(Xmax/Ymax), ratio of aspect ratios
	if (aspectFactor>1) {
		windowXmax = Xmax * aspectFactor;
		windowYmax = Ymax;
	}
	else {
		windowYmax = Ymax / aspectFactor;
		windowXmax = Xmax;
	}

	// Using the max & min values for x & y & z that should be visible in the window,
	//		we set up the orthographic projection.
    // Could update the zNear and zFar each time the distance changes, but we'll avoid it for now
    double zNear = zDistance+ZextraDistance;
    double zFar = zNear + Zmax - Zmin;
    double scale = zNear / zDistance;
    theProjectionMatrix.Set_glFrustum(-windowXmax * scale, windowXmax * scale,
                                      -windowYmax * scale, windowYmax * scale, zNear, zFar);

    if (glIsProgram(phShaderPhongGouraud)) {
        glUseProgram(phShaderPhongGouraud);
        theProjectionMatrix.DumpByColumns(matEntries);
        glUniformMatrix4fv(projMatLocationPG, 1, false, matEntries);
    }
    if (glIsProgram(phShaderPhongPhong)) {
        glUseProgram(phShaderPhongPhong);
        theProjectionMatrix.DumpByColumns(matEntries);
        glUniformMatrix4fv(projMatLocationPP, 1, false, matEntries);
    }
    check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void my_setup_OpenGL() {
	
	glEnable(GL_DEPTH_TEST);	// Enable depth buffering
	glDepthFunc(GL_LEQUAL);		// Useful for multipass shaders

	// Set polygon drawing mode for front and back of each polygon
    glPolygonMode(GL_FRONT_AND_BACK, wireframeMode ? GL_LINE : GL_FILL );

    glEnable(GL_CULL_FACE);

	check_for_opengl_errors();   // Really a great idea to check for errors -- esp. good for debugging!
}

void error_callback(int error, const char* description)
{
	// Print error
	fputs(description, stderr);
}

void setup_callbacks(GLFWwindow* window) {
	// Set callback function for resizing the window
	glfwSetFramebufferSizeCallback(window, window_size_callback);

	// Set callback for key up/down/repeat events
	glfwSetKeyCallback(window, key_callback);

	// Set callbacks for mouse movement (cursor position) and mouse botton up/down events.
	glfwSetCursorPosCallback(window, cursor_pos_callback);
	glfwSetCursorEnterCallback(window, cursor_enter_callback);
	glfwSetMouseButtonCallback(window, mouse_button_callback);
	glfwSetScrollCallback(window, scroll_callback);
}

int main() {
	glfwSetErrorCallback(error_callback);	// Supposed to be called in event of errors. (doesn't work?)
	glfwInit();
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "Phong Demo", NULL, NULL);
	if (window == NULL) {
		printf("Failed to create GLFW window!\n");
		return -1;
	}
	glfwMakeContextCurrent(window);

	if (GLEW_OK != glewInit()) {
		printf("Failed to initialize GLEW!.\n");
		return -1;
	}

	// Print info of GPU and supported OpenGL version
	printf("Renderer: %s\n", glGetString(GL_RENDERER));
	printf("OpenGL version supported %s\n", glGetString(GL_VERSION));
#ifdef GL_SHADING_LANGUAGE_VERSION
	printf("Supported GLSL version is %s.\n", (char *)glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
    printf("Using GLEW version %s.\n", glewGetString(GLEW_VERSION));

	printf("------------------------------\n");
	printf("Press 'r' or 'R' (Run) to toggle(off and on) running the animation.\n");
    printf("Press arrow keys to adjust the view direction.\n");
    printf("Press HOME or END to closer to and farther away from the scene.\n");
    printf("Press 'w' or 'W' (wireframe) to toggle whether wireframe or fill mode.\n");
    printf("Press 'M' (mesh) to increase the mesh resolution.\n");
    printf("Press 'm' (mesh) to decrease the mesh resolution.\n");
    printf("Press 'P' key (Phong) to toggle using Phong shading and Gouraud shading.\n");
    printf("Press 'E' key (Emissive) to toggle rendering Emissive light.\n");
    printf("Press 'A' key (Ambient) to toggle rendering Ambient light.\n");
    printf("Press 'D' key (Diffuse) to toggle rendering Diffuse light.\n");
    printf("Press 'S' key (Specular) to toggle rendering Specular light.\n");
    printf("Press 'V' key (Viewer) to toggle using a local viewer.\n");
    printf("Press ESCAPE to exit.\n");
	
    setup_callbacks(window);
   
	// Initialize OpenGL, the scene and the shaders
    my_setup_OpenGL();
	my_setup_SceneData();
 	window_size_callback(window, screenWidth, screenHeight);

    // Loop while program is not terminated.
	while (!glfwWindowShouldClose(window)) {
	
		MyRenderScene();				// Render into the current buffer
		glfwSwapBuffers(window);		// Displays what was just rendered (using double buffering).

		// Poll events (key presses, mouse events)
		glfwWaitEventsTimeout(1.0/60.0);	    // Use this to animate at 60 frames/sec (timing is NOT reliable)
		// glfwWaitEvents();					// Or, Use this instead if no animation.
		// glfwPollEvents();					// Use this version when animating as fast as possible
	}

	glfwTerminate();
	return 0;
}

// If an error is found, it could have been caused by any command since the
//   previous call to check_for_opengl_errors()
// To find what generated the error, you can try adding more calls to
//   check_for_opengl_errors().
char errNames[8][36] = {
	"Unknown OpenGL error",
	"GL_INVALID_ENUM", "GL_INVALID_VALUE", "GL_INVALID_OPERATION",
	"GL_INVALID_FRAMEBUFFER_OPERATION", "GL_OUT_OF_MEMORY",
	"GL_STACK_UNDERFLOW", "GL_STACK_OVERFLOW" };
bool check_for_opengl_errors() {
	int numErrors = 0;
	GLenum err;
	while ((err = glGetError()) != GL_NO_ERROR) {
		numErrors++;
		int errNum = 0;
		switch (err) {
		case GL_INVALID_ENUM:
			errNum = 1;
			break;
		case GL_INVALID_VALUE:
			errNum = 2;
			break;
		case GL_INVALID_OPERATION:
			errNum = 3;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			errNum = 4;
			break;
		case GL_OUT_OF_MEMORY:
			errNum = 5;
			break;
		case GL_STACK_UNDERFLOW:
			errNum = 6;
			break;
		case GL_STACK_OVERFLOW:
			errNum = 7;
			break;
		}
		printf("OpenGL ERROR: %s.\n", errNames[errNum]);
	}
	return (numErrors != 0);
}