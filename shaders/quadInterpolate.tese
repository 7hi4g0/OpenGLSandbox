#version 410 core

layout(quads, equal_spacing, ccw) in;

in gl_PerVertex {
	vec4 gl_Position;
} gl_in[];
in vec4 teColor[];

out gl_PerVertex {
	vec4 gl_Position;
};
out vec4 gColor;

vec4 interpolate(vec4 v0, vec4 v1, vec4 v2, vec4 v3);

void main() {
	gl_Position = interpolate(
		gl_in[0].gl_Position,
		gl_in[1].gl_Position,
		gl_in[2].gl_Position,
		gl_in[3].gl_Position
	);
	gColor = interpolate(
		teColor[0],
		teColor[1],
		teColor[2],
		teColor[3]
	);
}

vec4 interpolate(vec4 v0, vec4 v1, vec4 v2, vec4 v3) {
	vec4 a = mix(v0, v1, gl_TessCoord.x);
	vec4 b = mix(v3, v2, gl_TessCoord.x);
	return mix(a, b, gl_TessCoord.y);
}
