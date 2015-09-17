#version 420 core

uniform mat4 uModelView;
uniform mat4 uProjection;
uniform vec3 uLightPos[2];

layout(location = 0) in vec4 vVertex;
layout(location = 1) in vec3 vNormal;

out gl_PerVertex {
	vec4 gl_Position;
};
out vec4 tcColor;

vec4 lightColor(vec4 normal, vec3 lightPos, vec4 baseColor);

void main() {
	gl_Position = uModelView * vVertex;

	vec4 normal = normalize(uModelView * vec4(vNormal, 0.0));
	vec4 baseColor = vec4(1.0, 0.0, 0.0, 0.0);

	tcColor = lightColor(normal, uLightPos[0], baseColor) + lightColor(normal, uLightPos[1], baseColor);
	gl_Position = uProjection * gl_Position;
}

vec4 lightColor(vec4 normal, vec3 lightPos, vec4 baseColor) {
	vec4 lightDir = vec4(lightPos, 1.0) - gl_Position;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	return clamp(dot(normal, lightDir), 0.0, 1.0) * baseColor / (1 + (0.01 / 9) * dist * dist);
}
