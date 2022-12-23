#include "tools.h"
#include "constants.h"
#include <cmath>
#include <cstdio>
#include <sstream> // for ostringstream

using std::string;

bool distance_is_zero(float d) {
	return fabs(d) < Constants::distance_epsilon;
}

bool is_power_of_2(int n) {
	return (n>0 && ((n & (n-1)) == 0));
}

std::string getFilename(const std::string &dir, const std::string & fname) {
	string res(dir);
	if (dir[dir.size() - 1] != '/') res.append("/");
	return res + fname;
}

void writeV(const std::vector<float> & v) {
	std::vector<float>::const_iterator it = v.begin();
	std::vector<float>::const_iterator end = v.end();
	printf("(");
	if (it != end) {
		end--;
		for(; it != end; ++it)
			printf("%.2f, ", *it);
		printf("%.2f", *end);
	}
	printf(")");
}

std::string float_to_string(float v) {
	std::ostringstream out;
	out << v;
	return out.str();
}
