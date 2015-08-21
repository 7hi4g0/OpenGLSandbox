#version 420 core

layout(triangles, invocations = 1) in;
layout(line_strip, max_vertices = 4) out;

in vec4 gColor[];

flat out vec4 fColor;

void main() {
//	fColor = ((gColor[0] + gColor[1] + gColor[2]) / 3 + gColor[0]) / 2;

	gl_Position = gl_in[0].gl_Position;
	fColor = gColor[0];
	EmitVertex();
	gl_Position = gl_in[1].gl_Position;
	fColor = gColor[1];
	EmitVertex();
	gl_Position = gl_in[2].gl_Position;
	fColor = gColor[2];
	EmitVertex();
	gl_Position = gl_in[0].gl_Position;
	fColor = gColor[0];
	EmitVertex();

	EndPrimitive();
}
