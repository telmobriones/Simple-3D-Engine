// -*-C++-*-

#pragma once

#include <GL/glew.h> // gor GLfloat
#include <GL/freeglut.h> // gor GLfloat
#include <stack>
#include "trfm3D.h"

/**
 * @file   TrfmStack.h
 *
 * @brief  Emulation of the openGL matrix stack.
 */

class TrfmStack {

public:

	TrfmStack();
	~TrfmStack();

	/**
	 * Push a copy of the top element into the stack
	 */
	void push();

	/**
	 * Remove one element of the stack, and place it on top
	 * @return the removed element, which is now on top.
	 */
	Trfm3D *pop();

	/**
	 * Get the top of the stack
	 *
	 * @param st the stack
	 *
	 * @return the top element
	 */
	Trfm3D *top();

	/**
	 * Get the top of the stack
	 *
	 * @param st the stack
	 *
	 * @return the top element
	 */
	const Trfm3D *top() const;

	/**
	 * Set the top element as the identity trfm
	 */
	void loadIdentity();

	/**
	 * Load a Trfm3D to the top element
	 */
	void load(const Trfm3D * T);

	/**
	 * Compose top element with given Trfm3D, so that
	 *
	 * new_top = top * T
	 *
	 */
	void add(const Trfm3D * T);

	/**
	 * Get a, openGL matrix representation of the top element.
	 *
	 * @return a vector of 16 elements representing the oprenGL 4x4 matrix of the
	 * top element.
	 */
	const GLfloat *getGLMatrix();

	/**
	 * Print the top Trfm3D
	 */
	void print() const;

private:
	// You cannot copy stacks
	TrfmStack & operator =(const TrfmStack & o);
	TrfmStack(const TrfmStack & o);

	std::stack<Trfm3D> m_V;
	Trfm3D m_top;
	float m_M[16];
};
