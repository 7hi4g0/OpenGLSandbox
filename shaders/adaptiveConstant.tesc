#version 410 core

layout(vertices = 16) out;

uniform float tessLevelOuter;
uniform float tessLevelInner;

in gl_PerVertex {
	vec4 gl_Position;
} gl_in[];

out gl_PerVertex {
	vec4 gl_Position;
} gl_out[];

void main() {
	gl_TessLevelOuter[0] =
	gl_TessLevelOuter[1] =
	gl_TessLevelOuter[2] =
	gl_TessLevelOuter[3] =
		tessLevelOuter;

	gl_TessLevelInner[0] =
	gl_TessLevelInner[1] =
		tessLevelInner;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}
