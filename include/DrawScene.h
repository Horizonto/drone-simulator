#pragma once

//
// Header file for LetterProg.cpp
//


// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC

#include <GL/glew.h> 
#include <GLFW/glfw3.h>

class LinearMapR4;      // Used in the function prototypes, declared in LinearMapR4.h

//
// External variables.  Can be be used by other .cpp files.
// These external variables are set mostly in LetterProj.cpp,
//    and many of them are used in MyInitial.cpp to control the rendering.
// The external declarations do not give the initial values:
//    Initial values are set with their declaration in LetterPrj.cpp
//

// These two variables control whether running or paused.
//  IT IS OPTIONAL TO INCLUDE SINGLE STEP/SPIN MODE FUNCTIONALITY IN YOUR PROGRAMMING ASSIGNMENT
extern bool spinMode;
extern bool singleStep;

// These two variables control how triangles are rendered.
extern bool wireframeMode;	// Equals true for polygon GL_FILL mode. False for polygon GL_LINE mode.
extern bool cullBackFaces;   // Equals true to cull backfaces. Equals false to not cull backfaces. 

// The next variable controls the resoluton of the meshes for cylinders and spheres.
extern int meshRes;             // Resolution of the meshes (slices, stacks, and rings all equal)

// These variables control the animation's state and speed.
// YOU PROBABLY WANT TO CHANGE PARTS OF THIS FOR YOUR CUSTOM ANIMATION.  
extern double animateIncrement;   // Make bigger to speed up animation, smaller to slow it down.
extern double currentTime;         // Current "time" for the animation.
extern double currentPhase[];
extern double currentDelta;        // Current state of the animation (YOUR CODE MAY NOT WANT TO USE THIS.)
extern double anglePhi;
extern double angularVelocity[];
extern LinearMapR4 viewMatrix;		// The current view matrix, based on viewAzimuth and viewDirection.
// Comment: It would be a better design to have two matrices --- a view matrix and model matrix;
//     and have the vertex shader multiply them together; however, it is left as a single matrix for 
//     simplicity and for consistency with Math 155A Project #2.


extern double animateIncrement;
extern double currentTime;
extern double currentPhase[4];
extern double currentDelta;
extern double anglePhi;
extern double spinVelocity[4];
extern double angularVelocityIncrement;
extern VectorR3 currentVelocity;
extern VectorR3 currentAngularVelocity;
extern LinearMapR4 centerOfGravityMatrix;

// We create one shader program: consisting of a vertex shader and a fragment shader
extern const unsigned int aPos_loc;         // Corresponds to "location = 0" in the verter shader definitions
extern const unsigned int aColor_loc;       // Corresponds to "location = 1" in the verter shader definitions
extern unsigned int projMatLocation;		// Location of the projectionMatrix in the "smooth" shader program.
extern unsigned int modelviewMatLocation;	// Location of the modelviewMatrix in the "smooth" shader program.

extern float matEntries[16];	// Holds 16 floats (since cannot load doubles into a shader that uses floats)

// ***********************
// Function prototypes
// By declaring function prototypes here, they can be defined in any order desired in the .cpp file.
// ******
bool check_for_opengl_errors();     

void mySetupFloorGeometry();      
void mySetupGeometries();
void mySetViewMatrix();  

void MyRenderScene();
void MyRenderFloor();
void MyRenderTwoCylinders();

void my_setup_SceneData();
void my_setup_OpenGL();
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);
void error_callback(int error, const char* description);
void setup_callbacks(GLFWwindow* window);