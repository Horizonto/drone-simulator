/*
* GlGeomSphere.cpp - Version 0.3 - February 25 2018
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

// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>


#include "LinearR3.h"
#include "MathMisc.h"
#include "assert.h"

#include "GlGeomSphere.h"

void GlGeomSphere::InitializeAttribLocations(
	unsigned int pos_loc, unsigned int normal_loc, unsigned int texcoords_loc)
{
	posLoc = pos_loc;
	normalLoc = normal_loc;
	texcoordsLoc = texcoords_loc;

	// Decide what values to use for numSlices and numStacks
	// Default value is 6.  
	// Minimum value is 3.
    // Maximum value is 255 -- Allows use of unsigned shorts for elements.
	numSlices = (numSlices == 0 ? 6 : ClampRange(numSlices, 3, 255));
	numStacks = (numStacks == 0 ? 6 : ClampRange(numStacks, 2, 255));

 	// Generate Vertex Array Object and Buffer Objects, not already done.
	if (theVAO == 0) {
		glGenVertexArrays(1, &theVAO);
		glGenBuffers(1, &theVBO);
		glGenBuffers(1, &theEBO);
	}

	// Link the VBO and EBO to the VAO, and request OpenGL to
	//   allocate memory for them.
	glBindVertexArray(theVAO);
	glBindBuffer(GL_ARRAY_BUFFER, theVBO);
	glBufferData(GL_ARRAY_BUFFER, StrideVal() * GetNumVertices() * sizeof(float), 0, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetNumElements() * sizeof(unsigned short), 0, GL_STATIC_DRAW);
	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, StrideVal() * sizeof(float), (void*)0);
	glEnableVertexAttribArray(posLoc);
	if (UseNormals()) {
		glVertexAttribPointer(normalLoc, 3, GL_FLOAT, GL_FALSE, StrideVal() * sizeof(float), 
                              (void*)(NormalOffset() * sizeof(float)));
		glEnableVertexAttribArray(normalLoc);
	}
	if (UseTexCoords()) {
		glVertexAttribPointer(texcoordsLoc, 2, GL_FLOAT, GL_FALSE, StrideVal() * sizeof(float), 
                              (void*)(TexOffset() * sizeof(float)));
		glEnableVertexAttribArray(texcoordsLoc);
	}

	// Calculate the buffer data
	LoadBufferData();
}

void GlGeomSphere::Remesh(int slices, int stacks)
{
	if (slices == numSlices && stacks == numStacks) {
		return;
	}

	numSlices = ClampRange(slices, 3, 255);
	numStacks = ClampRange(stacks, 3, 255);
	if (theVAO != 0) {
		LoadBufferData();
	}
}


// ******************************
// Calculate and load all vertex attributes into the VBO.
// Load all element indices into the EBO.
// After this is called, the sphere is ready to be rendered.
// ******************************
void GlGeomSphere::LoadBufferData() {

    // Allocate memory for vertex positions, normals, and texture coordinates.
    float *tempStoref = new float[GetNumVertices() * StrideVal()];
    
    // Set North pole and South pole positions and normals.
    // The north pole is on the positive y-axis.
    tempStoref[1] = 1.0f;       // North pole, y position
    tempStoref[0] = tempStoref[2] = 0.0f;
    int s = 3;
    if ( UseNormals() ) {
        tempStoref[s+1] = 1.0f;       // North pole, normal, y component.
        tempStoref[s] = tempStoref[s + 2] = 0.0f;
        s += 3;
     }
    if (UseTexCoords()) {
        tempStoref[s] = 0.5f;           // s texture coordinate, North pole
        tempStoref[s + 1] = 1.0f;       // t texture coordinate
        s += 2;
    }
    tempStoref[s + 1] = -1.0f;       // South pole, y position
    tempStoref[s] = tempStoref[s + 2] = 0.0f;
    s += 3;
    if (UseNormals()) {
        tempStoref[s + 1] = -1.0f;       // South pole, normal, y component.
        tempStoref[s] = tempStoref[s + 2] = 0.0f;
        s += 3;
    }
    if (UseTexCoords()) {
        tempStoref[s] = 0.5f;           // s texture coordinate, South pole
        tempStoref[s + 1] = 0.0f;       // t texture coordinate
        s += 2;
    }

    // Set the positions and normals for the rest of the vertices
    for (int i = 0; i <= numSlices; i++) {
        // Handle a slice of vertices.
        // theta measures from the xz-plane, going counterclockwise viewed from above.
        float theta = ((float)(i%numSlices))*(float)PI2 / (float)(numSlices);
        float costheta = cos(theta);
        float sintheta = sinf(theta);
        for (int j = 1; j < numStacks; j++) {
            float fracJ = ((float)j) / (float)(numStacks);
            float phi = fracJ * (float)PI;
            float cosphi = cosf(phi);
            float sinphi = sinf(phi);
            float tTex = 1.0f - fracJ;                // Use for spherical coordinates
            // float tTex = cosphi * 0.5f + 0.5f;         // Use for cylindrical texture coordinates
            tempStoref[s] = -sintheta*sinphi;       // Position, x coordinate            
            tempStoref[s + 1] = cosphi;            // Position, y coordinate
            tempStoref[s + 2] = -costheta*sinphi;   // Position, z coordinate
            s += 3;
            if (UseNormals()) {
                tempStoref[s] = tempStoref[s - 3];     // Normal, x coordinate
                tempStoref[s + 1] = tempStoref[s - 2]; // Normal, y coordinate
                tempStoref[s + 2] = tempStoref[s - 1]; // Normal, z coordinate
                s += 3;
            }
            if (UseTexCoords()) {
                float tTex = 1.0f - fracJ;                // Use for spherical coordinates
                // float tTex = cosphi * 0.5 + 0.5;         // Use for cylindrical texture coordinates
                tempStoref[s] = (i==numSlices) ? 1.0f : ((float)i) / (float)numSlices;   // s texture coordinate
                tempStoref[s + 1] = tTex;                                                // t texture coordinate
                s += 2;
            }
        }
    }
    glBindVertexArray(theVAO);
    glBindBuffer(GL_ARRAY_BUFFER, theVBO);
    glBufferData(GL_ARRAY_BUFFER, StrideVal()*GetNumVertices()*sizeof(float), tempStoref, GL_STATIC_DRAW);

    unsigned short* tempStorei = (unsigned short*)tempStoref;     // Can reuse the same memory to hold our element indices array
    unsigned short* toPtr = tempStorei;                // Load up data sequentially
    // Load all the data for the EBO.
    for (int i = 0; i < numSlices; i++) {
        // Handle a slice of vertices.
        unsigned short leftSideIdx = (i + 1)*(numStacks - 1) + 2;    // First vertex index in slice i
        unsigned short rightSideIdx = i*(numStacks - 1) + 2;         // First vertex index in slice i+1
        *(toPtr++) = 0;                                     // Index for North Pole
        for (int j = 1; j < numStacks; j++) {
            *(toPtr++) = rightSideIdx++;                     // Triangle strip vertex on the left
            *(toPtr++) = leftSideIdx++;                    // Triangle strip vertex on the right
        }
		*(toPtr++) = 1;										// Index for the South Pole
		*(toPtr++) = PrimRestartIndex;                         // Indicate the end of this triangle strip.
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetNumElements()*sizeof(unsigned short), tempStorei, GL_STATIC_DRAW);

	delete[] tempStoref;
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

// **********************************************
// This routine does the rendering.
// If the sphere's VAO, VBO, EBO need to be loaded, it does this first.
// It turns on primitive restart detection and then turns it off. 
// **********************************************
void GlGeomSphere::Render()
{
    if (theVAO == 0) {
        assert(false && "GlGeomSphere::InitializeAttribLocations must be called before rendering!");
    }
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(PrimRestartIndex);
    glBindVertexArray(theVAO);
	glDrawElements(GL_TRIANGLE_STRIP, (GLsizei)GetNumElements(), GL_UNSIGNED_SHORT, 0);
	glDisable(GL_PRIMITIVE_RESTART);     // Not clear that there is any reason to disable (maybe for performance?)
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}


