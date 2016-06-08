#version 430 core

layout(vertices = 4) out;

layout(location = 0) uniform float tessLevelOuter;
layout(location = 1) uniform float tessLevelInner;

in gl_PerVertex {
	vec4 gl_Position;
} gl_in[];
in vec4 tcColor[];

out gl_PerVertex {
	vec4 gl_Position;
} gl_out[];
out vec4 teColor[];

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
	teColor[gl_InvocationID] = tcColor[gl_InvocationID];
}
