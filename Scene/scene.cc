#include <GL/glew.h>
#include <GL/glut.h>
#include <string>
#include "scene.h"
#include "renderState.h"
#include "shaderManager.h"
#include "nodeManager.h"

Scene * Scene::instance() {
	static Scene inst;
	return &inst;
}

Scene::Scene() {
	m_rootNode = NodeManager::instance()->create("MG_ROOTNODE");
	ShaderProgram *rootShader = ShaderManager::instance()->find("dummy");
	if(!rootShader)
		rootShader = ShaderManager::instance()->create("dummy", "Shaders/dummy.vert", "Shaders/dummy.frag");
	m_rootNode->attachShader(rootShader);
	glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
}

Scene::~Scene() {}

// Node Management

Node *Scene::rootNode() {
	return m_rootNode;
}

void Scene::setShading(GLenum face, GLenum mode) {
	glPolygonMode(face, mode);
}

void Scene::attach(Node *theNode) {
	m_rootNode->addChild(theNode);
}

void Scene::print_scene_trfm() const {
	m_rootNode->print_trfm();
}

// TODO: deal with transparent objects

void Scene::draw() {
	if (m_rootNode) {
		RenderState::instance()->setShader(m_rootNode->getShader());
		m_rootNode->draw();
	}
}
