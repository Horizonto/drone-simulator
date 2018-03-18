/*
* GlGeomCylinder.cpp - Version 0.2 - February 25, 2018
*
* C++ class for rendering spheres in Modern OpenGL.
*   A GlGeomCylinder object encapsulates a VAO, a VBO, a VEO,
*   and possiblu a UBO
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

// Use the static library (so glew32.dll is not needed):
#define GLEW_STATIC
#include <GL/glew.h> 
#include <GLFW/glfw3.h>


#include "LinearR3.h"
#include "MathMisc.h"
#include "assert.h"

#include "GlGeomCylinder.h"

void GlGeomCylinder::InitializeAttribLocations(
	unsigned int pos_loc, unsigned int normal_loc, unsigned int texcoords_loc)
{
	posLoc = pos_loc;
	normalLoc = normal_loc;
	texcoordsLoc = texcoords_loc;

	// Decide what values to use for numSlices and numStacks
	// Default value is 6 for slices, 1 for stacks and rings.  
	// Minimum value is 3 for slices; 1 for stacks and rings
    // Maximum value is 255
	numSlices = (numSlices == 0 ? 6 : ClampRange(numSlices, 3, 255));
    numStacks = ClampRange(numStacks, 1, 255);
    numRings = ClampRange(numRings, 1, 255);

 	// Generate Vertex Array Object and Buffer Objects, not already done.
	if (theVAO == 0) {
		glGenVertexArrays(1, &theVAO);
		glGenBuffers(1, &theVBO);
		glGenBuffers(1, &theEBO);
        if (false && GLEW_ARB_multi_draw_indirect) {    // Don't use multi_draw_indirect
            glGenBuffers(1, &theIBO);
        }
	}

	// Link the VBO and EBO to the VAO, and request OpenGL to
	//   allocate memory for them.
	glBindVertexArray(theVAO);
	glBindBuffer(GL_ARRAY_BUFFER, theVBO);
	glBufferData(GL_ARRAY_BUFFER, StrideVal() * GetNumVertices() * sizeof(float), 0, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetNumElements() * sizeof(unsigned int), 0, GL_STATIC_DRAW);
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
    if (MultiDrawIndirectUsed()) {
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, theIBO);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, GetNumDraws()*sizeof(multiDrawData), 0, GL_STATIC_DRAW);
    }

	// Calculate and load the buffer data
	LoadBufferData();
}

void GlGeomCylinder::Remesh(int slices, int stacks, int rings)
{
    if (slices == numSlices && stacks == numStacks && rings == numRings) {
		return;
	}

	numSlices = ClampRange(slices, 3, 255);
	numStacks = ClampRange(stacks, 1, 255);
    numRings = ClampRange(rings, 1, 255);
	if (theVAO != 0) {
		LoadBufferData();
	}
}


// ******************************
// Calculate and load all vertex attributes into the VBO.
// Load all element indices into the EBO.
// After this is called, the sphere is ready to be rendered.
//   -- At the moment, only vertex position and normals are loaded.
// ******************************
void GlGeomCylinder::LoadBufferData() {

    // Allocate memory for vertex positions, normals, texture coordinates, and multidraw commands.
    float *tempStoref = new float[GetNumVertices() * StrideVal()];
    
    // Data is laid out: top face vertices, then bottom face vertices, then side vertices
    const int bottomStart = (1 + (numSlices+1)*numRings)*StrideVal();

    // Set top and bottom center vertices
    tempStoref[1] = 1.0;
    tempStoref[0] = tempStoref[2] = 0.0;
    tempStoref[bottomStart + 1] = -1.0;
    tempStoref[bottomStart] = tempStoref[bottomStart + 2] = 0.0;
    if (UseNormals()) {
        tempStoref[4] = 1.0f;
        tempStoref[3] = tempStoref[5] = 0.0;
        tempStoref[bottomStart + 4] = -1.0f;
        tempStoref[bottomStart + 3] = tempStoref[bottomStart + 5] = 0.0;
    }
    if (UseTexCoords()) {
        int toff = TexOffset();
        tempStoref[toff] = tempStoref[toff + 1] = 0.5f;
        tempStoref[bottomStart + toff] = tempStoref[bottomStart + toff + 1] = 0.5f;
    }

    // Set all positions and normals, radial slice by radial slice.
    int topIdx = StrideVal();
    int bottomIdx = bottomStart + StrideVal();
    for (int i = 0; i <= numSlices; i++) {
        // Handle a slice of vertices.
        // theta measures from the xz-plane, going counterclockwise viewed from above.
        float theta = ((float)i)*(float)PI2 / (float)(numSlices);
        float c = cos(theta);
        float s = sinf(theta);
        // Top & bottom face vertices, positions and normals
        for (int j = 0; j < numRings; j++) {
            float radius = (float)(j+1) / (float)numRings;
            float sinRadius = s * radius;
            float cosRadius = c * radius;
            tempStoref[topIdx] = -sinRadius;        // x coordinate
            tempStoref[bottomIdx] = sinRadius;    // x coordinate (negated, for outward facing downward)
            tempStoref[topIdx + 1] = 1.0f;           // y coordinate
            tempStoref[bottomIdx + 1] = -1.0f;       // y coordinate
            tempStoref[topIdx + 2] = -cosRadius;        // z coordinate
            tempStoref[bottomIdx + 2] = -cosRadius;     // z coordinate
            if (UseNormals()) {
                const int noff = NormalOffset();
                tempStoref[topIdx + noff + 1] = 1.0f;
                tempStoref[topIdx + noff] = tempStoref[topIdx + noff + 2] = 0.0f;
                tempStoref[bottomIdx + noff + 1] = -1.0f;
                tempStoref[bottomIdx + noff] = tempStoref[bottomIdx + noff + 2] = 0.0f;
            }
            if (UseTexCoords()) {
                const int toff = TexOffset();
                tempStoref[topIdx + toff] = -sinRadius * 0.5f + 0.5f;
                tempStoref[topIdx + toff + 1] = -cosRadius*0.5f + 0.5f;
                tempStoref[bottomIdx + toff] = sinRadius * 0.5f + 0.5f;
                tempStoref[bottomIdx + toff + 1] = -cosRadius * 0.5f + 0.5f;
            }
            topIdx += StrideVal();
            bottomIdx += StrideVal();
        }
        // Side vertices, positions and normals.
        int sideIdx = 2 * bottomStart + i*(numStacks + 1)*StrideVal();    // Immediately after bottom comes the side
        for (int j = 0; j <= numStacks; j++) {
            float jFrac = (float)j / (float)numStacks;
            tempStoref[sideIdx] = -s;
            tempStoref[sideIdx + 1] = 1.0f - 2.0f*jFrac;
            tempStoref[sideIdx + 2] = -c;
            if (UseNormals()) {
                tempStoref[sideIdx + NormalOffset()] = -s;
                tempStoref[sideIdx + NormalOffset() + 1] = 0.0f;
                tempStoref[sideIdx + NormalOffset() + 2] = -c;
            }
            if (UseTexCoords()) {
                tempStoref[sideIdx + TexOffset()] = (i == numSlices) ? 1.0f : ((float)i) / (float)numSlices;   // s texture coordinate;
                tempStoref[sideIdx + TexOffset() + 1] = 1.0f - jFrac;                        // t texture coordinate
            }
            sideIdx += StrideVal();
        }
        // Texture coordinates not yet assigned in this version of the code
    }
    glBindVertexArray(theVAO);
    glBindBuffer(GL_ARRAY_BUFFER, theVBO);
    glBufferData(GL_ARRAY_BUFFER, StrideVal() * GetNumVertices() * sizeof(float), tempStoref, GL_STATIC_DRAW);

    // Set the Element Array Buffer values.
    unsigned int* tempStorei = (unsigned int*)tempStoref;     // Can reuse the same memory to hold our element indices array
    // Set vertex indices for triangle strips from the top face, slice by slice
    // Then, do the same for the bottom face.
    unsigned int* toPtr = tempStorei;
    for (int iFace = 0; iFace < 2; iFace++) {
        // iFace = 0 or 1 for top or bottom face, respectively
        unsigned int leftIdx = iFace * (1 + numRings * (numSlices + 1));
        unsigned int firstLeftIdx = leftIdx;    // Current face's center vertex
        unsigned int rightIdx = firstLeftIdx + 1 + numRings;
        leftIdx++;                              // Skip center vertex
        for (int i = 0; i < numSlices; i++) {
            *(toPtr++) = firstLeftIdx;          // Center top or bottom vertex
            for (int j = 0; j < numRings; j++) {
                *(toPtr++) = leftIdx++;
                *(toPtr++) = rightIdx++;
            }
        }
    }

    // Set vertex indices for triangle strips for the sides, slice by slice
    const int sideStart = 2 * (1 + numRings*(numSlices+1));
    unsigned int leftIdx = sideStart;
    unsigned int rightIdx = sideStart + (numStacks + 1);
    for (int i = 0; i < numSlices; i++) {
        for (int j = 0; j <= numStacks; j++) {
            *(toPtr++) = rightIdx++;
            *(toPtr++) = leftIdx++;
        }
    }

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, theEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, GetNumElements()*sizeof(unsigned int), tempStorei, GL_STATIC_DRAW);
	delete[] tempStoref;

    if (MultiDrawIndirectUsed()) {
        mdDataPtr = new multiDrawData[GetNumDraws()];   // Hold the multi-draw commands
        int ii = 0;
        int iC = 0;
        for (int i = 0; i < 2 * numSlices; i++) {
            mdDataPtr[i].firstIndex = iC;
            mdDataPtr[i].elementCount = 2 * numRings + 1;
            mdDataPtr[i].instanceCount = 1;
            mdDataPtr[i].baseVertex = 0;
            mdDataPtr[i].baseInstance = 0;
            iC += mdDataPtr[i].elementCount;
        }
        for (int i = 2 * numSlices; i < 3*numSlices; i++) {
            mdDataPtr[i].firstIndex = iC;
            mdDataPtr[i].elementCount = 2 * numStacks + 2;
            mdDataPtr[i].instanceCount = 1;
            mdDataPtr[i].baseVertex = 0;
            mdDataPtr[i].baseInstance = 0;
            iC += mdDataPtr[i].elementCount;
        }
        check_for_opengl_errors();
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, theIBO);
        glBufferData(GL_DRAW_INDIRECT_BUFFER, GetNumDraws() * sizeof(multiDrawData), mdDataPtr, GL_STATIC_DRAW);
        check_for_opengl_errors();
        delete[] mdDataPtr;
        mdDataPtr = 0;
    }
    else {
        mdCounts  = new GLsizei[3*numSlices];
        mdIndices = new GLvoid*[3*numSlices];
        int ii = 0;
        int iC = 0;
        for (int i = 0; i < 2 * numSlices; i++) {
            mdCounts[i] = 2 * numRings + 1;
            mdIndices[i] = (void*)(iC*sizeof(unsigned int));
            iC += mdCounts[i];
        }
        for (int i = 2*numSlices; i < 3*numSlices; i++) {
            mdCounts[i] = 2 * numStacks + 2;
            mdIndices[i] = (void*)(iC * sizeof(unsigned int));
            iC += mdCounts[i];
        }
    }

    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

// **********************************************
// This routine does the rendering.
// If the sphere's VAO, VBO, EBO need to be loaded, it does this first.
// It turns on primitive restart detection, but does not turn it off. 
// **********************************************
void GlGeomCylinder::Render()
{
    assert(AssertReadyToRender());
    if (MultiDrawIndirectUsed()) {
        check_for_opengl_errors();
        glBindVertexArray(theVAO);
        glBindBuffer(GL_DRAW_INDIRECT_BUFFER, theIBO);
        check_for_opengl_errors();
        //glMultiDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, (void*)0, GetNumDraws(), 0);
        glDrawElementsIndirect(GL_TRIANGLE_STRIP, GL_UNSIGNED_INT, (void*)0);
        check_for_opengl_errors();
    }
    else {
        glBindVertexArray(theVAO);
        glMultiDrawElements(GL_TRIANGLE_STRIP, mdCounts, GL_UNSIGNED_INT, mdIndices, GetNumDraws());
    }
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
 }

void GlGeomCylinder::RenderTop()
{
    assert(AssertReadyToRender());
    assert(!MultiDrawIndirectUsed());           // MultiDrawIndirectUsed: Not implemented successfully yet!
    glBindVertexArray(theVAO);
    glMultiDrawElements(GL_TRIANGLE_STRIP, mdCounts, GL_UNSIGNED_INT, mdIndices, GetNumDrawsFace());
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

void GlGeomCylinder::RenderBase()
{
    assert(AssertReadyToRender());
    assert(!MultiDrawIndirectUsed());           // MultiDrawIndirectUsed: Not implemented successfully yet!
    glBindVertexArray(theVAO);
    int d = GetNumDrawsFace();
    glMultiDrawElements(GL_TRIANGLE_STRIP, mdCounts+d, GL_UNSIGNED_INT, mdIndices+d, d);
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

void GlGeomCylinder::RenderSide()
{
    assert(AssertReadyToRender());
    assert(!MultiDrawIndirectUsed());           // MultiDrawIndirectUsed: Not implemented successfully yet!
    glBindVertexArray(theVAO);
    int d = GetNumDrawsFace();
    glMultiDrawElements(GL_TRIANGLE_STRIP, mdCounts + 2*d, GL_UNSIGNED_INT, mdIndices + 2*d, d);
    glBindVertexArray(0);           // Good practice to unbind: helps with debugging if nothing else
}

bool GlGeomCylinder::AssertReadyToRender()
{
    bool isReady = (theVAO != 0);
    if (!isReady) {
        assert(false && "GlGeomCylinder::InitializeAttribLocations must be called before rendering!");
    }
    return isReady;
}



