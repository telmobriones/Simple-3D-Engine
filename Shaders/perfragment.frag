#version 120

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)
uniform vec3 scene_ambient; // Scene ambient light

vec3 i_diffuse = vec3( 0 ); // diffuse
vec3 i_specular = vec3( 0 ); // specular
vec3 i_tot_light = vec3( 0 ); // diffuse + specular

uniform struct light_t {
	vec4 position;    // Camera space
	vec3 diffuse;     // rgb
	vec3 specular;    // rgb
	vec3 attenuation; // (constant, lineal, quadratic)
	vec3 spotDir;     // Camera space
	float cosCutOff;  // cutOff cosine
	float exponent;
} theLights[4];     // MG_MAX_LIGHTS

uniform struct material_t {
	vec3 diffuse;
	vec3 specular;
	float alpha;
	float shininess;
} theMaterial;

uniform sampler2D texture0;

varying vec3 f_position;      // camera space
varying vec3 f_viewDirection; // camera space
varying vec3 f_normal;        // camera space
varying vec2 f_texCoord;

vec3 halfway( vec3 l, vec3 v ) {
	return normalize( l + v );
}

float phong_blinn_coef( vec3 l, vec3 v, vec3 n ) {
	vec3 h = halfway( l, v );
	float d = dot( h, n );
	return pow( max( 0, d ), 4 * theMaterial.shininess );
}

float spotlight_coef( vec3 l, vec3 dir, float exp_ ) {
	return pow( max( dot( ( -1 ) * l, dir ), 0 ), exp_ );
}

float local_atenuation( vec3 att, float dist ) {
	return 1 / ( att.x + att.y * dist + att.z * pow( dist, 2 ) );
}

void main( ) {
	// Interpolazio bidez lortutako norabideak normalizatu
	vec3 V = normalize( f_viewDirection );
	vec3 N = normalize( f_normal );

	vec3 L;

	// Argi guztiak korritzeko
	for( int i = 0; i < active_lights_n; i++ ) {
		if( theLights[i].position[3] == 0 ) {	// Argi infinituak (Directional) bektoreak direnez light_t.position[3]==0 izango da (koord. homog.)

			L = normalize( -1 * theLights[i].position.xyz );
			i_diffuse = theMaterial.diffuse * theLights[i].diffuse; // i_diff = m_diff * s_diff
			i_specular = phong_blinn_coef( L, V, N ) * theMaterial.specular * theLights[i].specular;
			i_tot_light += dot( N, L ) * ( i_diffuse + i_specular );

		} else { // Spotlight edo Argi locala (local)

			float dist = length( theLights[i].position.xyz - f_position );	// Argiaren eta eraso-puntuaren arteko distantzia
			L = ( theLights[i].position.xyz - f_position.xyz ) / dist;
			i_specular = phong_blinn_coef( L, V, N ) * theMaterial.specular * theLights[i].specular;

			if( theLights[i].cosCutOff == 0 ) {	// Argi locala
				float d = local_atenuation( theLights[i].attenuation, dist );
				i_diffuse = theMaterial.diffuse * theLights[i].diffuse; // i_diff = m_diff * s_diff
				i_tot_light += d * dot( N, L ) * ( i_diffuse + i_specular );

			} else if( dot( ( -1 ) * L, theLights[i].spotDir ) > theLights[i].cosCutOff ) {	// Spotlight
				i_diffuse = theMaterial.diffuse * theLights[i].diffuse; // i_diff = m_diff * s_diff
				i_tot_light += spotlight_coef( L, theLights[i].spotDir, theLights[i].exponent ) * dot( N, L ) * ( i_diffuse + i_specular );
			}
		}
	}
	vec4 f_color = vec4( 1 ); //clamp(vec4(v_position, 1), 0, 1.0)
	f_color.rgb = scene_ambient + i_tot_light;
	vec4 texColor = texture2D( texture0, f_texCoord );
	gl_FragColor = f_color * texColor;

}
