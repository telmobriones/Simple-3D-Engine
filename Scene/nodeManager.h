// -*-C++-*-

#pragma once

#include <string>
#include <map>
#include "mgriter.h"
#include "node.h"

class NodeManager {

public:
	static NodeManager * instance();

	/**
	 * Register a new node
	 *
	 * If name is new, create new node and register it.
	 * If node already exists, return it
	 *
	 */
	Node *create(const std::string &name);

	/**
	 * Get a registered node
	 *
	 * Return the node or 0 if not found
	 */
	Node *find(const std::string & name) const;

	void print() const;

	// iterate over all nodes
	typedef mgrIter<Node *> iterator;
	iterator begin();
	iterator end();

private:
	NodeManager();
	~NodeManager();
	NodeManager(const NodeManager &);
	NodeManager & operator=(const NodeManager &);

	std::map<std::string, Node *> m_hash;
};
