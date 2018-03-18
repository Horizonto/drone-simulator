// *******************************
// EduPhong.h - Version 0.1 - February 17, 2018
//
// EduPhong.h and EduPhong.cpp code gives C++ classes
//     assisting in demostrating Phong lighting for 
//     educational purposes.
//
// Author: Sam Buss
//
// Software is "as-is" and carries no warranty. It may be used without
//  restriction, but if you modify it, please change the filenames to
//  prevent confusion between different versions.
// Bug reports: Sam Buss, sbuss@ucsd.edu
// *******************************


#pragma once

#ifndef EDU_PHONG_H
#define EDU_PHONG_H

#include "LinearR3.h"
#include "LinearR4.h"

constexpr int phMaxNumLights = 8;           // Needs to match the number in the shaders

// ********
// phMaterial - 
//   Material properies describe the color/reflectively of the surface.
//   Material properies are vertex attributes; however, they are
//    generally the same across a single object and do not vary per vertex,
//    so they are therefore implemented as generic vertex attributes.
// ********
class phMaterial {
public:
    VectorR3 EmissiveColor;
    VectorR3 AmbientColor;
    VectorR3 DiffuseColor;
    VectorR3 SpecularColor;
    float SpecularExponent;

    // Constructors and initializers
    phMaterial();
 
    // Load generic attributes for the shader programs.
    void LoadIntoShaders();
};

// ********
// phLight - 
//   Light properties describe the color/brightness of a light.
//   Light properties may need to be accessed by the fragment shader
//      as well as the vertex shader (at least for Phong shading),
//      so they are uniform values, not vertex attributes.
// ********
class phLight {
public:
    bool IsEnabled;             // True if light is turned on (default is false)
    bool IsAttenuated;          // True if attenuation is active (default is false)
    bool IsSpotLight;           // True if spotlight (default is false)
    bool IsDirectional;         // True if directional (default is false) 
private:
    VectorR3 PosOrDir;          // Position or direction, already transformed by the model view matrix
public:
    VectorR3 AmbientColor;
    VectorR3 DiffuseColor;
    VectorR3 SpecularColor;
private:
    VectorR3 SpotDirection;     // Spot light center direction, already transformed by the model view matrix
public:
    float SpotCosCutoff;        // Cosine of cutoff angle
    float SpotExponent;
    float ConstantAttenuation;
    float LinearAttenuation;
    float QuadraticAttenuation;

    phLight();                  // Constructor
    bool CheckCorrectness();

    void SetPosition(const LinearMapR4& modelviewMatrix, const VectorR3& position);
    void SetDirection(const LinearMapR4& modelviewMatrix, const VectorR3& direction);
    void SetSpotlightDirection(const LinearMapR4& modelviewMatrix, const VectorR3& direction);
    void LoadIntoShaders(int lightNumber);
};

// ********
// phGlobal - 
//   Global illumination properties.
//   These are uniform values.
//   The viewer is presumed to be at the origin, looking down the negative z-axis
// ********
class phGlobal {
public:
    VectorR3 GlobalAmbientColor;    // Global ambient light color
    unsigned int NumLights;         // Number of lights.
    bool LocalViewer;               // true for local viewer; false for directional viewer (default false)
    bool EnableEmissive;            // Control whether emissive colors are rendered (default is true)
    bool EnableDiffuse;             // Control whether diffuse colors are rendered (default is true)
    bool EnableAmbient;             // Control whether ambient colors are rendered (default is true)
    bool EnableSpecular;            // Control whether specular colors are rendered (default is true)

    phGlobal();                     // Constructor
    bool CheckCorrectness();

    void LoadIntoShaders();               // Load the global lighting data into the shaders
};

// ***********************************************************
// Externals for Phong lighting -- and Phong lighting or Gouraud shading.
// ***********************************************************

extern unsigned int phShaderPhongPhong;     // Shader program: Phuong lighting with Phong shading
extern unsigned int phShaderPhongGouraud;   // Shader program: Phuong lighting with Gouraud shading

// The next values are used when setting vertex attribute pointers,
//     and when loading generic vertex attributes
extern const unsigned int phVertPos_loc;                   // Corresponds to "location = 0" in the vertex shader definition
extern const unsigned int phVertNormal_loc;                // Corresponds to "location = 1" in the vertex shader definition
extern const unsigned int EmissiveColor_loc;               // Corresponds to "location = 3" in the vertex shader definition
extern const unsigned int AmbientColor_loc;                // Corresponds to "location = 4" in the vertex shader definition
extern const unsigned int DiffuseColor_loc;                // Corresponds to "location = 5" in the vertex shader definition
extern const unsigned int SpecularColor_loc;               // Corresponds to "location = 6" in the vertex shader definition
extern const unsigned int SpecularExponent_loc;            // Corresponds to "location = 7" in the vertex shader definition

extern unsigned int projMatLocationPP;				    // Location of the projectionMatrix in the Phong-Phong shader program.
extern unsigned int projMatLocationPG;				    // Location of the projectionMatrix in the Phong-Gouraud shader program.
extern unsigned int modelviewMatLocationPP;			    // Location of the modelviewMatrix in the Phong-Phong shader program.
extern unsigned int modelviewMatLocationPG;			    // Location of the modelviewMatrix in the Phong-Gouraud shader program.
extern unsigned int applyTextureLocationPP;			    // Location of applyTexture in the Phong-Phong shader program.
extern unsigned int applyTextureLocationPG;			    // Location of applyTexture in the Phong-Gouraud shader program.

void setup_phong_shaders();                 // Compiles and links the two shader programs

// *************************************
// Constructors: Set default values.
// *************************************

// Constructor for EduPhone_Material: sets default values
inline phMaterial::phMaterial() :
    EmissiveColor{ 0.0f, 0.0f, 0.0f },
    AmbientColor{ 0.0f, 0.0f, 0.0f },
    DiffuseColor{ 0.0f, 0.0f, 0.0f },
    SpecularColor{ 0.0f, 0.0f, 0.0f },
    SpecularExponent(0.0)
{}

// Constructor for phLight: sets default values
inline phLight::phLight() :
    IsEnabled(false),
    IsAttenuated(false),
    IsSpotLight(false),
    IsDirectional(false),
    PosOrDir{ 0.0f, 0.0f, 0.0f }, 
    AmbientColor{ 0.0f, 0.0f, 0.0f },
    DiffuseColor{ 0.0f, 0.0f, 0.0f },
    SpecularColor{ 0.0f, 0.0f, 0.0f },
    SpotDirection{ 0.0, 0.0, -1.0 },
    SpotCosCutoff(0.0f),
    SpotExponent(0.0f),
    ConstantAttenuation(1.0f),
    LinearAttenuation(0.0f),
    QuadraticAttenuation(0.0f)
{}

// Constructor for phGlobal: sets default values
inline phGlobal::phGlobal() :
    GlobalAmbientColor{ 0.0f, 0.0f, 0.0f },
    LocalViewer(false),
    EnableEmissive(true),
    EnableDiffuse(true),
    EnableAmbient(true),
    EnableSpecular(true)
{};

// Set the light's position: The position transformed
//     by the modelview matrix.  The modelview matrix must be an affine mapping.
// The IsDirectional flag is reset to make sure the light is positional.
inline void phLight::SetPosition(
    const LinearMapR4& modelviewMatrix, const VectorR3& position)
{
    PosOrDir = position;
    modelviewMatrix.AffineTransformPosition(PosOrDir);
    IsDirectional = false;
}

// Set the light's direction: The direction is transformed
//     by the modelview matrix.  The modelview matrix must be an affine mapping.
// The IsDirectional flag is set to make sure the light is directional.
inline void phLight::SetDirection(
    const LinearMapR4& modelviewMatrix, const VectorR3& direction)
{
    PosOrDir = direction;
    modelviewMatrix.AffineTransformDirection(PosOrDir);
    PosOrDir.Normalize();
    IsDirectional = true;
}

// Set the spotlight's central direction: The spotlight direction is transformed
//     by the modelview matrix.  The modelview matrix must be an affine mapping.
// It does not set the flag indicating the light is a spotlight.
inline void phLight::SetSpotlightDirection(
    const LinearMapR4& modelviewMatrix, const VectorR3& direction)
{
    SpotDirection = direction;
    modelviewMatrix.AffineTransformDirection(SpotDirection);
    SpotDirection.Normalize();
}

#endif // EDU_PHONG_H
