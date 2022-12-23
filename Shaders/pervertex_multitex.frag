#version 120

varying vec4 f_color;
varying vec2 f_texCoord;

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float uCloudOffset; // The offset of the cloud texture

void main() {

	// The final color must be a linear combination of both
	// textures with a factor of 0.5, e.g:
	//
	// color = 0.5 * texture0 + 0.5 * texture1
	vec4 t1 = texture2D(texture0, f_texCoord);
	vec2 cloudCoords = vec2(f_texCoord[0]+uCloudOffset,f_texCoord[1]);
	vec4 t2 = texture2D(texture1, cloudCoords);
	gl_FragColor = 0.5*t1 + 0.5*t2;
}
