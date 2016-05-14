#version 330 core
#extension GL_ARB_tessellation_shader : require
#extension GL_ARB_gpu_shader5 : require

flat in vec4 fColor;

out vec4 fragColor;

void main() {
	fragColor = fColor;
}
