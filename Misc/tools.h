// -*-C++-*-

#pragma once

#include <string>
#include <vector>

#define ASSERT_OPENGL { GLenum errorCode = glGetError();				\
		if (errorCode != GL_NO_ERROR) {									\
			fprintf (stderr, "[E] %s, line %d. Error: %s\n", __FUNCTION__, __LINE__, gluErrorString(errorCode)); \
			exit(1);}}

bool distance_is_zero(float d);

bool is_power_of_2(int n);

std::string getFilename(const std::string &dir, const std::string & fname);

// print a vector
void writeV(const std::vector<float> & v);

// float 2 string
std::string float_to_string(float v);
