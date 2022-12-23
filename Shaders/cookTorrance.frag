#version 120

uniform int active_lights_n; // Number of active lights (< MG_MAX_LIGHT)
uniform vec3 scene_ambient; // Scene ambient light

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
	vec3 diffuse;  // albedoa hemendik lortuko dugu
	vec3 specular; // fresnel islapen faktorea (lambda) hemendik lortu ---> k_d = lambda eta k_s = (1,1,1) - k_d 
	float alpha;
	float shininess;
} theMaterial;

uniform sampler2D texture0;
uniform float roughness;

varying vec3 f_position;      // camera space
varying vec3 f_viewDirection; // camera space
varying vec3 f_normal;        // camera space
varying vec2 f_texCoord;

float r = roughness;
vec3 albedo = theMaterial.diffuse;
vec3 lambda = theMaterial.specular;
vec3 k_diff = lambda;
vec3 k_spec = vec3( 1 ) - k_diff;

const float PI = 3.14159265358979323846;

vec3 halfway( vec3 l, vec3 v ) {
	return normalize( l + v );
}

float spotlight_coef( vec3 l, vec3 dir, float exp_ ) {
	return pow( max( dot( ( -1 ) * l, dir ), 0 ), exp_ );
}

float local_atenuation( vec3 att, float dist ) {
	return 1 / ( att.x + att.y * dist + att.z * pow( dist, 2 ) );
}

float func_D( vec3 h, vec3 n ) {
	float r_2 = r * r;
	float h_n_2 = pow( dot( h, n ), 2 );
	float coef = 1 / ( PI * r_2 * pow( h_n_2, 2 ) );
	float exponent = ( h_n_2 - 1 ) / ( r_2 * h_n_2 );
	return coef * exp( exponent );
}

float func_G( vec3 h, vec3 n, vec3 v, vec3 l ) {

	float n_h = dot( n, h );
	float v_h = dot( v, h );

	float masking = 2 * n_h * dot( n, v ) / v_h;
	float shadowing = 2 * n_h * dot( n, l ) / v_h;

	if( masking >= 1 && shadowing >= 1 ) {
		return 1.0; // OKLUSIORIK EZ!
	} else {
		return min( masking, shadowing );
	}
}

vec3 func_F( vec3 l, vec3 h ) {
	return ( 1 - lambda ) * pow( ( 1 - dot( l, h ) ), 5 ) + lambda;
}

vec3 diffuse_f( ) {
	return albedo / PI;
}

vec3 specular_f( vec3 h, vec3 n, vec3 v, vec3 l ) {
	return ( func_D( h, n ) * func_F( l, h ) * func_G( h, n, v, l ) ) / ( 4 * dot( n, l ) * dot( n, v ) );
}

void main( ) {
	// Interpolazio bidez lortutako norabideak normalizatu
	vec3 V = normalize( f_viewDirection );
	vec3 N = normalize( f_normal );
	vec3 L;

	vec3 H;
	vec3 f_r;

	// Argi guztiak korritzeko
	for( int i = 0; i < active_lights_n; i++ ) {
		if( theLights[i].position[3] == 0 ) {	// Argi infinituak (Directional) bektoreak direnez light_t.position[3]==0 izango da (koord. homog.)

			L = normalize( -1 * theLights[i].position.xyz );
			H = halfway( L, V );

			f_r = ( diffuse_f( ) * k_diff ) + ( specular_f( H, N, V, L ) * k_spec );

			i_tot_light += theLights[i].diffuse * max( 0, dot( N, L ) ) * f_r;

		} else { // Spotlight edo Argi locala (local)

			float dist = length( theLights[i].position.xyz - f_position );	// Argiaren eta eraso-puntuaren arteko distantzia
			L = ( theLights[i].position.xyz - f_position.xyz ) / dist;
			H = halfway( L, V );

			f_r = ( diffuse_f( ) * k_diff ) + ( specular_f( H, N, V, L ) * k_spec );

			if( theLights[i].cosCutOff == 0 ) {	// Argi locala

				float d = local_atenuation( theLights[i].attenuation, dist );
				i_tot_light += d * max( 0, dot( N, L ) ) * theLights[i].diffuse * f_r;

			} else if( dot( ( -1 ) * L, theLights[i].spotDir ) > theLights[i].cosCutOff ) {	// Spotlight
				i_tot_light += spotlight_coef( L, theLights[i].spotDir, theLights[i].exponent ) * theLights[i].diffuse * max( 0, dot( N, L ) ) * f_r;
			}
		}
	}
	vec4 f_color = vec4( 1 ); //clamp(vec4(v_position, 1), 0, 1.0)
	f_color.rgb = scene_ambient + i_tot_light;
	vec4 texColor = texture2D( texture0, f_texCoord );
	gl_FragColor = f_color * texColor;

}
