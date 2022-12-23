#include "trfmStack.h"
#include <cstdio>
#include <stack>

/// TrfmStack
TrfmStack::TrfmStack() {}

TrfmStack::~TrfmStack() {}

void TrfmStack::push() {
	m_V.push(m_top);
}

Trfm3D *TrfmStack::pop() {
	m_top.clone(m_V.top());
	m_V.pop();
	return &m_top;
}

void TrfmStack::loadIdentity() {
	m_top.setUnit();
}

void TrfmStack::load(const Trfm3D * T) {
	m_top.clone(T);
}

void TrfmStack::add(const Trfm3D * T) {
	m_top.add(T);
}

Trfm3D *TrfmStack::top() {
	return &m_top;
}

const Trfm3D *TrfmStack::top() const {
	return &m_top;
}

const GLfloat *TrfmStack::getGLMatrix() {
	m_top.getGLMatrix(&m_M[0]);
	return &m_M[0];
}

void TrfmStack::print() const {
	m_top.print();
}
