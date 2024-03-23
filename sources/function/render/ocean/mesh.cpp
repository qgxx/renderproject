#include "mesh.h"

oMesh::oMesh() {
    numSubsets = 0;
    subsetTable = nullptr;
    numVertices = 0;
    numIndices = 0;
    meshOptions = 0;
    VAO = VBO = EBO = 0;
    materials = nullptr;
}

oMesh::~oMesh() {
    Destroy();
}

void oMesh::Destroy() {
    delete[] vertexDecl.Elements;

	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    delete[] subsetTable;
    delete[] materials;
    subsetTable = nullptr;
    materials = nullptr;
    numSubsets = 0;
}

bool GLCreateMesh(GLuint numv, GLuint numi, GLuint options, OceanVertexElement* decl, oMesh** mesh) {
    oMesh* omesh = new oMesh();
    int num_decl_elements = 0;
    for (int i = 0; i < 16; i++) {
        ++num_decl_elements;
        if (decl[i].Stream == 0xff) break;
    }

    glGenBuffers(1, &omesh->VBO);
    
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
    omesh->subsetTable->vertexCount = (numi > 0 ? 0 : numv);
    omesh->subsetTable->vertexStart = 0;
    omesh->subsetTable->enabled = GL_TRUE;

    omesh->vertexDecl.Elements = new OceanVertexElement[num_decl_elements];
    memcpy(omesh->vertexDecl.Elements, decl, num_decl_elements * sizeof(OceanVertexElement));

    // VAO...
    omesh->RecreateVertexLayout();
    assert(omesh->vertexDecl.Stride != 0);

	// allocate storage
	GLenum usage = ((options & OMESH_DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
	GLuint istride = ((options & OMESH_32BIT) ? 4 : 2);

	glBindBuffer(GL_ARRAY_BUFFER, omesh->VBO);
	glBufferData(GL_ARRAY_BUFFER, numv * omesh->vertexDecl.Stride, 0, usage);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	if (numi > 0) {
		glGenBuffers(1, &omesh->EBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, omesh->EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, numi * istride, 0, usage);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
    (*mesh) = omesh;

    return true;
}

void oMesh::RecreateVertexLayout() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    glGenVertexArrays(1, &VAO);
    vertexDecl.Stride = 0;

    // calculate stride
    for (int i = 0; i < 16; ++i) {
        OceanVertexElement& elem = vertexDecl.Elements[i];
        if (elem.Stream == 0xff) break;
        switch (elem.Type) {
            case GLDECLTYPE_GLCOLOR:
            case GLDECLTYPE_FLOAT1:	vertexDecl.Stride += 4;	break;
            case GLDECLTYPE_FLOAT2:	vertexDecl.Stride += 8;	break;
            case GLDECLTYPE_FLOAT3:	vertexDecl.Stride += 12; break;
            case GLDECLTYPE_FLOAT4:	vertexDecl.Stride += 16; break;
            default: break;
        }
    }
    glBindVertexArray(VAO);
    {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // bind locations
        for (int i = 0; i < 16; ++i) {
            OceanVertexElement& elem = vertexDecl.Elements[i];

            if (elem.Stream == 0xff) break;

            glEnableVertexAttribArray(elem.Usage);

            switch (elem.Usage) {
                case GLDECLUSAGE_POSITION:
                    glVertexAttribPointer(elem.Usage, (elem.Type == GLDECLTYPE_FLOAT4 ? 4 : 3), GL_FLOAT, GL_FALSE, vertexDecl.Stride, reinterpret_cast<const GLvoid*>(elem.Offset));
                    break;

                case GLDECLUSAGE_COLOR:
                    glVertexAttribPointer(elem.Usage, 4, GL_UNSIGNED_BYTE, GL_TRUE, vertexDecl.Stride, reinterpret_cast<const GLvoid*>(elem.Offset));
                    break;

                case GLDECLUSAGE_NORMAL:
                    glVertexAttribPointer(elem.Usage, (elem.Type == GLDECLTYPE_FLOAT4 ? 4 : 3), GL_FLOAT, GL_FALSE, vertexDecl.Stride, reinterpret_cast<const GLvoid*>(elem.Offset));
                    break;

                case GLDECLUSAGE_TEXCOORD:
                    // haaack...
                    glVertexAttribPointer(elem.Usage + elem.UsageIndex, (elem.Type + 1), GL_FLOAT, GL_FALSE, vertexDecl.Stride, reinterpret_cast<const GLvoid*>(elem.Offset));
                    break;

                case GLDECLUSAGE_TANGENT:
                    glVertexAttribPointer(elem.Usage, 3, GL_FLOAT, GL_FALSE, vertexDecl.Stride,reinterpret_cast<const GLvoid*>(elem.Offset));
                    break;

                case GLDECLUSAGE_BINORMAL:
                    glVertexAttribPointer(elem.Usage, 3, GL_FLOAT, GL_FALSE, vertexDecl.Stride, reinterpret_cast<const GLvoid*>(elem.Offset));
                    break;

                default:
                    std::cout << "Unhandled layout element...\n";
                    break;
                }
        }
    }
    glBindVertexArray(0);
}

bool oMesh::LockVertexBuffer(GLuint offset, GLuint size, GLuint flags, void** data) {
    if (offset >= numVertices * vertexDecl.Stride) {
        (*data) = nullptr;
        return false;
    }

    if (size == 0) size = numVertices * vertexDecl.Stride - offset;
    if (flags == 0) flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    vertexdata_locked.ptr = glMapBufferRange(GL_ARRAY_BUFFER, offset, size, flags);
    vertexdata_locked.flags = flags;
    if (!vertexdata_locked.ptr) return false;
    (*data) = vertexdata_locked.ptr;

    return true;
}

bool oMesh::LockIndexBuffer(GLuint offset, GLuint size, GLuint flags, void** data) {
	GLuint istride = ((meshOptions & OMESH_32BIT) ? 4 : 2);
	if (offset >= numIndices * istride) {
		(*data) = nullptr;
		return false;
	}

	if (size == 0) size = numIndices * istride - offset;
	if (flags == 0) flags = GL_MAP_READ_BIT | GL_MAP_WRITE_BIT;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	indexdata_locked.ptr = glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, offset, size, flags);
	indexdata_locked.flags = flags;
	if (!indexdata_locked.ptr) return false;
	(*data) = indexdata_locked.ptr;

	return true;
}

void oMesh::UnlockVertexBuffer() {
    if (vertexdata_locked.ptr != nullptr && VBO != 0) {
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glUnmapBuffer(GL_ARRAY_BUFFER);
        vertexdata_locked.ptr = nullptr;
    }
}

void oMesh::UnlockIndexBuffer() {
    if (indexdata_locked.ptr != nullptr && EBO != 0) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        indexdata_locked.ptr = nullptr;
    }
}

void oMesh::SetAttributeTable(const OceanAttribute* table, GLuint size) {
	delete[] subsetTable;
	subsetTable = new OceanAttribute[size];
	memcpy(subsetTable, table, size * sizeof(OceanAttribute));
	numSubsets = size;
}

void oMesh::DrawSubset(GLuint subset, bool bindtextures) {
	if (VAO == 0 || numVertices == 0) return;

	if (subsetTable != nullptr && subset < numSubsets) {
		const OceanAttribute& attr = subsetTable[subset];
		const OceanMaterial& mat = materials[subset];

		if (!attr.enabled)
			return;

		GLenum itype = (meshOptions & OMESH_32BIT) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
		GLuint start = attr.indexStart * ((meshOptions & OMESH_32BIT) ? 4 : 2);

		if (bindtextures) {
			if (mat.Texture != 0) {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, mat.Texture);
			}

			if (mat.NormalMap != 0) {
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, mat.NormalMap);
			}
		}

		glBindVertexArray(VAO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

		if (attr.indexCount == 0) {
			glDrawArrays(attr.primitiveType, attr.vertexStart, attr.vertexCount);
		} else {
			if (attr.vertexCount == 0) {
				glDrawElements(attr.primitiveType, attr.indexCount, itype, (char*)0 + start);
            }
			else {
				glDrawRangeElements(attr.primitiveType, attr.vertexStart, attr.vertexStart + attr.vertexCount - 1, attr.indexCount, itype, (char*)0 + start);
            }
		}
	}
}