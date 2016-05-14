#version 330 core
#extension GL_ARB_tessellation_shader : require
#extension GL_ARB_gpu_shader5 : require

smooth in vec4 gColor;

out vec4 fragColor;

void main() {
	fragColor = gColor;
}
