// -*-C++-*-

#pragma once

#include <string>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "image.h"

class Texture {

public:

	enum type_t {
		empty,    // empty texture
		white,    // white 1x1 texture
		tex,      // texture (from JPG)
		rt_depth, // Depth texture (render texture)
		rt_color, // Color texture (render testure)
		cubemap,  // Cubemap texture (from JPG)
		bumpmap,  // Bump map (normal) texture (from JPG)
		proj	  // Underlying texture of projection textures
	};

	type_t getType() const;

	// Texture map types
	void setImage(const std::string &img_fname); // image texture (jpg)
	void setBumpMap(const std::string &img_fname);  // bump texture (jpg)
	void setProj(const std::string &img_fname);  // projective texture (jpg)
	void setCubeMap(const std::string &xpos, const std::string &xneg,
					const std::string &ypos, const std::string &yneg,
					const std::string &zpos, const std::string &zneg); // cubemap textures (jpg)

	void setWhiteTexture(); // 1x1 pixel, white color. no filter, no mipmap.

	/////////////////////////////////////////////////
	// Functions for changing the texture parameters

	bool setMipmap(bool mipmap);
	bool getMipmap() const;

	// Wrap

	bool setWrapST(const std::string & wrapS, const std::string & wrapT);
	void setWrapST(GLenum wrapS, GLenum wrapT);
	void setWrapS(GLenum wrapS);
	void setWrapT(GLenum wrapT);

	// Filters

	bool setFilters(const std::string & minFilter,
					const std::string & magFilter);
	void setMinFilter(GLenum minFilter);
	void setMagFilter(GLenum magFilter);
	void setFilters(GLenum minFilter, GLenum magFilter);

	//
	// bind/unbind openGL
	//
	// bind texture to unit 0
	virtual void bindGL() const;
	// ubind unit 0
	virtual void unbindGL() const;
	// bind texture to given unit
	void bindGLUnit(int location) const;
	// bind texture from given unit
	void unbindGLUnit(int location) const;

	// Save a texture to a JPG file
	// TODO
	virtual int saveJPG(const std::string &filename) const;

	// Cycle between wrap values

	void cycleWrapS();
	void cycleWrapT();

	// Cycle between filters

	void cycleMagFilter();
	void cycleMinFilter();

	const std::string &getName() const;
	void print() const;

	friend class TextureManager;

protected:
	Texture();
	virtual ~Texture();

	type_t m_type;       // texture type
	std::string m_name;  // texture name
	GLenum m_target;     // GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP. Default is GL_TEXTURE_2D.
	GLuint m_id;         // OpenGL id for the texture
	size_t m_size;       // Image size in bytes
	int m_height;        // Texture height (must be power of 2)
	int m_width;         // Texture width (must be power of 2)
	int m_components;    // number of components of each pixel (3 or 1)
	GLuint m_format;     // format of pixels (GL_RGB or DEPTH_COMPONENT)

protected:
	// Create, Destroy & Copy functions
	Texture(const std::string &);
	Texture(const Texture &);
	Texture & operator =(const Texture &);

	void loadImage(const std::string &fname);
	void loadCubemapImages(const std::string &xpos, const std::string &xneg,
						   const std::string &ypos, const std::string &yneg,
						   const std::string &zpos, const std::string &zneg); // cubemap textures (jpg)

	void sendImageGL();
	void sendParamsGL();
	// Get image from OpenGL and store it in memory
	void SyncOpenGLTexture();

	bool m_mipmap;     // 0 - no mipmap, 1 - mipmap. Default 0

	GLenum m_magFilter; // GL_NEAREST, GL_LINEAR. Default GL_NEAREST.
	GLenum m_minFilter; // GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST,
	// GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR
	// and GL_LINEAR_MIPMAP_LINEAR.
	// Default GL_NEAREST (GL_NEAREST_MIPMAP_NEAREST if mipmap)

	GLenum m_wrapS;     // GL_CLAMP, GL_REPEAT. Default GL_REPEAT.
	GLenum m_wrapT;     // GL_CLAMP, GL_REPEAT. Default GL_REPEAT.
	GLenum m_wrapR;     // GL_CLAMP, GL_REPEAT.
	std::vector<Image *> m_imgs;  // jpg images
};
