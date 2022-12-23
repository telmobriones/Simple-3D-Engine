// -*-C++-*-

#pragma once

#include "texture.h"

// Textures as render target

class TextureRT : public Texture {

public:
	void bindGL() const; // Set this texture as render target
	void unbindGL() const; // Unbind texture and set framebuffer as render target
	// Save textureRT to JPG.
	// Note: binds and unbinds texture.
	int saveJPG(const std::string &fname) const;
	void print() const;
	friend class TextureManager;

private:
	TextureRT(Texture::type_t type, int width, int height);
	~TextureRT();
	TextureRT(const TextureRT &);
	const TextureRT & operator=(const TextureRT &);

	// FrameBuffer objects
	GLuint m_fbo; // for rendering to texture
	GLuint m_rbo; // render buffer (for depth textures)
	mutable int m_oldViewport[4]; // old viewport coordinates
};
