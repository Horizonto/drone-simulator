#pragma once
// ************************
// DemoPhongData.h
// ************************


#include "EduPhong.h"
#include "LinearR4.h"

extern LinearMapR4 viewMatrix;          // Defined in PhongProj.cpp

// It is suggested to use
//     myMaterial[0] for your initial
//     myMaterial[1] for the ground plane
//     myMaterial[2] for the surface of rotation
extern phMaterial myMaterials[3];

// There are (up to) four lights.
// They are enabled/disabled by PhongProj.cpp code (already written)
// myLights[0], myLights[1], myLights[2] are the three lights above the scene.
// myLights[3] is the spotlight.
extern phLight myLights[4];

void MySetupGlobalLight();
void MySetupLights();
void LoadAllLights();
void MySetupMaterials();
void MyRenderSpheresForLights();