/*
 * LetterProj.cpp - Version 0.1 - January 27, 2018
 *
 * Starting code for Math 155A, Project #3,
 * 
 * Author: Sam Buss
 *
 * Software accompanying POSSIBLE SECOND EDITION TO the book
 *		3D Computer Graphics: A Mathematical Introduction with OpenGL,
 *		by S. Buss, Cambridge University Press, 2003.
 *
 * Software is "as-is" and carries no warranty.  It may be used without
 *   restriction, but if you modify it, please change the filenames to
 *   prevent confusion between different versions.
 * Bug reports: Sam Buss, sbuss@ucsd.edu.
 * Web page: http://math.ucsd.edu/~sbuss/MathCG2
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

#include "LinearR3.h"		// Adjust path as needed.
#include "LinearR4.h"		// Adjust path as needed.
#include "ShaderBuild.h"
#include "GlGeomSphere.h"
#include "GlGeomCylinder.h"

// Enable standard input and output via printf(), etc.
// Put this include *after* the includes for glew and GLFW!
#include <stdio.h>

#include "DrawScene.h"
#include "MyDrone.h"

const unsigned int aPos_loc = 0;   // Corresponds to "location = 0" in the verter shader definitions
const unsigned int aColor_loc = 1; // Corresponds to "location = 1" in the verter shader definitions
bool singleStep = false;

// These variables control the animation's state and speed.
// YOU PROBABLY WANT TO RE-DO THIS FOR YOUR CUSTOM ANIMATION.  
double currentPhase[4] = { 0.0, 0.0, 0.0, 0.0 };        // Current "phase" for the animation.
double anglePhi = 0.0;
double spinVelocity[4] = { 0.0, 0.0, 0.0, 0.0 };
double angularVelocityIncrement = 0.5;
VectorR3 currentVelocity = VectorR3(0.0, 0.0, 0.0);
VectorR3 currentAngularVelocity = VectorR3(0.0, 0.0, 0.0);
LinearMapR4 centerOfGravityMatrix;
