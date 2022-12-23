#version 120

varying vec3 f_texCoord;
uniform samplerCube texture0;

// To sample a texel from a cubemap, use "textureCube" function:
//
// vec4 textureCube(samplerCube sampler, vec3 coord);

void main() {
	gl_FragColor = textureCube(texture0, f_texCoord);
}
