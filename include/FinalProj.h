#pragma once

//
// Header file for TextureProg.cpp
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

// This  variable controls whether running or paused.
extern bool spinMode;

// The next variable controls the resoluton of the meshes for cylinders and spheres.
extern int meshRes;             // Resolution of the meshes (slices, stacks, and rings all equal)

// These variables control the animation's state and speed.
// YOU PROBABLY WANT TO CHANGE PARTS OF THIS FOR YOUR CUSTOM ANIMATION.  
extern double animateIncrement;   // Make bigger to speed up animation, smaller to slow it down.
extern double currentTime;         // Current "time" for the animation.
extern double currentDelta;        // Current state of the animation (YOUR CODE MAY NOT WANT TO USE THIS.)

extern LinearMapR4 viewMatrix;		// The current view matrix, based on viewAzimuth and viewDirection.
// Comment: This viewMatrix changes only when the view changes.
// The modelViewMatrix is updated to render objects in the desired position and orientation.
// The modelViewMatrix must incorporate the viewMatrix: the shaders do NOT use the viewMatrix.

// ***********************
// Function prototypes
// By declaring function prototypes here, they can be defined in any order desired in the .cpp file.
// ******
bool check_for_opengl_errors();     

void mySetupGeometries();
void mySetViewMatrix();  

void MyRenderScene();

void my_setup_SceneData();
void my_setup_OpenGL();
void setProjectionMatrix();

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);
void error_callback(int error, const char* description);
void setup_callbacks(GLFWwindow* window);
