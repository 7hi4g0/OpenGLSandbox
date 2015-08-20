#version 420 core

layout(vertices = 4) out;

in vec4 tcColor[];

out vec4 teColor[];

void main() {
	gl_TessLevelOuter[0] = 2.0;
	gl_TessLevelOuter[1] = 2.0;
	gl_TessLevelOuter[2] = 2.0;
	gl_TessLevelOuter[3] = 2.0;

	gl_TessLevelInner[0] = 2.0;
	gl_TessLevelInner[1] = 2.0;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	teColor[gl_InvocationID] = tcColor[gl_InvocationID];
}
