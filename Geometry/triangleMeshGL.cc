#include "triangleMeshGL.h"
#include "renderState.h" // For rendering state
#include "shader.h"

// This module renders a triangleMesh using openGL as a backend.

struct Vbo_vertex {
	GLfloat v[3];
	GLfloat n[3];
	GLfloat t[2];
	GLfloat tbn_t[3];
	GLfloat tbn_b[3];
};

#define VBO_BUFFER_OFFSET(i) ((char *)NULL + (i))

void TriangleMeshGL::init_opengl_vbo(TriangleMesh * thisMesh) {

	float *v;

	// free previous VBO/VAO
	glDeleteBuffers(1, &thisMesh->m_vbo_id);
	glDeleteVertexArrays(1, &thisMesh->m_vao_id);
	size_t vIndices_n = thisMesh->m_vIndices.size();
	Vbo_vertex *buffer = new Vbo_vertex[ vIndices_n ];
	for(size_t i = 0; i < vIndices_n; ++i) {
		v = thisMesh->vCoords( thisMesh->m_vIndices[i] );
		buffer[i].v[0] = v[0];
		buffer[i].v[1] = v[1];
		buffer[i].v[2] = v[2];
	}

	for(size_t i = 0; i < vIndices_n; ++i) {
		v = thisMesh->nCoords( thisMesh->m_nIndices[i] );
		buffer[i].n[0] = v[0];
		buffer[i].n[1] = v[1];
		buffer[i].n[2] = v[2];
	}
	if(thisMesh->m_type & TriangleMesh::texcoords) {
		for(size_t i = 0; i < vIndices_n; ++i) {
			// textures
			v = thisMesh->texCoords( thisMesh->m_texIndices[i] );
			buffer[i].t[0] = v[0];
			buffer[i].t[1] = v[1];
			if(thisMesh->m_type & TriangleMesh::bump) {
				// TBN: tangents
				v = thisMesh->tgtCoords( thisMesh->m_tgtIndices[i] );
				buffer[i].tbn_t[0] = v[0];
				buffer[i].tbn_t[1] = v[1];
				buffer[i].tbn_t[2] = v[2];
				// TBN: bitangents
				v = thisMesh->btgtCoords( thisMesh->m_btgtIndices[i] );
				buffer[i].tbn_b[0] = v[0];
				buffer[i].tbn_b[1] = v[1];
				buffer[i].tbn_b[2] = v[2];
			}
		}
	}

	glGenVertexArrays(1, &thisMesh->m_vao_id);
	// bind new VAO to the conetxt
	glBindVertexArray(thisMesh->m_vao_id);
	// create new VBO
	glGenBuffers(1, &thisMesh->m_vbo_id);
	// bind VBO
	glBindBuffer(GL_ARRAY_BUFFER, thisMesh->m_vbo_id);
	// upload data to VBO
	glBufferData(GL_ARRAY_BUFFER,
				 vIndices_n * sizeof(Vbo_vertex),
				 buffer,
				 GL_STATIC_DRAW);
	// Attribute specification
	// 0-11 (vertex-position) 12-23 (normal) 24-31 (texture coord) 32-43 (TBN tangent)
	// 44-55 (TBN bitangent) 56-67 (TBN normal)
	glEnableVertexAttribArray(0); // 0 attrib. for vertex position (3 x 4 = 12)
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vbo_vertex), VBO_BUFFER_OFFSET(0));
	glEnableVertexAttribArray(1); // 1 attrib. for vertex normal (3 x 4 = 12)
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vbo_vertex), VBO_BUFFER_OFFSET(12));
	if(thisMesh->m_type & TriangleMesh::texcoords) {
		glEnableVertexAttribArray(2); // 2 attrib. for texture coord. (2 x 4 = 8)
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vbo_vertex), VBO_BUFFER_OFFSET(24));
	}
	if(thisMesh->m_type & TriangleMesh::bump) {
		glEnableVertexAttribArray(3); // 3 attrib. for TBN tangent coord.  (3 x 4 = 12)
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vbo_vertex), VBO_BUFFER_OFFSET(32));
		glEnableVertexAttribArray(4); // 4 attrib. for TBN bitangent coord. (3 x 4 = 12)
		glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vbo_vertex), VBO_BUFFER_OFFSET(44));
	}
	// unbind VBO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	// unbind VAO
	glBindVertexArray(0);
	delete [] buffer;
	thisMesh->m_vbo_uptodate = 1;
}

#undef VBO_BUFFER_OFFSET

void TriangleMeshGL::draw(TriangleMesh * thisMesh) {

	if (!thisMesh->numVertices()) return;
	RenderState *rs = RenderState::instance();
	ShaderProgram *shaderProgram = rs->getShader();

	if(!shaderProgram) {
		fprintf(stderr, "[E] TriangleMeshGL::draw: no active shader\n");
		exit(1);
	}
	if (thisMesh->m_vbo_uptodate == 0)
		init_opengl_vbo(thisMesh);

	if (thisMesh->m_materialFront) {
		rs->setFrontMaterial(thisMesh->m_materialFront);
	}

	if (thisMesh->m_materialBack) {
		rs->setBackMaterial(thisMesh->m_materialBack);
	}

	shaderProgram->beforeDraw();

	glBindVertexArray(thisMesh->m_vao_id);
	glDrawArrays(GL_TRIANGLES,
				 0,
				 thisMesh->m_vIndices.size());
	glBindVertexArray(0);
}
