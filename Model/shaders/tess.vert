#version 420 core

uniform mat4 mvmat;
uniform mat4 pmat;

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vNormal;

out gl_PerVertex {
	vec4 gl_Position;
};
out vec4 tcColor;

void main() {
	gl_Position = pmat * mvmat * vVertex;
	tcColor = vec4(1.0, 0.0, 0.0, 0.0);
}
