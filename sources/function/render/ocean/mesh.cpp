#include "mesh.h"

bool GLCreateMesh(GLuint numv, GLuint numi, GLuint options, oMesh** mesh) {
    oMesh* omesh = new oMesh();
    
    if (numv >= 0xffff) options |= OMESH_32BIT;
    omesh->meshOptions = options;
    omesh->numSubsets = 1;
    omesh->numVertices = numv;
    omesh->numIndices = numi;
    omesh->subsetTable = new OceanAttribute[1];

    omesh->subsetTable->attributeId = 0;
    omesh->subsetTable->indexCount = numi;
    omesh->subsetTable->indexStart = 0;
    omesh->subsetTable->primitiveType = GL_TRIANGLES;
    omesh->subsetTable->vertexCount = (numv > 0 ? 0 : numv);
    omesh->subsetTable->vertexStart = 0;
    omesh->subsetTable->enabled = GL_TRUE;

    // VAO...
    glGenVertexArrays(1, &omesh->VAO);
    glBindVertexArray(omesh->VAO);
    glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, (void*)0);

	// allocate storage
	GLenum usage = ((options & OMESH_DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	GLuint istride = ((options & OMESH_32BIT) ? 4 : 2);

	glBindBuffer(GL_ARRAY_BUFFER, omesh->VBO);
	glBufferData(GL_ARRAY_BUFFER, numv * sizeof(float) * 3, 0, usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (numv > 0) {
		glGenBuffers(1, &omesh->EBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, omesh->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numv * sizeof(unsigned int), 0, usage);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
    (*mesh) = omesh;

    return true;
}