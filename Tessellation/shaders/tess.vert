#version 420 core

in vec4 vVertex;
in vec4 vColor;

out vec4 tcColor;

void main() {
	gl_Position = vVertex;
	tcColor = vColor;
}
