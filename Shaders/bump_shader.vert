#version 120

// Bump mapping with many lights.

// all attributes in model space
attribute vec3 v_position;
attribute vec3 v_normal;
attribute vec2 v_texCoord;
attribute vec3 v_TBN_t;
attribute vec3 v_TBN_b;

uniform mat4 modelToCameraMatrix;
uniform mat4 modelToWorldMatrix;
uniform mat4 cameraToClipMatrix;
uniform mat4 modelToClipMatrix;

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)

uniform struct light_t {
	vec4 position;    // Camera space
	vec3 diffuse;     // rgb
	vec3 specular;    // rgb
	vec3 attenuation; // (constant, lineal, quadratic)
	vec3 spotDir;     // Camera space
	float cosCutOff;  // cutOff cosine
	float exponent;
} theLights[4];     // MG_MAX_LIGHTS

// All bump computations are performed in tangent space; therefore, we need to
// convert all light (and spot) directions and view directions to tangent space
// and pass them the fragment shader.

varying vec2 f_texCoord;
varying vec3 f_viewDirection;     // tangent space
varying vec3 f_lightDirection[4]; // tangent space
varying vec3 f_spotDirection[4];  // tangent space

void main( ) {
	// get 3x3 modelview matrix
	mat3 MV3x3 = mat3( modelToCameraMatrix );

	// Normal, tangent and bitangent in camera coordinates (they MUST be normalized!)
	// (object space -> camera space)
	vec3 t = normalize( MV3x3 * v_TBN_t );
	vec3 b = normalize( MV3x3 * v_TBN_b );
	vec3 n = normalize( MV3x3 * v_normal );

	// matrix to transform from camera space to tangent space
	// KONTUZ! OpenGL-n matrizeak "For multiple values, matrices are filled in in column-major order."
	mat3 cameraToTangent;
	cameraToTangent[0] = vec3( t.x, b.x, n.x );
	cameraToTangent[1] = vec3( t.y, b.y, n.y );
	cameraToTangent[2] = vec3( t.z, b.z, n.z );

	// Ligth direction is in camera space
	// (camera space -> tangent space)
	// NOTE: do not normalize the vector
	// Do the same with f_viewDirection, f_spotDirection

	vec4 posInCamCoords = modelToCameraMatrix * vec4( v_position, 1.0 );
	f_viewDirection = cameraToTangent * ( ( -1 ) * posInCamCoords.xyz );

	// Argi guztiak korritzeko
	for( int i = 0; i < active_lights_n; i++ ) {
		if( theLights[i].position[3] == 0 ) {	// Argi infinituak (Directional)
			f_lightDirection[i] = cameraToTangent * ( -1 * theLights[i].position.xyz );
		} else { // Spotlight edo Argi locala (local)
			f_lightDirection[i] = cameraToTangent * ( theLights[i].position.xyz - posInCamCoords.xyz );
			if( theLights[i].cosCutOff > 0 ) {	// Spotlight
				f_spotDirection[i] = cameraToTangent * theLights[i].spotDir;
			}
		}
	}

	f_texCoord = v_texCoord;
	gl_Position = modelToClipMatrix * vec4( v_position, 1.0 );
}
