// -*-C++-*-

#pragma once

#include <string>
#include <map>
#include "mgriter.h"
#include "camera.h"

class CameraManager {

public:
	static CameraManager * instance();

	/**
	 * Register a new camera (orthographic or perspective)
	 *
	 * If name is new, create new camera and register it.
	 * If camera already exists, return it
	 *
	 */
	OrthographicCamera *createOrthographic(const std::string & name);
	PerspectiveCamera *createPerspective(const std::string & name);

	/**
	 * Get a registered camera
	 *
	 * Return the camera or 0 if not found
	 */
	Camera *find(const std::string & name);

	void print() const;

	// iterate over all cameras
	typedef mgrIter<Camera *> iterator;
	iterator begin();
	iterator end();

private:
	CameraManager();
	~CameraManager();
	CameraManager(const CameraManager &);
	CameraManager & operator=(const CameraManager &);

	std::map<std::string, Camera *> m_hash;

};
