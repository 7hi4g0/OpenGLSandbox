#version 420 core

layout(triangles, invocations = 1) in;
layout(triangle_strip, max_vertices = 3) out;

in vec4 gColor[];

flat out vec4 fColor;

void main() {
	gl_Position = gl_in[0].gl_Position;
	fColor = gColor[0];
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	fColor = gColor[1];
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	fColor = gColor[2];
	EmitVertex();
//	gl_Position = gl_in[0].gl_Position;
//	fColor = gColor[0];
//	EmitVertex();

	EndPrimitive();
}
