#ifndef __O_MESH_H__
#define __O_MESH_H__

#include <glm/glm.hpp>
#include <glad/glad.h>

struct OceanAttribute {
    GLenum primitiveType;
    GLuint attributeId;
    GLuint indexStart;
    GLuint indexCount;
    GLuint vertexStart;
    GLuint vertexCount;
    GLboolean enabled;
};

struct OceanMaterial {
    glm::vec3 diffuse;
    glm::vec3 ambient;
};

enum OceanDeclType {
	GLDECLTYPE_FLOAT1 =  0,
	GLDECLTYPE_FLOAT2 =  1,
	GLDECLTYPE_FLOAT3 =  2,
	GLDECLTYPE_FLOAT4 =  3,
	GLDECLTYPE_GLCOLOR =  4
};

enum OceanDeclUsage {
	GLDECLUSAGE_POSITION = 0,
	GLDECLUSAGE_BLENDWEIGHT,
	GLDECLUSAGE_BLENDINDICES,
	GLDECLUSAGE_NORMAL,
	GLDECLUSAGE_PSIZE,
	GLDECLUSAGE_TEXCOORD,
	GLDECLUSAGE_TANGENT = GLDECLUSAGE_TEXCOORD + 8,
	GLDECLUSAGE_BINORMAL,
	GLDECLUSAGE_TESSFACTOR,
	GLDECLUSAGE_POSITIONT,
	GLDECLUSAGE_COLOR,
	GLDECLUSAGE_FOG,
	GLDECLUSAGE_DEPTH,
	GLDECLUSAGE_SAMPLE
};

struct OceanVertexElement {
	GLushort	Stream;
	GLushort	Offset;
	GLubyte		Type;   // OceanDeclType
	GLubyte		Usage;  // OceanDeclUsage
	GLubyte		UsageIndex;
};

struct OceanVertexDeclaration {
	OceanVertexElement*	Elements;	
	GLuint Stride;
};


enum OceanMeshFlags
{
	OMESH_DYNAMIC = 1,
	OMESH_32BIT = 2
};

class oMesh {
public:
    friend bool GLCreateMesh(GLuint, GLuint, GLuint, oMesh**);
    ~oMesh();

private:
    OceanAttribute* subsetTable;
    OceanMaterial* materials;

    GLuint meshOptions;
    GLuint numSubsets;
    GLuint numVertices;
    GLuint numIndices;

    unsigned int VAO, VBO, EBO;

    oMesh() {}
};

#endif  // !__O_MESH_H__