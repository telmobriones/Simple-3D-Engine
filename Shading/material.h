// -*-C++-*-

#pragma once

#include <string>
#include <vector>
#include "vector3.h"
#include "texture.h"

class Material {

public:

	// get
	float getAlpha() const;
	const Vector3 &getDiffuse() const;
	const Vector3 &getSpecular() const;
	float getShininess() const;
	// set
	void setAlpha(float alpha);
	void setDiffuse(const Vector3 &rgb);
	void setSpecular(const Vector3 &rgb, float shininess);
	void setTexture (Texture * theTexture);
	void addTexture (Texture * theTexture);
	void setBumpMap (Texture * bumpMapexture);
	void setSpecularMap (Texture * bumpMapexture);
	size_t numTextures() const;
	Texture *getTexture();
	Texture *getTexture (size_t i);
	Texture *getBumpMap();
	Texture *getSpecularMap();

	// iterate textures
	std::vector<Texture *>::iterator begin();
	std::vector<Texture *>::iterator end();
	std::vector<Texture *>::const_iterator begin() const;
	std::vector<Texture *>::const_iterator end() const;
	// query
	bool isTransp() const;
	bool hasTexture() const;
	bool hasBump() const;
	// print
	void print() const;

	bool parse_set_textures(const std::vector<std::string> & texNames,
							std::string &err);
	friend class MaterialManager;
private:
	Material(const std::string & name);
	~Material();
	Material();
	Material(const Material &);
	Material & operator =(const Material &);

	// The default values that we use
	// are those defined by OpenGL
	std::string m_library;
	std::string m_name;

	Vector3  m_diffuse;          // GL_DIFFUSE  : RGB default (0.8, 0.8, 0.8)
	int      m_hasSpecular;
	Vector3  m_specular;         // GL_SPECULAR : RGB default (0.0, 0.0, 0.0)
	float    m_shininess;        // GL_SHININESS: default 0.0 ; range [0,128]
	float    m_alpha;            // alpha value. Default 1
	std::vector<Texture *> m_tex;  // Textures
	Texture *m_bump;               // Bump-mapping texture
	Texture *m_specmap;            // Specular map
};
