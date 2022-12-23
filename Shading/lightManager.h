// -*-C++-*-

#pragma once

#include <vector>
#include <string>
#include "mgrviter.h"
#include "light.h"

class LightManager {

public:
	static LightManager * instance();

	/**
	 * Register a new light
	 *
	 * If name is new, create new light and register it.
	 * If light already exists, return it
	 *
	 */
	Light *create(const std::string &name, Light::type_t t);

	/**
	 * Get a registered light
	 *
	 * Return the light or 0 if not found
	 */
	Light *find(const std::string & name) const;

	void switchOn(); // switch all lights 'ON'
	void switchOff(); // switch all lights 'ON'

	void print() const;

	// iterate over elements
	typedef mgrViter<Light *> iterator;
	iterator begin();
	iterator end();

private:
	LightManager();
	~LightManager();
	LightManager(const LightManager &);
	LightManager & operator=(const LightManager &);

	std::vector<Light *> m_v;

};
