#include "uniform.h"
#include "json.h"
#include <cstring>

std::string Uniform::last_error_str;
const char *Uniform::allowed_types[] = {"uerror", "uint", "ufloat", "uvec3", "uvec4", "uivec3", "umat4", "usampler"};

const char *Uniform::type_str(Uniform::type t) {

	return allowed_types[t];
}

Uniform::Uniform() : m_name(), m_type(Uniform::uerror), m_handler(-1) {}
Uniform::Uniform(const char *name, Uniform::type t) : m_name(name), m_type(t), m_handler(-1) {}

Uniform::Uniform(const char *name, const char *t) : m_name(name), m_type(Uniform::uerror) {
	for(int i = 0; i < 8; ++i) {
		if (!strcmp(t, allowed_types[i])) {
			m_type = (Uniform::type) i;
			break;
		}
	}
}

Uniform::Uniform(const std::string & name, const std::string & t) : m_name(name), m_type(Uniform::uerror) {
	for(int i = 0; i < 8; ++i) {
		if (!strcmp(t.c_str(), allowed_types[i])) {
			m_type = (Uniform::type) i;
			break;
		}
	}
}


bool Uniform::send(int value) const {
	if (m_type != Uniform::type::uint && m_type != Uniform::type::usampler) {
		Uniform::last_error_str = "uniform is (" + std::string(type_str(m_type)) + ") but expected to be (uint | usampler)";
		return false;
	}
	shader_set_uniform_1i(m_handler, value);
	return true;
}

bool Uniform::send(float value) const {
	if (m_type != Uniform::type::ufloat) {
		Uniform::last_error_str = "uniform is (" + std::string(type_str(m_type)) + ") but expected to be (ufloat)";
		return false;
	}
	shader_set_uniform_1f(m_handler, value);
	return true;
}

bool Uniform::send(const Vector3 &value) const {
	if (m_type != Uniform::type::uvec3) {
		Uniform::last_error_str = "uniform is (" + std::string(type_str(m_type)) + ") but expected to be (uvec3)";
		return false;
	}
	shader_set_uniform(m_handler, value);
	return true;
}

bool Uniform::send(const Vector3 *value) const {
	return send(*value);
}

bool Uniform::send(const float *value) const {
	if (m_type != Uniform::type::uvec4) {
		Uniform::last_error_str = "uniform is (" + std::string(type_str(m_type)) + ") but expected to be (uvec4)";
		return false;
	}
	shader_set_uniform_4fv(m_handler, &value[0]);
	return true;
}

bool Uniform::send(const int *value) const {
	if (m_type != Uniform::type::uivec3) {
		Uniform::last_error_str = "uniform is (" + std::string(type_str(m_type)) + ") but expected to be (uivec3)";
		return false;
	}
	shader_set_uniform_3i(m_handler, value[0], value[1], value[2]);
	return true;
}

bool Uniform::send(const Trfm3D &value) const {
	float m[16];
	if (m_type != Uniform::type::umat4) {
		Uniform::last_error_str = "uniform is (" + std::string(type_str(m_type)) + ") but expected to be (umat4)";
		return false;
	}
	value.getGLMatrix(m);
	shader_set_uniform_matrix4(m_handler, m);
	return true;
}

bool Uniform::send(const Trfm3D *value) const {
	return send(*value);
}
