#include "nodeManager.h"

using std::string;
using std::map;

//////////////////////////////////////////////////7
// node manager

NodeManager * NodeManager::instance() {
	static NodeManager mgr;
	return &mgr;
}

NodeManager::NodeManager() {}

NodeManager::~NodeManager() {
	for(auto it = m_hash.begin(), end = m_hash.end();
		it != end; ++it)
		delete it->second;
}

Node *NodeManager::create(const std::string &key) {
	auto it = m_hash.find(key);
	if (it != m_hash.end()) {
		fprintf(stderr, "[W] duplicate node %s\n", key.c_str());
		return it->second;
	}
	Node * newnode = new Node(key);
	it = m_hash.insert(make_pair(key, newnode)).first;
	return it->second;
}


Node *NodeManager::find(const std::string & key) const {
	auto it = m_hash.find(key);
	if (it == m_hash.end()) return 0;
	return it->second;
}

// void NodeManager::print() const {
//	for(auto it = m_hash.begin(), end = m_hash.end();
//		it != end; ++it)
//		it->second->print();
// }

NodeManager::iterator NodeManager::begin() { return NodeManager::iterator(m_hash.begin()); }
NodeManager::iterator NodeManager::end() { return NodeManager::iterator(m_hash.end()); }
