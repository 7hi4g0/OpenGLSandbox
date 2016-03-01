#version 410 core

layout(triangles, invocations = 1) in;
layout(triangle_strip, max_vertices = 3) out;

in gl_PerVertex {
	vec4 gl_Position;
} gl_in[];
in vec4 gColor[];

out gl_PerVertex {
	vec4 gl_Position;
};
flat out vec4 fColor;

void main() {
	fColor = ((gColor[0] + gColor[1] + gColor[2]) / 3 + gColor[0]) / 2;

	gl_Position = gl_in[0].gl_Position;
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	EmitVertex();

	EndPrimitive();
}
