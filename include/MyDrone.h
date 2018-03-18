#pragma once

// 
// MyInitial.h   ---  Header file for Initial.cpp.
// 
//   Sets up and renders the initial (alphabet letter)
//   for the Math 155A project.
//
//   Comes supplied with the code for Sam Buss's "S".
//

// These objects take care of generating and loading VAO's, VBO's and EBO's,
//    They are used for the ellipsoids and cylinders in the initial.
//    They are extern'ed here to let LetterProj.cpp use them too,
//    but MyInitial.cpp is responsible for maintaining them.
//#include "Everything.h"

//extern GlGeomSphere mySphere;
//extern GlGeomCylinder myCylinder;
#include "LinearR3.h"
#include "LinearR4.h"
#include "MathMisc.h"

const double floatingHeight = 3.0;
const double axleHeight = 1.0;
const double axleRadius = 0.1;
const double centerSphereRadius = 0.3;
const double bladeLength = 1.5;
const double bladeWidth = 0.2;
const double bladeHeight = 0.05;
const double frameLength = 3.0;
const double frameRadius = 0.2;
const double connectSphereRadius = 0.25;
const double droneHeight = connectSphereRadius + axleHeight;
const double centerOfGravityHeight = 0.0 * droneHeight - floatingHeight;

const double density = 3.0;
const double totalMass = density * frameLength * PI * frameRadius * frameRadius * 4.0;
const double inertiaPerMassAxle = frameRadius * frameRadius / 2.0;
const double inertiaPerMassEnd = (3.0 * frameRadius * frameRadius + 4.0 * frameLength * frameLength) / 12.0;
const double Ix = inertiaPerMassAxle * totalMass / 2.0 + inertiaPerMassEnd * totalMass / 2.0;
const double Iy = inertiaPerMassEnd * totalMass;
const LinearMapR3 momentOfInertia = LinearMapR3(Ix, 0.0, 0.0, 0.0, Iy, 0.0, 0.0, 0.0, Ix);

//
// Function Prototypes
//
void MySetupInitialGeometries();   // Called once, before rendering begins.
void MyRemeshGeometries();         // Called when mesh changes, must update initial's goemetries.

void MyRenderDrone();
void myRenderOneEllip(const LinearMapR4& curMatr);
void myRenderShrinkingEllip(const LinearMapR4& curMatrix, double shrinkDist);


