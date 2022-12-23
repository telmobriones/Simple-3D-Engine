// -*-C++-*-

#pragma once

#include <string>
#include "vector3.h"

class Light {

public:

	enum type_t {
		invalid,
		directional,
		positional,
		spotlight
	};

	Light(const std::string & name, type_t t = positional);
	~Light();
	void swap(Light & rhs);

	// Light name
	const char *getName() const;
	const std::string &getNameString() const;

	type_t getType() const;
	/**
	 * Turn the light on/off according to OpenGL
	 * @param status true: on / false: off
	 */
	void switchLight(bool status );
	bool isOn() const; // whether light is on

	/////////////////////////////
	// position / direction

	/**
	 * Set light position
	 *
	 * If light is directional, pos is a direction (vector)
	 */
	void setPosition(const Vector3 & pos);

	/**
	 * Get light position in Eye coordinates.
	 */
	Vector3 getPositionEye() const;
	/**
	 * Get light position in World coordinates.
	 */
	Vector3 getPositionWorld() const;

	/**
	 * Get light position in Eye coordinates. Returns a pointer to a 4-float
	 * vector, which represents a point in homogeneous coordinates. Last
	 * coordinate (w) is 0.0 for directional lights, 1.0 for positional and
	 * spotlights.
	 *
	 */
	const float *getPositionEye_4fv() const;

	/**
	 * Put light into scene (multiply position and possibly spot direction with modelview)
	 *
	 */
	void placeScene();

	////////////////////////////////////////////////
	// spotlight

	/**
	 *
	 * Set spot light direction
	 *
	 * @param direction point to a 3-valued vector representing spot direction
	 *
	 */
	void setSpotData(const Vector3 & direction,
					 float cutOff, float exponent);
	/**
	 * Get spotlight direction in Eye coordinates
	 *
	 * @return a pointer to a 3-vector with position in Eye coordinates or the vector (0,0,0) if light is not spotlight.
	 */
	const Vector3 & getSpotDirectionEye() const;
	/**
	 * Get spotlight direction in World coordinates
	 *
	 * @return a pointer to a 3-vector with position in World coordinates or the vector (0,0,0) if light is not spotlight.
	 */
	const Vector3 & getSpotDirectionWorld() const;

	float getSpotExponent() const;
	float getSpotCutoff() const;
	bool isSpot() const;

	////////////////////////////////////////////////
	// light channels

	void setDiffuse(const Vector3 &rgb);
	const Vector3 &getDiffuse() const;
	void setSpecular(const Vector3 &rgb);
	const Vector3 &getSpecular() const;

	/////////////////////////////
	// light attenuation

	/**
	 * Get a vector of 3 elements with attenuations (constant, lineal, quadratic)
	 */
	const Vector3 &getAttenuationVector() const;
	void setConstantAttenuation(float c);
	float getConstantAttenuation() const;
	void setLinearAttenuation(float b);
	float getLinearAttenuation() const;
	void setQuadraticAttenuation(float a);
	float getQuadraticAttenuation() const;

	void twoSides(int twoside);

	void  print();


private:

	Light(const Light &);
	Light & operator=(const Light &);

	// The default values that are listed here
	// are those defined by OpenGL
	// Our Default Light has different values

	std::string m_name;

	type_t   m_type;                  // default is directional
	bool     m_twoside;               // 0 only front faces  are shaded (those which look towards the light)
									  // 1 both front & back faces are shaded. Default false
	bool     m_switched;              // true => ON (default)
	Vector3  m_diffuse;               // default (0.8, 0.8, 0.8)
	Vector3  m_specular;              // default (0.0, 0.0, 0.0)
	Vector3  m_position;              // default (0.0 ,5.0, 0.0);
									  // Note that, if m_type is directional, the position is in fact a direction.
	Vector3  m_spotDirection;         // GL_SPOT_DIRECTION : default direction is (sqrt(3)/3, sqrt(3)/3, sqrt(3)/3)
									  //  significant only when GL_SPOT_CUTOFF is not 180
	Vector3  m_positionEye;           // Light position in eye coordinates
	Vector3  m_spotDirectionEye;      // GL_SPOT_DIRECTION : default direction is (sqrt(3)/3, sqrt(3)/3, sqrt(3)/3)
	float    m_spotExponent;          // GL_SPOT_EXPONENT [0,128], default 10
									  //  0 => uniform light distribution
									  //  higher spot => more focused light source,
	float    m_spotCutOff;            // GL_SPOT_CUTOFF [0,90] & 180, default 30
									  //  180 => uniform light distribution
	Vector3  m_att;                   // [GL_CONSTANT_ATTENUATION, GL_LINEAR_ATTENUATION, GL_QUADRATIC_ATTENUATION]
	int      m_needsUpdate;           // internal use.

};
