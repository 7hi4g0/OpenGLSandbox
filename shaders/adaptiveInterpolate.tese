#version 410 core

layout(quads, equal_spacing, ccw) in;

in gl_PerVertex {
	vec4 gl_Position;
} gl_in[];
in vec4 teColor[];

out gl_PerVertex {
	vec4 gl_Position;
};
out vec4 gColor;

void main() {
	mat4 B = mat4(-1.0,  3.0, -3.0,  1.0,
				   3.0, -6.0,  0.0,  4.0,
				  -3.0,  3.0,  3.0,  1.0,
				   1.0,  0.0,  0.0,  0.0);
	float U[4];
	float V[4];

	float Nu[4];
	float Nv[4];

	vec4 NuP[4];
	vec4 res;

	U[0] = gl_TessCoord.x * (U[1] = gl_TessCoord.x * (U[2] = gl_TessCoord.x * (U[3] = 1)));
	V[0] = gl_TessCoord.y * (V[1] = gl_TessCoord.y * (V[2] = gl_TessCoord.y * (V[3] = 1)));
	B = (1/6) * B;

	for (int i = 0; i < 4; i++) {
		Nu[i] = 0;
		Nv[i] = 0;
		for (int j = 0; j < 4; j++) {
			Nu[i] += U[j] * B[i][j];
			Nv[i] += V[j] * B[i][j];
		}
	}

	for (int i = 0; i < 4; i++) {
		NuP[i] = vec4(0);
		for (int j = 0; j < 4; j++) {
			NuP[i] += Nu[j] * gl_in[4 * j + i].gl_Position;
		}
	}

	gl_Position = vec4(0);

	for (int i = 0; i < 4; i++) {
		gl_Position += NuP[i] * Nv[i];
	}

	gColor = teColor[0];
}
