/*
* GlGeomSphere.h - Version 0.3 - February 25, 2018
*
* C++ class for rendering spheres in Modern OpenGL.
*   A GlGeomSphere object encapsulates a VAO, VBO, and VEO,
*   which can be used to render a sphere.
*   The number of slices and stacks can be varied.
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

#pragma once
#ifndef GLGEOM_SPHERE_H
#define GLGEOM_SPHERE_H

#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

class GlGeomSphere
{
public:
    GlGeomSphere() : GlGeomSphere(0, 0) {}
    GlGeomSphere(int slices, int stacks);
    ~GlGeomSphere();

	// Allocate the VAO, VBO, and EBO.
	// Set up info about the Vertex Attribute Locations
	// This must be called before render is first called.
    // First parameter is the location for the vertex position vector in the shader program.
    // Second parameter is the location for the vertex normal vector in the shader program.
    // Third parameter is the location for the vertex 2D texture coordinates in the shader program.
    // The second and third parameters are optional.
    void InitializeAttribLocations(
		unsigned int pos_loc, unsigned int normal_loc = UINT_MAX, unsigned int texcoords_loc = UINT_MAX);

	// Remesh: re-mesh to change the number slices and stacks.
	// Can be called either before or after InitAttribLocations(), but it is
	//    more efficient if Remesh() is called first, or if the constructor sets the mesh resolution.
	void Remesh(int slices, int stacks);

	void Render();
 
    int GetVAO() const { return theVAO; }
    int GetVBO() const { return theVBO; }
    int GetEBO() const { return theEBO; }

    int GetNumSlices() const { return numSlices; }
    int GetNumStacks() const { return numStacks; }
    int GetNumElements() const { return numSlices*(2*numStacks+1); }
    int GetNumVertices() const { return (numSlices+1)*(numStacks-1)+2; }

	// Disable all copy and assignment operators.
	// A GlGeomSphere can be allocated as a global or static variable, or with new.
    //     If you need to pass it to/from a function, use references or pointers
    //     and be use that there are no implicit copy or assignment operations!
    GlGeomSphere(const GlGeomSphere&) = delete;
	GlGeomSphere& operator=(const GlGeomSphere&) = delete;
	GlGeomSphere(GlGeomSphere&&) = delete;
	GlGeomSphere& operator=(GlGeomSphere&&) = delete;

private: 
    void LoadBufferData();
    unsigned short PrimRestartIndex = USHRT_MAX;        // Use for primitive restarts (starting new triangle strips)

private:
    unsigned int theVAO = 0;        // Vertex Array Object
    unsigned int theVBO = 0;        // Vertex Buffer Object
    unsigned int theEBO = 0;        // Element Buffer Object;

    // Stride value, and offset values for the data in the VBO.
    // These take into account whether normals and texture coordinates are used.
    bool UseNormals() const { return normalLoc != UINT_MAX; }
    bool UseTexCoords() const { return texcoordsLoc != UINT_MAX; }
    int StrideVal() const {
        return 3 + (UseNormals() ? 3 : 0) + (UseTexCoords() ? 2 : 0);
    }
    int NormalOffset() const { return 3; }
    int TexOffset() const { return 3 + (UseNormals() ? 3 : 0); }

    unsigned int posLoc;            // location of vertex position x,y,z data in the shader program
    unsigned int normalLoc;         // location of vertex normal data in the shader program
    unsigned int texcoordsLoc;      // location of s,t texture coordinates in the shader program.

    int numSlices;              // Number of radial slices (like case slices)
    int numStacks;              // Number of levels separating the north pole from the south pole.
};

inline GlGeomSphere::GlGeomSphere(int slices, int stacks)
{
	numSlices = slices;
	numStacks = stacks;
}

inline GlGeomSphere::~GlGeomSphere() 
{
    glDeleteBuffers(3, &theVAO);  // The three buffer id's are contigous in memory!
}



#endif  // GLGEOM_SPHERE_H
