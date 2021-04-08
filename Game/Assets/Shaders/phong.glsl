#ifdef VERTEX

#define MAX_BONES 100

in layout(location=0) vec3 pos;
in layout(location=1) vec3 norm;
in layout(location=2) vec2 uvs;
in layout(location=3) uvec4 boneIndices;
in layout(location=4) vec4 boneWeitghts;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat4 palette[MAX_BONES];
uniform bool hasBones;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 uv;

void main() {
	vec4 position;
	vec4 normal;
	if (hasBones) {
		mat4 skinT = palette[boneIndices[0]] * boneWeitghts[0] + palette[boneIndices[1]] * boneWeitghts[1]
		+ palette[boneIndices[2]] * boneWeitghts[2] + palette[boneIndices[3]] * boneWeitghts[3];
		position = skinT * vec4(pos, 1.0);
		normal = skinT * vec4(norm, 0.0);
	}
	else {
		position = vec4(pos, 1.0);
		normal = vec4(norm, 0.0);
	}

	gl_Position = proj * view * model * position;
	fragNormal = transpose(inverse(mat3(model))) * normal.xyz;
	fragPos = vec3(model * position);
	uv = uvs;
}
#endif

#ifdef FRAGMENT


in vec3 fragNormal;
in vec3 fragPos;
in vec2 uv;
out vec4 outColor;

// Material
uniform vec3 diffuseColor;
uniform sampler2D diffuseMap;
uniform vec3 specularColor;
uniform sampler2D specularMap;
uniform float shininess;

uniform int hasDiffuseMap;
uniform int hasSpecularMap;
uniform int hasShininessInSpecularAlpha;

struct AmbientLight {
	vec3 color;
};

struct DirLight {
	vec3 direction;
	vec3 color;
	float intensity;
    int isActive;
};

struct PointLight {
	vec3 pos;
	vec3 color;
	float intensity;
	float kc;
	float kl;
	float kq;
};

struct SpotLight {
	vec3 pos;
	vec3 direction;
	vec3 color;
	float intensity;
	float kc;
	float kl;
	float kq;
	float innerAngle;
	float outerAngle;
};

struct Light {
	AmbientLight ambient;
	DirLight directional;
	PointLight points[8];
	int numPoints;
	SpotLight spots[8];
	int numSpots;
};

uniform Light light;
uniform vec3 viewPos;

void main() {
	vec3 fragN = normalize(fragNormal);
	vec3 viewN = normalize(viewPos - fragPos);

	// diffuse
	vec3 diffuseColor = hasDiffuseMap * pow(texture(diffuseMap, vec2(uv.x, 1 - uv.y)).rgb, vec3(2.2)) + (1 - hasDiffuseMap) * diffuseColor;

	// specular
	vec4 specularColor = hasSpecularMap * pow(texture(specularMap, uv), vec4(2.2)) + (1 - hasSpecularMap) * vec4(specularColor, 0.0);
	vec3 Rf0 = specularColor.rgb;

	// shininess
	float shininess = hasShininessInSpecularAlpha * exp2(specularColor.a * 7 + 1) + (1 - hasShininessInSpecularAlpha) * shininess;

	// Ambient Color
	vec3 ambientColor = diffuseColor * light.ambient.color;

	vec3 accumulativeColor = ambientColor;

	// Directional Light
	if (light.directional.isActive == 1) {
		vec3 directionalDir = normalize(light.directional.direction);
		float NL = max(dot(fragN, -directionalDir), 0.0);
		vec3 diffuse = light.directional.color * light.directional.intensity * NL;

		vec3 reflectDir = reflect(directionalDir, fragN);
		float VRn = pow(max(dot(viewN, reflectDir), 0.0), shininess);


		vec3 Rf = Rf0 + (1 - Rf0) * pow(1 - NL, 5);

		vec3 directionalColor = (diffuseColor * (1 - Rf0) + (shininess + 2) / 2 * Rf * VRn) * diffuse;

		accumulativeColor = accumulativeColor + directionalColor;
	}

	// Point Light
	for (int i = 0; i < light.numPoints; i++) {
		float pointDistance = length(light.points[i].pos - fragPos);
		float distAttenuation = 1.0 / (light.points[i].kc + light.points[i].kl * pointDistance + light.points[i].kq * pointDistance * pointDistance);

		vec3 pointDir = normalize(fragPos - light.points[i].pos);
		float NL = max(dot(fragN, -pointDir), 0.0);
		vec3 diffuse = light.points[i].color * light.points[i].intensity * distAttenuation * NL;

		vec3 reflectDir = reflect(pointDir, fragN);
		float VRn = pow(max(dot(viewN, reflectDir), 0.0), shininess);

		vec3 Rf = Rf0 + (1 - Rf0) * pow(1 - NL, 5);

		vec3 pointColor = (diffuseColor * (1 - Rf0) + (shininess + 2) / 2 * Rf * VRn) * diffuse;

		accumulativeColor = accumulativeColor + pointColor;
	}

	// Spot Light
	for (int i = 0; i < light.numSpots; i++) {
		float spotDistance = length(light.spots[i].pos - fragPos);
		float distAttenuation = 1.0 / (light.spots[i].kc + light.spots[i].kl * spotDistance + light.spots[i].kq * spotDistance * spotDistance);

		vec3 spotDir = normalize(fragPos - light.spots[i].pos);

		vec3 aimDir = normalize(light.spots[i].direction);
		float C = dot(aimDir, spotDir);
		float cAttenuation = 0;
		float cosInner = cos(light.spots[i].innerAngle);
		float cosOuter = cos(light.spots[i].outerAngle);
		if (C > cosInner) {
			cAttenuation = 1;
		} else if (cosInner > C && C > cosOuter) {
			cAttenuation = (C - cosOuter) / (cosInner - cosOuter);
		}

		float NL = max(dot(fragN, -spotDir), 0.0);

		vec3 diffuse = light.spots[i].color * light.spots[i].intensity * distAttenuation * cAttenuation * NL;

		vec3 reflectDir = reflect(spotDir, fragN);
		float VRn = pow(max(dot(viewN, reflectDir), 0.0), shininess);

		vec3 Rf = Rf0 + (1 - Rf0) * pow(1 - NL, 5);

		vec3 spotColor = (diffuseColor * (1 - Rf0) + (shininess + 2) / 2 * Rf * VRn) * diffuse;

		accumulativeColor = accumulativeColor + spotColor;
	}

	vec3 ldr = accumulativeColor.rgb / (accumulativeColor.rgb + vec3(1.0)); // reinhard tone mapping
	ldr = pow(ldr, vec3(1/2.2)); // gamma correction
	outColor = vec4(ldr, 1.0);
}
#endif