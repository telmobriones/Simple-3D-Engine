#include <stdlib.h>
#include <stdio.h>
#include <algorithm>
#include "texture.h"
#include "tools.h"
#include "imageManager.h"

using std::string;

static const char *TT_string(Texture::type_t e);
static const char *PT_string(GLenum e);
static GLenum PT_string_inv(const std::string & str);
bool isMipmapFilter(GLenum filter);

Texture::Texture() :
	m_type(empty),
	m_name("<noname>"),
	m_target(GL_TEXTURE_2D),
	m_id(0),
	m_size(0),
	m_height(0),
	m_width(0),
	m_components(3),
	m_format(GL_RGB),
	m_mipmap(true),
	m_magFilter(GL_LINEAR),
	m_minFilter(GL_LINEAR),
	m_wrapS(GL_REPEAT),
	m_wrapT(GL_REPEAT),
	m_imgs() {
	//Allocates a texture name
	glGenTextures( 1, &m_id );
}

Texture::~Texture() {
	if (m_id) glDeleteTextures(1, &m_id);
}

Texture::Texture(const std::string & name) :
	m_type(empty),
	m_name(name),
	m_target(GL_TEXTURE_2D),
	m_id(0),
	m_size(0),
	m_height(0),
	m_width(0),
	m_components(3),
	m_format(GL_RGB),
	m_mipmap(true),
	m_magFilter(GL_LINEAR),
	m_minFilter(GL_LINEAR),
	m_wrapS(GL_REPEAT),
	m_wrapT(GL_REPEAT),
	m_imgs() {
	glGenTextures( 1, &m_id );
}

Texture::type_t Texture::getType() const { return m_type; }

void Texture::loadImage(const string &FileName) {
	if(m_imgs.size()) {
		//remove image data from openGL
		glDeleteTextures(1, &m_id);
		//Allocates a texture name
		glGenTextures(1, &m_id);
		std::vector<Image *>().swap(m_imgs);
	}
	m_imgs.push_back(ImageManager::instance()->create(FileName));
	Image *img_ptr = m_imgs[0];
	m_size = img_ptr->getSize();
	m_height = img_ptr->getHeight();
	m_width = img_ptr->getWidth();
	m_components = img_ptr->getComponents();
	if (m_components == 1)
		m_format = GL_RED;
	else if (m_components == 3)
		m_format = GL_RGB;
	else if (m_components == 4)
		m_format = GL_RGBA;
}

void Texture::loadCubemapImages(const string &xpos, const string &xneg,
								const string &ypos, const string &yneg,
								const string &zpos, const string &zneg) {
	const string *names[6] = { &xpos, &xneg, &ypos, &yneg, &zpos, &zneg };
	Image *img;
	ImageManager *imgr = ImageManager::instance();
	if(m_imgs.size()) {
		//remove image data from openGL
		glDeleteTextures(1, &m_id);
		//Allocates a texture name
		glGenTextures(1, &m_id);
		std::vector<Image *>().swap(m_imgs);
	}
	for(int i = 0; i < 6; i++) {
		img = imgr->create(*names[i], false);
		m_imgs.push_back(img);
	}
}

void Texture::setImage(const string &FileName) {

	m_type = tex;
	loadImage(FileName);
	// Filters
	if (m_mipmap) {
		// Filters
		m_magFilter = GL_LINEAR;
		m_minFilter = GL_NEAREST_MIPMAP_NEAREST;
	} else {
		m_magFilter = GL_LINEAR;
		m_minFilter = GL_LINEAR;
	}
	// Send texture data to OpenGL (initialize OpenGL texture)
	sendImageGL();
	sendParamsGL();
}

void Texture::setBumpMap(const string &FileName) {

	m_type = bumpmap;
	loadImage(FileName);
	m_mipmap = 0; // no mipmap for bump textures
	m_magFilter = GL_LINEAR;
	m_minFilter = GL_LINEAR;
	sendImageGL();
	sendParamsGL();
}

void Texture::setProj(const string &FileName) {

	m_type = proj;
	loadImage(FileName);
	m_mipmap = 0; // no mipmap for projective textures
	m_magFilter = GL_LINEAR;
	m_minFilter = GL_LINEAR;
	m_wrapS = GL_CLAMP_TO_BORDER;
	m_wrapT = GL_CLAMP_TO_BORDER;
	sendImageGL();
	sendParamsGL();
}

void Texture::setCubeMap(const string &xpos, const string &xneg,
						 const string &ypos, const string &yneg,
						 const string &zpos, const string &zneg) {

	m_type = cubemap;
	loadCubemapImages(xpos, xneg, ypos, yneg, zpos, zneg);
	m_target = GL_TEXTURE_CUBE_MAP;
	/* // Typical cube map settings */
	m_magFilter = GL_LINEAR;
	m_minFilter = GL_LINEAR;
	m_wrapS = GL_CLAMP_TO_EDGE;
	m_wrapT = GL_CLAMP_TO_EDGE;
	m_wrapR = GL_CLAMP_TO_EDGE;
	sendImageGL();
	sendParamsGL();
}

void Texture::setWhiteTexture() {

	if(m_imgs.size()) {
		//remove image data from openGL
		glDeleteTextures(1, &m_id);
		//Allocates a texture name
		glGenTextures( 1, &m_id );
		std::vector<Image *>().swap(m_imgs);
	}
	m_type = white;
	m_size = 3;
	m_height = 1;
	m_width = 1;
	m_format = GL_RGB;
	m_components = 3;
	m_mipmap = 0;
	m_magFilter = GL_NEAREST;
	m_minFilter = GL_NEAREST;
	m_wrapS = GL_REPEAT;
	m_wrapT = GL_REPEAT;
	sendImageGL();
	sendParamsGL();
}

/* Set/get functions */

bool Texture::setMipmap(bool mipmap) {
	if (!mipmap && (isMipmapFilter(m_magFilter) || isMipmapFilter(m_minFilter))) {
		fprintf(stderr, "[E] can not disable mipmap with mipmap filters\n");
		exit(1);
	}
	bool old = m_mipmap;
	m_mipmap = mipmap;
	if (m_mipmap != old) sendImageGL();
	return old;
}
bool Texture::getMipmap() const { return m_mipmap; }

// Wrap

void Texture::setWrapST(GLenum wrapS, GLenum wrapT) {
	m_wrapS = wrapS;
	m_wrapT = wrapT;
	sendParamsGL();
}

void Texture::setWrapS(GLenum wrapS) {
	m_wrapS = wrapS;
	sendParamsGL();
}

void Texture::setWrapT(GLenum wrapT) {
	m_wrapT = wrapT;
	sendParamsGL();
}


bool Texture::setWrapST(const std::string & wrapS,
						const std::string & wrapT) {
	GLenum wS;
	wS = PT_string_inv(wrapS);
	if (wS != GL_CLAMP && wS != GL_CLAMP_TO_BORDER && wS != GL_REPEAT)
		return false;
	GLenum wT = PT_string_inv(wrapT);
	if (wT != GL_CLAMP && wT != GL_CLAMP_TO_BORDER && wT != GL_REPEAT)
		return false;
	setWrapST(wS, wT);
	return true;
}

// Filters

bool isMipmapFilter(GLenum filter) {
	return
		filter == GL_NEAREST_MIPMAP_LINEAR ||
		filter == GL_NEAREST_MIPMAP_NEAREST ||
		filter == GL_LINEAR_MIPMAP_LINEAR ||
		filter == GL_LINEAR_MIPMAP_NEAREST;
}

void Texture::setMinFilter(GLenum minFilter) {
	bool isMipmap = isMipmapFilter(minFilter);
	m_minFilter = minFilter;
	if (isMipmap && !m_mipmap) {
		m_mipmap = true;
		sendImageGL();
	}
	sendParamsGL();
}

void Texture::setMagFilter(GLenum magFilter) {
	bool isMipmap = isMipmapFilter(magFilter);
	m_magFilter = magFilter;
	if (isMipmap && !m_mipmap) {
		m_mipmap = true;
		sendImageGL();
	}
	sendParamsGL();
}

void Texture::setFilters(GLenum minFilter, GLenum magFilter) {
	setMinFilter(minFilter);
	setMagFilter(magFilter);
}

bool Texture::setFilters(const std::string & minFilter,
						 const std::string & magFilter) {
	GLenum wS;
	GLenum minf = PT_string_inv(minFilter);
	GLenum magf = PT_string_inv(magFilter);
	if (minf != GL_LINEAR && minf != GL_NEAREST && minf != GL_NEAREST_MIPMAP_LINEAR &&
		minf != GL_NEAREST_MIPMAP_NEAREST && minf != GL_LINEAR_MIPMAP_LINEAR &&
		minf != GL_LINEAR_MIPMAP_NEAREST)
		return false;
	if (magf != GL_LINEAR && magf != GL_NEAREST && magf != GL_NEAREST_MIPMAP_LINEAR &&
		magf != GL_NEAREST_MIPMAP_NEAREST && magf != GL_LINEAR_MIPMAP_LINEAR &&
		magf != GL_LINEAR_MIPMAP_NEAREST)
		return false;
	setFilters(minf, magf);
	return true;
}


// BindGLTexture
//
// Set texture as "active" in the zero texture unit. Subsequent texture
// operations are done on this texture.

void Texture::bindGL() const {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( m_target, m_id );
}

void Texture::unbindGL() const {
	glActiveTexture(GL_TEXTURE0);
	glBindTexture( m_target, 0 );
}


void Texture::bindGLUnit(int location) const {
	// Set texture unit index
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture( m_target, m_id );
}

void Texture::unbindGLUnit(int location) const {
	// Set texture unit index
	glActiveTexture(GL_TEXTURE0 + location);
	glBindTexture( m_target, 0 );
}

static int cycleTexEnum(GLenum *fil, int m,
						GLenum theFil) {
	int fil_i = 0;
	for(fil_i = 0; fil_i != m; ++fil_i) {
		if(fil[fil_i] == theFil)
			break;
	}
	fil_i++;
	return (fil_i % m);
}

void Texture::cycleWrapS() {

	static GLenum wrap_fil[] = { GL_CLAMP, GL_REPEAT };
	int idx = cycleTexEnum(wrap_fil, 2, m_wrapS);
	setWrapS(wrap_fil[idx]);
}

void Texture::cycleWrapT() {

	static GLenum wrap_fil[] = { GL_CLAMP, GL_REPEAT };
	int idx = cycleTexEnum(wrap_fil, 2, m_wrapT);
	setWrapT(wrap_fil[idx]);
}

void Texture::cycleMagFilter() {

	static GLenum mag_fil[] = { GL_NEAREST, GL_LINEAR };
	int idx = cycleTexEnum(mag_fil, 2, m_magFilter);
	setMagFilter(mag_fil[idx]);
}

void Texture::cycleMinFilter() {
	static GLenum min_fil[] = { GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST,
								GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR,
								GL_LINEAR_MIPMAP_LINEAR };
	int idx = cycleTexEnum(min_fil, 6, m_minFilter);
	setMinFilter(min_fil[idx]);
	//printf("%s\n", min_fil_str[idx]);
}

int Texture::saveJPG(const std::string &fname) const {
	if (!m_imgs.size()) return 0;
	return m_imgs[0]->saveJPG(fname);
}

/**
 * Send texture parameters to OpenGL
 *
 * hints:
 *        - use glTexParameteri for setting filters/wrap
 *        - use glTexImage2D or gluBuild2DMipmaps for loading image into graphic card
 *          -- internalFormat: 3
 *          -- format:         GL_RGB
 *          -- border:         0
 *          -- type:           GL_UNSIGNED_BYTE
 *
 */

void Texture::sendParamsGL() {

	bindGL();
	// Set OpenGL texture wrap
	glTexParameteri( m_target, GL_TEXTURE_WRAP_S, m_wrapS);
	glTexParameteri( m_target, GL_TEXTURE_WRAP_T, m_wrapT);
	if (m_type == cubemap) {
		glTexParameterf(m_target, GL_TEXTURE_WRAP_R,
						m_wrapR);
	}
	// Set OpenGL texture filters
	glTexParameteri( m_target, GL_TEXTURE_MAG_FILTER, m_magFilter);
	glTexParameteri( m_target, GL_TEXTURE_MIN_FILTER, m_minFilter);
	unbindGL();
}

/**
 * Send image to OpenGL
 *
 * hints:
 *        - use glTexImage2D or gluBuild2DMipmaps for loading image into graphic card
 *          -- internalFormat: 3
 *          -- format:         GL_RGB
 *          -- border:         0
 *          -- type:           GL_UNSIGNED_BYTE
 *
 */

void Texture::sendImageGL() {

	Image *img_ptr;
	static unsigned char whiteRGB[3] = {255, 255, 255};

	bindGL();
	if (m_type == white) {
		glTexImage2D( m_target, 0, m_components, m_width,
					  m_height, 0, m_format, GL_UNSIGNED_BYTE,
					  &whiteRGB[0]);
		return;
	}
	if (!m_imgs.size()) return;
	if (m_type == cubemap) {
		GLuint targets[6] = {
			GL_TEXTURE_CUBE_MAP_POSITIVE_X,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
			GL_TEXTURE_CUBE_MAP_NEGATIVE_Z
		};
		for(int i = 0; i < m_imgs.size(); i++) {
			img_ptr = m_imgs[i];
			glTexImage2D(targets[i], 0, 3,
						 img_ptr->getWidth(), img_ptr->getHeight(),
						 0, GL_RGB, GL_UNSIGNED_BYTE, img_ptr->getData());
		}
		return;
	}
	img_ptr = m_imgs[0];
	if (img_ptr->getHeight() && img_ptr->getWidth()) {
		// Load image to OpenGL texture
		if (m_mipmap) {
			gluBuild2DMipmaps(m_target, m_components, img_ptr->getWidth(), img_ptr->getHeight(),
							  m_format, GL_UNSIGNED_BYTE, img_ptr->getData());
		} else {
			glTexImage2D( m_target, 0, m_components, img_ptr->getWidth(),
						  img_ptr->getHeight(), 0, m_format, GL_UNSIGNED_BYTE,
						  img_ptr->getData());
		}
	}
	unbindGL();
}

static const char *TT_string(Texture::type_t e) {

	static Texture::type_t N[] = {Texture::empty, Texture::tex, Texture::rt_depth, Texture::rt_color, Texture::cubemap, Texture::bumpmap, Texture::proj};
	static const char *T[]  = {"empty", "tex", "rt_depth", "rt_color", "cubemap", "bumpmap", "proj"};

	int i;
	int m;

	m = sizeof(T) / sizeof(char *);
	for(i=0; i < m; i++) {
		if (e == N[i]) return T[i];
	}
	return "<unknown>";
}


static const char *PT_string(GLenum e) {

	static GLenum N[] = {GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_CLAMP, GL_CLAMP_TO_BORDER, GL_REPEAT, GL_LINEAR, GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_RGB, GL_RGB8, GL_DEPTH_COMPONENT};
	static const char * T[]  = {"GL_TEXTURE_2D", "GL_TEXTURE_CUBE_MAP", "GL_CLAMP", "GL_CLAMP_TO_BORDER", "GL_REPEAT", "GL_LINEAR", "GL_NEAREST", "GL_NEAREST_MIPMAP_LINEAR", "GL_NEAREST_MIPMAP_NEAREST", "GL_LINEAR_MIPMAP_LINEAR", "GL_LINEAR_MIPMAP_NEAREST", "GL_RGB", "GL_RGB8", "GL_DEPTH_COMPONENT"};
	int i;
	int m;

	m = sizeof(N) / sizeof(GLenum);
	for(i=0; i < m; i++) {
		if (e == N[i]) return T[i];
	}
	return "<unknown>";
}

static GLenum PT_string_inv(const std::string & str) {

	static GLenum N[] = {GL_TEXTURE_2D, GL_TEXTURE_CUBE_MAP, GL_CLAMP, GL_CLAMP_TO_BORDER, GL_REPEAT, GL_LINEAR, GL_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_NEAREST, GL_RGB, GL_RGB8, GL_DEPTH_COMPONENT};
	static const char * T[]  = {"GL_TEXTURE_2D", "GL_TEXTURE_CUBE_MAP", "GL_CLAMP", "GL_CLAMP_TO_BORDER", "GL_REPEAT", "GL_LINEAR", "GL_NEAREST", "GL_NEAREST_MIPMAP_LINEAR", "GL_NEAREST_MIPMAP_NEAREST", "GL_LINEAR_MIPMAP_LINEAR", "GL_LINEAR_MIPMAP_NEAREST", "GL_RGB", "GL_RGB8", "GL_DEPTH_COMPONENT"};
	int i;
	int m;

	m = sizeof(N) / sizeof(GLenum);
	for(i=0; i < m; i++) {
		if (str == string(T[i])) return N[i];
	}
	return 0;
}

const std::string &Texture::getName() const {
	return m_name;
}

void Texture::print() const {

	printf("Texture id:%d\tName: %s\tType: %s\ttarget:%s\n", m_id, m_name.c_str(), TT_string(m_type), PT_string(m_target));
	printf("Images (%lu):\n", m_imgs.size());
	for(int i = 0; i < m_imgs.size(); ++i){
		printf("  Image: %s\tResolution: %d x %d\tSize: %lu\tMipmap: %d\tData: %p\n", m_imgs[i]->getName(), m_imgs[i]->getWidth(), m_imgs[i]->getHeight(),
			   m_size, m_mipmap, m_imgs[i]->getData());

	}
	if (!m_imgs.size() && m_size) {
		printf("Generated image\tResolution: %d x %d \t Size: %lu\tMipmap: %d\n", m_width, m_height,
			   m_size, m_mipmap);
	}
	printf("Format: %s\tComponents:%d\n", PT_string(m_format), m_components);
	printf("Filters (min,mag): %s, %s\t Wrap (s,t): %s,%s\n\n",
			 PT_string(m_minFilter), PT_string(m_magFilter),
			 PT_string(m_wrapS), PT_string(m_wrapT));
}
