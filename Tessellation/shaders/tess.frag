#version 420 core
out vec4 fragColor;
flat in vec4 fColor;

void main() {
	fragColor = fColor;
}
