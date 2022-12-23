#include <cstdio>
#include <string>
#include <assert.h>
#include "tools.h"
#include "material.h"
#include "textureManager.h"

using std::string;

Material::Material(const std::string & name) :
	m_name(name),
	m_diffuse(Vector3(0.8f, 0.8f, 0.8f)),
	m_hasSpecular(false),
	m_specular(Vector3::ZERO),
	m_shininess(65.0f),
	m_alpha(1.0f),
	m_tex(),
	m_bump(0),
	m_specmap(0) {
	m_tex.push_back(TextureManager::instance()->whiteTexture());
}

Material::~Material() {};

// get
float  Material::getAlpha() const { return m_alpha; }
const Vector3 &Material::getDiffuse() const { return m_diffuse; }
const Vector3 &Material::getSpecular() const { return m_specular; }
float  Material::getShininess() const { return m_shininess; }
size_t Material::numTextures() const { return m_tex.size(); };
Texture *Material::getTexture()  { return m_tex[0]; }
Texture *Material::getTexture(size_t i) {
	if (i < m_tex.size()) {
		return m_tex[i];
	}
	fprintf(stderr, "[E] Material %s does not have texture number %lu\n", m_name.c_str(), i);
	exit(1);
}
Texture *Material::getBumpMap() { return m_bump; }
Texture *Material::getSpecularMap() { return m_specmap; }

//Texture *Material::getTextureMaterial() const { return m_tex; }
// Texture *Material::getBumpMap() const { return m_bump; }

// set
void Material::setAlpha(float alpha)  { m_alpha = alpha; }
void Material::setDiffuse(const Vector3 &rgb) { m_diffuse = rgb; }
void Material::setSpecular(const Vector3 &rgb, float shininess) {
	m_hasSpecular = true;
	m_shininess = shininess;
	m_specular = rgb;
}
void Material::setTexture (Texture * tex) {	m_tex[0] = tex; }
void Material::addTexture (Texture * tex) {	m_tex.push_back(tex); }
void Material::setBumpMap (Texture * bump) { m_bump = bump; }
void Material::setSpecularMap (Texture * specmap) { m_specmap = specmap; }

// iterate textures
std::vector<Texture *>::iterator Material::begin() { return m_tex.begin(); }
std::vector<Texture *>::iterator Material::end() { return m_tex.end(); }
std::vector<Texture *>::const_iterator Material::begin() const { return m_tex.begin(); }
std::vector<Texture *>::const_iterator Material::end() const { return m_tex.end(); }

// query
bool Material::isTransp() const {
	return m_alpha != 1.0;
}
bool Material::hasTexture() const { return m_tex.size() != 0; }
bool Material::hasBump() const { return m_bump != 0; }

// print
void Material::print() const {
	printf("Name: %s\n", m_name.c_str() );
	printf(" Diffuse  %0.4f %0.4f %0.4f\n",
		   m_diffuse[0], m_diffuse[1],
		   m_diffuse[2]);
	printf(" Specular %0.4f %0.4f %0.4f\n",
		   m_specular[0], m_specular[1],
		   m_specular[2]);
	printf(" Shininess %5.1f\n", m_shininess );
	printf(" Alpha: %f ", m_alpha);
	if (m_tex.size()) {
		printf("Textures (%lu)\n", m_tex.size());
		for(auto it = m_tex.begin(); it != m_tex.end(); ++it) {
			printf("  ");
			(*it)->print();
		}
	}
	if (m_bump) {
		printf("Bump map: ");
		m_bump->print();
	}
}

bool Material::parse_set_textures(const std::vector<std::string> & texNames, std::string &err) {
	std::vector<Texture *> aux;
	for(auto it = texNames.begin(); it != texNames.end(); ++it) {
		Texture *tex = TextureManager::instance()->find(*it);
		if (!tex) {
			err = "'" + *it +"' not found";
			return false;
		}
		aux.push_back(tex);
	}
	m_tex.swap(aux);
	return true;
}
