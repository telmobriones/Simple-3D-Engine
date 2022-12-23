// -*-C++-*-

#pragma once

#include <string>
#include <list>
#include "vector3.h"
#include "trfm3D.h"
#include "bbox.h"
#include "bsphere.h"
#include "gObject.h"
#include "camera.h"
#include "light.h"
#include "shader.h"


class Node {

public:
	/**
	 * Make a deep copy of the subtree starting at this. The created node will
	 * _not_ have any parents.
	 *
	 * @return a new subtree not linked to any node
	 *
	 * \note the WC transformations and BBox-es of the subtree are not
	 * calculated (as we don't know which parent is the subtree linked to). It
	 * will be calculated, as usual, when attaching the subtree to a node, via
	 * attachNode function.
	 */
	Node *clone();

	///////////////////////////////////
	// attach/detach
	void attachGobject(GObject * M); //!< Attach geometry
	GObject *detachGobject();        //!< Remove geometry
	GObject *getGobject();           //!< get geometry. 0 if node has not geometry.
	void attachLight(Light *theLight);
	Light *detachLight();            //!< remove light
	void attachShader(ShaderProgram *theShader); //!< Attach geometry
	ShaderProgram *detachShader(); //!< Remove geometry
	ShaderProgram *getShader(); //!< get shader. 0 if node has no shader
	void setDrawBBox(bool);
	bool getDrawBBox() const;
	void setCheckCollision(bool b);
	bool getCheckCollision() const;
	bool isCulled() const;

	///////////////////////////////////
	// transformations
	void initTrfm();
	void setTrfm(const Trfm3D * M); //!< set local transformation
	void addTrfm(const Trfm3D * M); //!< add 'M' to local trfm
	void translate(const Vector3 & P); //!< add translation
	void rotateX(float angle ); //!< add rotation X
	void rotateY(float angle ); //!< add rotation Y
	void rotateZ(float angle ); //!< add rotation Z
	void scale(float factor ); //!< add uniform scale

	///////////////////////////////////
	// tree operations

	Node *parent(); // get parent. If not parent, return this.
	Node *nextSibling(); // Get next sibling of node. Return first sibling if node is last child.
	Node *firstChild(); // Get first child of node. Return itself if leaf node.
	Node *cycleChild(size_t idx); // Cycle through children of a Node. Return children[ i % lenght(children) ]

	///////////////////////////////////
	// attch/detach operations

	void addChild(Node *theChild); //!> attach a Node as a child
	void detach(); //!> Detach a Node object from its parent. The object is _not_ destroyed, only detached from the Node.

	///////////////////////////////////
	// draw operations

	/*
	 * Draw a (sub)tree starting at this following a depth first strategy.
	 */
	void draw();

	/*
	 * Perform frustum culling (modify m_isCulled in nodes accordingly)
	 */
	void frustumCull(Camera *cam);

	/**
	 * Check wether a BSphere (in woorld coord.) collides with a (sub)-tree
	 *
	 * @param bsp  Bounding Sphere in world coordinates
	 * @return the Node which collided with the BSphere. 0 if not collision.
	 */
	const Node *checkCollision(const BSphere *bsp) const;


	/**
	 * Print placementWC trfm of subtree starting at this.
	 */
	void print_trfm(int sep=0) const;

	friend class NodeManager;

private:
	Node(const std::string & name);
	~Node();
	Node(const Node &);
	Node &operator=(const Node &);

	// auxiliary functions
	Node *cloneParent(Node *theParent);
	void updateWC();
	void updateGS();
	void updateBB ();
	void propagateBBRoot();
	void updateCull(Camera *cam, unsigned int *mask);
	void setCulled(bool culled);

	// member variables
	std::string m_name;
	Node *m_parent; // pointer to parent. root node points to 0
	std::list<Node *> m_children; // pointers to children
	GObject *m_gObject;  // 0 if not geometry
	Light   *m_light; // 0 if not light
	ShaderProgram *m_shader; // 0 if not shader
	Trfm3D *m_placement; // local transformation to parent node
	Trfm3D *m_placementWC; // local transformation to world
	BBox *m_containerWC; // BBox in world coordinates
	bool m_checkCollision; // if false, don't check collision
	bool m_isCulled; // whether the node is culled
	bool m_drawBBox; // whether BBox has to be drawn
};
