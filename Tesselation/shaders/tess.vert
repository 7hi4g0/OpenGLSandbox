#version 420 core
in vec4 vVertex;
in vec4 vColor;
flat out vec4 fColor;

void main() {
	gl_Position = vVertex;
	fColor = vColor;
}
