/*
 * CylinderSphere.h - Version 0.2 - February 25, 2018
*
* C++ class for rendering spheres in Modern OpenGL.
*   A GlGeomCylinder object encapsulates a VAO, a VBO, a VEO,
*   and possible a UBO
*   which can be used to render a cylinder.
*   The number of slices and stacks and rings can be varied.
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
#ifndef GLGEOM_CYLINDER_H
#define GLGEOM_CYLINDER_H

#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>

class GlGeomCylinder
{
public:
    GlGeomCylinder() : GlGeomCylinder(0, 0, 0) {}
    GlGeomCylinder(int slices, int stacks=1, int rings=1);
    ~GlGeomCylinder();

	// Allocate the VAO, VBO, EBO, and possibly the XXX.
	// Set up info about the Vertex Attribute Locations
	// This must be called before render is first called.
    // First parameter is the location for the vertex position vector in the shader program.
    // Second parameter is the location for the vertex normal vector in the shader program.
    // Third parameter is the location for the vertex 2D texture coordinates in the shader program.
    // The second and third parameters are optional.
    void InitializeAttribLocations(
		unsigned int pos_loc, unsigned int normal_loc = UINT_MAX, unsigned int texcoords_loc = UINT_MAX);

	// Re-mesh to change the number slices and stacks and rings.
	// Can be called either before or after InitAttribLocations(), but it is
	//    more efficient if Remesh() is called first, or if the constructor sets the mesh resolution.
	void Remesh(int slices, int stacks, int rings);

    void Render();          // Render: renders entire cylinder
    void RenderTop();
    void RenderBase();
    void RenderSide();

    int GetVAO() const { return theVAO; }
    int GetVBO() const { return theVBO; }
    int GetEBO() const { return theEBO; }
    int GetIBO() const { return theIBO; }

    int GetNumSlices() const { return numSlices; }
    int GetNumStacks() const { return numStacks; }
    int GetNumRings() const { return numRings; }
    int GetNumElements() const { return numSlices*(2*(2 * numRings + 1) + 2 * (numStacks + 1)); }
    int GetNumVertices() const { return 2 * ((numSlices+1)*numRings + 1) + (numSlices+1) * (numStacks + 1); }
    int GetNumDrawsFace() const { return numSlices; }
    int GetNumDraws() const { return 3 * numSlices; }

    bool MultiDrawIndirectUsed() const { return theIBO != 0; }

	// Disable all copy and assignment operators.
	// A GlGeomSphere can be allocated as a global or static variable, or with new.
	//     If you need to pass it to/from a function, use references or pointers
    //     and be use that there are no implicit copy or assignment operations!
    GlGeomCylinder(const GlGeomCylinder&) = delete;
    GlGeomCylinder& operator=(const GlGeomCylinder&) = delete;
    GlGeomCylinder(GlGeomCylinder&&) = delete;
    GlGeomCylinder& operator=(GlGeomCylinder&&) = delete;

private: 
    void LoadBufferData();
    bool AssertReadyToRender();

    // Stride value, and offset values for the data in the VBO.
    //   These take into account whether normals and texture coordinates are used.
    bool UseNormals() const { return normalLoc != UINT_MAX; }
    bool UseTexCoords() const { return texcoordsLoc != UINT_MAX; }
    int StrideVal() const {
        return 3 + (UseNormals() ? 3 : 0) + (UseTexCoords() ? 2 : 0);
    }
    int NormalOffset() const { return 3; }
    int TexOffset() const { return 3 + (UseNormals() ? 3 : 0); }
 
private:
    unsigned int theVAO = 0;        // Vertex Array Object
    unsigned int theVBO = 0;        // Vertex Buffer Object
    unsigned int theEBO = 0;        // Element Buffer Object;
    unsigned int theIBO = 0;        // Draw Indirect Buffer Object

	unsigned int posLoc;            // location of vertex position x,y,z data in the shader program
	unsigned int normalLoc;         // location of vertex normal data in the shader program
	unsigned int texcoordsLoc;      // location of s,t texture coordinates in the shader program.

    int numSlices;          // Number of radial slices (like cake slices
    int numStacks;          // Number of stacks between the two end faces
    int numRings;           // Number of concentric rings on two end faces

private:
    struct multiDrawData {
        GLuint elementCount;    // Number of elements, i.e. vertices
        GLuint instanceCount;   // Number of instances
        GLuint firstIndex;      // First element (starting position in the EBO)
        GLuint baseVertex;      // Base vertex
        GLuint baseInstance;    // Base instance
    };
    multiDrawData* mdDataPtr = 0;

    GLsizei* mdCounts = 0;
    GLvoid** mdIndices = 0;
};

inline GlGeomCylinder::GlGeomCylinder(int slices, int stacks, int rings)
{
	numSlices = slices;
	numStacks = stacks;
    numRings = rings;
}

inline GlGeomCylinder::~GlGeomCylinder()
{
    int numToDelete = MultiDrawIndirectUsed() ? 4 : 3;
    glDeleteBuffers(numToDelete, &theVAO);  // The buffer id's are contigous in memory! 

    delete[] mdDataPtr;
}

bool check_for_opengl_errors();

#endif  // GLGEOM_CYLINDER_H
