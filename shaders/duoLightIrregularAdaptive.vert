#version 430 core

uniform mat4 uModelView;
uniform mat4 uProjection;
uniform vec4 uLightPos[2];
uniform vec3 uColor;

layout(location = 0) in vec4 vVertex;
layout(location = 1) in uint vValence;

layout(std430, binding = 0) buffer VertexBuffer {
	vec3 vertexPosition[];
};

layout(std430, binding = 1) buffer ValenceBuffer {
	uint vertexValence[];
};

out gl_PerVertex {
	vec4 gl_Position;
};
out vec4 gColor;

vec4 lightColor(vec4 normal, vec3 lightPos, vec4 baseColor);

const float PI = 3.14159265359;
const float Bx[3] = {0.5f/3.0f, 2.0f/3.0f, 0.5f/3.0f};
const float Dx[3] = {-0.5f, 0.0f, 0.5f};
#define By Bx
#define Dy Dx

void main() {
	vec4 baseColor = vec4(uColor, 0.0);

	uint valence = vertexValence[vValence];
	float fvalence = valence;
	vec3 tangent1 = vec3(0.0);
	vec3 tangent2 = vec3(0.0);

	if (valence == 4) {
		vec3 controlPoints[8];

		for (uint i = 0; i < 4; i++) {
			controlPoints[2 * i + 0] = vertexPosition[vertexValence[vValence + 2 * i + 0 + 1]].xyz;
			controlPoints[2 * i + 1] = vertexPosition[vertexValence[vValence + 2 * i + 1 + 1]].xyz;
		}

    	vec3 diag0 =		controlPoints[7] * Bx[0] * By[0]	+	controlPoints[3] * Bx[2] * By[2];						
		vec3 diag1 =		controlPoints[1] * Bx[0] * By[2]	+	controlPoints[5] * Bx[2] * By[0];						
		vec3 midcross0 = 	controlPoints[0] * Bx[1] * By[0]	+	controlPoints[4] * Bx[1] * By[2];							
		vec3 midcross1 = 	controlPoints[2] * Bx[0] * By[1]	+	controlPoints[6] * Bx[2] * By[1];	
						
		vec3 diag = diag0 + diag1;
		vec3 midcross = midcross0 + midcross1;
		
		vec3 mid = vVertex.xyz *  Bx[1] * By[1];
		
		//TODO: vVertex.z should be 1.0?
		gl_Position = vec4((diag + midcross) + mid, 1.0);
		
			
		vec3 diag0_dx =		controlPoints[7] * Dx[0] * By[0]	+	controlPoints[3] * Dx[2] * By[2];					
		vec3 diag1_dx =		controlPoints[5] * Dx[0] * By[2]	+	controlPoints[1] * Dx[2] * By[0];						
		vec3 midcross0_dx =	controlPoints[0] * Dx[1] * By[0]	+	controlPoints[4] * Dx[1] * By[2];						
		vec3 midcross1_dx =	controlPoints[6] * Dx[0] * By[1]	+	controlPoints[2] * Dx[2] * By[1];								
								
		vec3 diag0_dy =		controlPoints[7] * Bx[0] * Dy[0]	+	controlPoints[3] * Bx[2] * Dy[2];					
		vec3 diag1_dy =		controlPoints[5] * Bx[0] * Dy[2]	+	controlPoints[1] * Bx[2] * Dy[0];						
		vec3 midcross0_dy =	controlPoints[0] * Bx[1] * Dy[0]	+	controlPoints[4] * Bx[1] * Dy[2];						
		vec3 midcross1_dy =	controlPoints[6] * Bx[0] * Dy[1]	+	controlPoints[2] * Bx[2] * Dy[1];	
								
		vec3 midcross_dx = midcross0_dx + midcross1_dx;
		vec3 midcross_dy = midcross0_dy + midcross1_dy;
		
		tangent1 = (diag0_dx + diag1_dx) + midcross_dx;
		tangent2 = (diag0_dy + diag1_dy) + midcross_dy;
	} else {
		float cos_valence = cos(PI / fvalence);
		float tmp = (sqrt(4.0 + cos_valence * cos_valence) - cos_valence) * 0.25;

		vec3 pos = vVertex.xyz * (fvalence * fvalence);

		for (uint i = 0; i < valence; i++) {
			vec3 edge = vertexPosition[vertexValence[vValence + 2 * i + 0 + 1]].xyz;
			vec3 face = vertexPosition[vertexValence[vValence + 2 * i + 1 + 1]].xyz;

			pos += edge * 4.0 + face;

			float alpha1 = cos((2.0 * PI * i) / fvalence);
			float beta1 = tmp * cos((2.0 * PI * i + PI) / fvalence);
			float alpha2 = sin((2.0 * PI * i) / fvalence);
			float beta2 = tmp * sin((2.0 * PI * i + PI) / fvalence);

			tangent1 += alpha1 * edge + beta1 * face;
			tangent2 += alpha2 * edge + beta2 * face;
		}

		gl_Position = vec4(pos / (fvalence * (fvalence + 5.0)), 1.0);
	}

	gl_Position = uModelView * gl_Position;
	vec4 normal = normalize(uModelView * vec4(cross(tangent1, tangent2), 0.0));
	gColor = lightColor(normal, uLightPos[0].xyz, baseColor) + lightColor(normal, uLightPos[1].xyz, baseColor);

	gl_Position = uProjection * gl_Position;
}

vec4 lightColor(vec4 normal, vec3 lightPos, vec4 baseColor) {
	vec3 lightDir = lightPos - gl_Position.xyz;
	float dist = length(lightDir);
	lightDir = normalize(lightDir);

	return 0.8 * clamp(dot(normal.xyz, lightDir), 0.0, 1.0) * baseColor / (1 + 0.1 * dist + (0.01 / 9) * dist * dist);
}
