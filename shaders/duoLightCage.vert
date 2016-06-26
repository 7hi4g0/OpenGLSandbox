#version 410 core

uniform mat4 uModelView;
uniform mat4 uProjection;
uniform vec4 uLightPos[2];
uniform vec3 uColor;

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
	vec4 baseColor = vec4(uColor, 0.0);

	gl_Position = gl_Position + (0.01 * normal);

	tcColor = lightColor(normal, uLightPos[0].xyz, baseColor) + lightColor(normal, uLightPos[1].xyz, baseColor);
	gl_Position = uProjection * gl_Position;
}

vec4 lightColor(vec4 normal, vec3 lightPos, vec4 baseColor) {
	vec3 lightDir = lightPos - gl_Position.xyz;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	return 0.8 * clamp(dot(normal.xyz, lightDir), 0.0, 1.0) * baseColor / (1 + 0.1 * dist + (0.01 / 9) * dist * dist);
}
