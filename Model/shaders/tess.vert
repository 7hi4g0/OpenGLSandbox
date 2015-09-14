#version 420 core

uniform mat4 uModelView;
uniform mat4 uProjection;
uniform vec3 uLightPos;

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec4 vNormal;

out gl_PerVertex {
	vec4 gl_Position;
};
out vec4 tcColor;

void main() {
	gl_Position = uModelView * vVertex;

	vec4 normal = normalize(vNormal);
	vec4 lightDir = normalize(vec4(uLightPos, 1.0) - gl_Position);

	tcColor = clamp(dot(normal, lightDir), 0.0, 1.0) * vec4(1.0, 0.0, 0.0, 0.0);
	gl_Position = uProjection * gl_Position;
}
