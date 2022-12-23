// -*-C++-*-

#pragma once

#include <string>
#include <map>
#include "mgriter.h"
#include "avatar.h"

class AvatarManager {

public:
	static AvatarManager * instance();

	/**
	 * Register a new avatar
	 *
	 * If name is new, create new avatar and register it.
	 * If avatar already exists, return it
	 *
	 */
	Avatar *create(const std::string &name,
				   Camera *theCamera, int radius);

	/**
	 * Get a registered avatar
	 *
	 * Return the avatar or 0 if not found
	 */
	Avatar *find(const std::string & name) const;
	void print() const;

	// iterate over all objects
	typedef mgrIter<Avatar *> iterator;
	iterator begin();
	iterator end();

private:
	AvatarManager();
	~AvatarManager();
	AvatarManager(const AvatarManager &);
	AvatarManager & operator=(const AvatarManager &);

	std::map<std::string, Avatar *> m_hash;

};
