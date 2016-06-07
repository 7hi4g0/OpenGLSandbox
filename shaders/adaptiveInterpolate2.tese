#version 410 core

layout(quads, equal_spacing, ccw) in;

uniform mat4 uModelView;
uniform mat4 uProjection;
uniform vec4 uLightPos[2];
uniform vec3 uColor;

in gl_PerVertex {
	vec4 gl_Position;
} gl_in[];

out gl_PerVertex {
	vec4 gl_Position;
};
out vec4 gColor;

vec4 lightColor(vec4 normal, vec3 lightPos, vec4 baseColor);
void evalCubicBSpline(in float u, out float N[4], out float dN[4]);

void main() {
	float N[4];
	float dN[4];

	evalCubicBSpline(gl_TessCoord.x, N, dN);

	vec3 NuCP[4];
	vec3 dNuCP[4];

	for (int i = 0; i < 4; i++) {
		NuCP[i] = vec3(0.0);
		dNuCP[i] = vec3(0.0);
		for (int j = 0; j < 4; j++) {
			vec3 CP = gl_in[4 * i + j].gl_Position.xyz;
			NuCP[i] += CP * N[j];
			dNuCP[i] += CP * dN[j];
		}
	}

	vec3 pos = vec3(0.0);
	vec3 tangent = vec3(0.0);
	vec3 biTangent = vec3(0.0);

	evalCubicBSpline(gl_TessCoord.y, N, dN);

	for (int i = 0; i < 4; i++) {
		pos += N[i] * NuCP[i];
		tangent += N[i] * dNuCP[i];
		biTangent += dN[i] * NuCP[i];
	}

	gl_Position = uModelView * vec4(pos, 1.0);

	vec4 normal = normalize(uModelView * vec4(cross(biTangent, tangent), 0.0));
	vec4 baseColor = vec4(uColor, 0.0);

	gColor = lightColor(normal, uLightPos[0].xyz, baseColor) + lightColor(normal, uLightPos[1].xyz, baseColor);
	gl_Position = uProjection * gl_Position;
}

void evalCubicBSpline(in float u, out float N[4], out float dN[4]) {
	float T = u;
	float S = 1.0 - u;

	N[0] = (S * S * S) / 6.0;
	N[1] = ((4.0 * S * S * S + T * T * T) + (12.0 * S * T * S + 6.0 * T * S * T)) / 6.0;
	N[2] = ((4.0 * T * T * T + S * S * S) + (12.0 * T * S * T + 6.0 * S * T * S)) / 6.0;
	N[3] = (T * T * T) / 6.0;

	dN[0] = (- S * S) / 2.0;
	dN[1] = (- T * T - 4.0 * T * S) / 2.0;
	dN[2] = (S * S + 4.0 * S * T) / 2.0;
	dN[3] = (T * T) / 2.0;
}

vec4 lightColor(vec4 normal, vec3 lightPos, vec4 baseColor) {
	vec4 lightDir = vec4(lightPos, 1.0) - gl_Position;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	return clamp(dot(normal, lightDir), 0.0, 1.0) * baseColor / (1 + (0.01 / 9) * dist * dist);
}
