#include <vector>
#include "skybox.h"
#include "tools.h"
#include "vector3.h"
#include "trfm3D.h"
#include "renderState.h"
#include "gObjectManager.h"
#include "nodeManager.h"
#include "textureManager.h"
#include "materialManager.h"
#include "shaderManager.h"


using std::vector;
using std::string;

// TODO: create skybox object given gobject, shader name of cubemap texture.
//
// This function does the following:
//
// - Create a new material.
// - Assign cubemap texture to material.
// - Assign material to geometry object gobj
// - Create a new Node.
// - Assign shader to node.
// - Assign geometry object to node.
// - Set sky node in RenderState.
//
// Parameters are:
//
//   - gobj: geometry object to which assign the new material (which incluides
//           cubemap texture)
//   - skyshader: The sky shader.
//   - ctexname: The name of the cubemap texture.
//
// Useful functions:
//
//  - MaterialManager::instance()->create(const std::string & matName): create a
//    new material with name matName (has to be unique).
//  - Material::setTexture(Texture *tex): assign texture to material.
//  - GObject::setMaterial(Material *mat): assign material to geometry object.
//  - NodeManager::instance()->create(const std::string &nodeName): create a new
//    node with name nodeName (has to be unique).
//  - Node::attachShader(ShaderProgram *theShader): attach shader to node.
//  - Node::attachGobject(GObject *gobj ): attach geometry object to node.
//  - RenderState::instance()->setSkybox(Node * skynode): Set sky node.

void CreateSkybox(GObject *gobj,
				  ShaderProgram *skyshader,
				  const std::string &ctexname) {
	if (!skyshader) {
		fprintf(stderr, "[E] Skybox: no sky shader\n");
		exit(1);
	}
	Texture *ctex = TextureManager::instance()->find(ctexname);
	if (!ctex) {
		fprintf(stderr, "[E] Cubemap texture '%s' not found\n", ctexname.c_str());
		std::string S;
		for(auto it = TextureManager::instance()->begin();
			it != TextureManager::instance()->end(); ++it)
			S += "'"+it->getName() + "' ";
		fprintf(stderr, "...avaliable textures are: ( %s)\n", S.c_str());
		exit(1);
	}
	/* =================== PUT YOUR CODE HERE ====================== */
	// - Create a new material.
	Material *skyBMat = MaterialManager::instance()->create("skyBMat");
	// - Assign cubemap texture to material.
	skyBMat->setTexture(ctex);
	// - Assign material to geometry object gobj
	gobj->setMaterial(skyBMat);
	// - Create a new Node.
	Node *skyBNode = NodeManager::instance()->create("skyBNode");
	// - Assign shader to node.
	skyBNode->attachShader(skyshader);
	// - Assign geometry object to node.
	skyBNode->attachGobject(gobj);
	// - Set sky node in RenderState.
	RenderState::instance()->setSkybox(skyBNode);
	/* =================== END YOUR CODE HERE ====================== */
}

// TODO: display the skybox
//
// This function does the following:
//
// - Store previous shader
// - Move Skybox to camera location, so that it always surrounds camera.
// - Disable depth test.
// - Set skybox shader
// - Draw skybox object.
// - Restore depth test
// - Set previous shader
//
// Parameters are:
//
//   - cam: The camera to render from
//
// Useful functions:
//
// - RenderState::instance()->getShader: get current shader.
// - RenderState::instance()->setShader(ShaderProgram * shader): set shader.
// - RenderState::instance()->push(RenderState::modelview): push MODELVIEW
//   matrix.
// - RenderState::instance()->pop(RenderState::modelview): pop MODELVIEW matrix.
// - Node::getShader(): get shader attached to node.
// - Node::getGobject(): get geometry object from node.
// - GObject::draw(): draw geometry object.
// - glDisable(GL_DEPTH_TEST): disable depth testing.
// - glEnable(GL_DEPTH_TEST): disable depth testing.

void DisplaySky(Camera *cam) {

	RenderState *rs = RenderState::instance();

	Node *skynode = rs->getSkybox();
	if (!skynode) return;

	/* =================== PUT YOUR CODE HERE ====================== */
	// - Store previous shader
	ShaderProgram *prevShader = rs->getShader();
	// - Move Skybox to camera location, so that it always surrounds camera.
	Trfm3D *t = new Trfm3D();	// Transformazio berria sortu
	Vector3 camPos = cam->getPosition();
	t->setTrans(camPos);
	rs->addTrfm(RenderState::modelview,t);
	// - Disable depth test.
	glDisable(GL_DEPTH_TEST);	// z-buffer desaktibatu
	// - Set skybox shader
	rs->setShader(skynode->getShader());
	// - Get skybox object.
	GObject *skybox = skynode->getGobject();
	// - Draw skybox object.
	skybox->draw();
	// - Restore depth test
	glEnable(GL_DEPTH_TEST);	// z-buffer aktibatu
	// - Set previous shader
	rs->setShader(prevShader);
	/* =================== END YOUR CODE HERE ====================== */
}
