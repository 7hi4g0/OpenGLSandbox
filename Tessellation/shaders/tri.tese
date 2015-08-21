#version 420 core

layout(triangles, equal_spacing, ccw) in;

in vec4 teColor[];

out vec4 gColor;

vec4 interpolate(vec4 v0, vec4 v1, vec4 v2);

void main() {
	gl_Position = interpolate(
		gl_in[0].gl_Position,
		gl_in[1].gl_Position,
		gl_in[2].gl_Position
	);
	gColor = interpolate(
		teColor[0],
		teColor[1],
		teColor[2]
	);
}

vec4 interpolate(vec4 v0, vec4 v1, vec4 v2) {
	return	v0 * gl_TessCoord.x +
			v1 * gl_TessCoord.y +
			v2 * gl_TessCoord.z;
}
