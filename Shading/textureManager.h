// -*-C++-*-

#pragma once

#include <string>
#include <map>
#include "mgriter.h"
#include "texture.h"

class TextureManager {

public:
	static TextureManager * instance();

	/**
	 * Register a new texture
	 *
	 * If texture name is new, create new texture and register it.
	 * If texture already exists, return it
	 *
	 * @param fName: The file name of the texture
	 * @return the texture
	 */
	Texture *create(const std::string & texName, const std::string & fName = std::string());

	// create bump texture
	Texture *createBumpMap(const std::string & texName, const std::string & fName = std::string());

	// create projective texture
	Texture *createProj(const std::string & texName, const std::string & fName = std::string());

	// create cubemap texture
	// needs 6 image files
	Texture *createCubeMap(const std::string & texName,
						   const std::string & xpos, const std::string & xneg,
						   const std::string & ypos, const std::string & yneg,
						   const std::string & zpos, const std::string & zneg);

	//
	// create RT (render target) texture of depth map
	//
	Texture *createRTDepth(const std::string & texName, int height, int width);

	//
	// create RT (render target) texture of color map
	//
	Texture *createRTColor(const std::string & texName, int height, int width);

	/**
	 * Get a registered texture
	 *
	 * @param fName: The file name of the texture
	 * @return the texture or 0 if not found
	 */

	Texture *find(const std::string & fName) const;

	/**
	 * Get the default texture
	 *
	 * @return the default texture
	 */

	Texture *whiteTexture() const;

	// void SceneSetTextureFunction(GLint texFunc);

	void print() const;

	// iterate over all textures
	typedef mgrIter<Texture *> iterator;
	iterator begin();
	iterator end();

private:
	TextureManager();
	~TextureManager();
	TextureManager(const TextureManager &);
	TextureManager & operator=(const TextureManager &);

	Texture *m_white;
	std::map<std::string, Texture *> m_hash;

};
