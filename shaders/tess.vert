#version 420 core

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vColor;

out gl_PerVertex {
	vec4 gl_Position;
};
out vec4 tcColor;

void main() {
	gl_Position = vVertex;
	tcColor = vColor;
}
