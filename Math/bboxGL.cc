#include "bboxGL.h"

#include <GL/glew.h>
#include <GL/glut.h>
#include "renderState.h" // For rendering state
#include "materialManager.h"

void BBoxGL::init_opengl_vbo(BBox *thisBBox) {

	static float buffer[24];
	static GLubyte idx[] = {
		0, 1, 1, 2, 2, 3, 3, 0, // front ( 4 lines: {P0, P1} {P1, P2}, {P2, P3}, {P3, P0} )
		5, 4, 4, 7, 7, 6, 6, 5, // back
		3, 7, 2, 6, 1, 5, 0, 4, // edges between front and back
	};

	buffer[0] =  thisBBox->m_min[0]; buffer[1]  = thisBBox->m_min[1]; buffer[2]  = thisBBox->m_max[2]; // P0
	buffer[3] =  thisBBox->m_max[0]; buffer[4]  = thisBBox->m_min[1]; buffer[5]  = thisBBox->m_max[2]; // P1
	buffer[6] =  thisBBox->m_max[0]; buffer[7]  = thisBBox->m_max[1]; buffer[8]  = thisBBox->m_max[2]; // P2
	buffer[9] =  thisBBox->m_min[0]; buffer[10] = thisBBox->m_max[1]; buffer[11] = thisBBox->m_max[2]; // P3
	buffer[12] = thisBBox->m_min[0]; buffer[13] = thisBBox->m_min[1]; buffer[14] = thisBBox->m_min[2]; // P4
	buffer[15] = thisBBox->m_max[0]; buffer[16] = thisBBox->m_min[1]; buffer[17] = thisBBox->m_min[2]; // P5
	buffer[18] = thisBBox->m_max[0]; buffer[19] = thisBBox->m_max[1]; buffer[20] = thisBBox->m_min[2]; // P6
	buffer[21] = thisBBox->m_min[0]; buffer[22] = thisBBox->m_max[1]; buffer[23] = thisBBox->m_min[2]; // P7

	/* // free previous VBO/VAO */
	/* glDeleteBuffers(1, &thisBBox->vbo_id); */
	/* glDeleteBuffers(1, &thisBBox->idxvbo_id); */

	if (thisBBox->m_vbo_id == 0) {
		// create new VBO for vertices
		glGenBuffers(1, &thisBBox->m_vbo_id);
		glBindBuffer(GL_ARRAY_BUFFER, thisBBox->m_vbo_id);
		glBufferData(GL_ARRAY_BUFFER,
					 sizeof(buffer),
					 NULL,
					 GL_STREAM_DRAW);
	}
	glBindBuffer(GL_ARRAY_BUFFER, thisBBox->m_vbo_id);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(buffer), &buffer[0]);
	// upload data to VBO
	// create new VBO for indices
	if (thisBBox->m_idxvbo_id == 0) {
		glGenBuffers(1, &thisBBox->m_idxvbo_id);
		// bind VBO
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, thisBBox->m_idxvbo_id);
		// upload data to VBO
		glBufferData(GL_ELEMENT_ARRAY_BUFFER,
					 sizeof(idx),
					 &idx[0],
					 GL_STATIC_DRAW);
	}
	thisBBox->m_vbo_uptodate = 1;
}


void BBoxGL::draw(BBox * thisBBox) {

	// Variables for displaying the BBox

	if (thisBBox->m_vbo_uptodate == 0)
		init_opengl_vbo(thisBBox);

	ShaderProgram *shaderProgram = RenderState::instance()->getShader();
	if(!shaderProgram) {
		fprintf(stderr, "[W] BBoxGL::draw: no shader\n");
		return;
	}
	shaderProgram->beforeDraw();
	glPushAttrib(GL_POLYGON_BIT);

	glPolygonMode(GL_FRONT, GL_LINE);
	glPolygonMode(GL_BACK , GL_LINE);

	// Drawing
	glBindBuffer(GL_ARRAY_BUFFER, thisBBox->m_vbo_id);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, thisBBox->m_idxvbo_id);
	// Attribute specification
	glEnableVertexAttribArray(0); // 0 attrib. for vertex position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawElements(GL_LINES, 24, GL_UNSIGNED_BYTE, 0);
	glDisableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glPopAttrib();
}
