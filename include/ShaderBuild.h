// *******************************
// ShaderBuild.h - Version 0.1 - February 25, 2018
//
// ShaderBuild.cpp code compiles and links shaders
//
// Software is "as-is" and carries no warranty. It may be used without
//  restriction, but if you modify it, please change the filenames to
//  prevent confusion between different versions.
// Bug reports: Sam Buss, sbuss@ucsd.edu
// *******************************

#pragma once
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

// All of our shader program will have common locations for 
//     the vertex position (a vec3), vertex normal (a vec3), texture coordinates (a vec 2),
//     the projection matrix, and the modelview matrix.
constexpr unsigned int vertPos_loc = 0;         // Location of vertex position in the vertex shaders
constexpr unsigned int vertNormal_loc = 1;      // Location of vertex normal in the vertex shaders
constexpr unsigned int vertTexCoords_loc = 2;   // Location of vertex texture coordinates in vertex shaders
extern unsigned int projMatLocation;		    // Location of the projectionMatrix in the shader programs.
extern unsigned int modelviewMatLocation;	    // Location of the modelviewMatrix in the shader programs.
extern unsigned int applyTextureLocation;	    // Location of the modelviewMatrix in the shader programs.

extern float matEntries[16];	// Holds 16 floats (since cannot load doubles into a shader that uses floats)


// ****
// Function prototypes
// ****

unsigned int setup_shader_vertfrag(const char* vertexShaderSource, const char* fragmentShaderSource);

GLuint check_compilation_shader(GLuint shader);
GLuint check_link_status(GLuint program);



