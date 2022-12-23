// -*-C++-*-

#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>
#include <string>
#include "shaderUtils.h"
#include "vector3.h"
#pragma once

#include <string>
#include "trfm3D.h"

struct Uniform {

	enum type {
		uerror = 0,
		uint = 1,
		ufloat = 2,
		uvec3 = 3,
		uvec4 = 4,
		uivec3 = 5,
		umat4 = 6,
		usampler = 7
	};

	static const char *allowed_types[];
	static const char *type_str(type t);

	static Uniform from_json(std::string & src);

	Uniform();
	Uniform(const char *name, Uniform::type t);
	Uniform(const char *name, const char * t);
	Uniform(const std::string & name, const std::string & t);

	bool send(int value) const;
	bool send(float value) const;
	bool send(const Vector3 &value) const;
	bool send(const Vector3 *value) const;
	bool send(const float *value) const;
	bool send(const int *value) const;
	bool send(const Trfm3D &value) const;
	bool send(const Trfm3D *value) const;

	std::string m_name;
	type m_type;
	GLint m_handler;
	static std::string last_error_str;
};
