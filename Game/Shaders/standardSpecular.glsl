#define PI 3.1415926538
#ifdef VERTEX
in layout(location=0) vec3 pos;
in layout(location=1) vec3 normal;
in layout(location=2) vec2 uvs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 uv;

void main()
{
 gl_Position = proj * view * model * vec4(pos, 1.0);
 fragNormal = normalize(transpose(inverse(mat3(model))) * normal);
 fragPos = vec3(model * vec4(pos, 1.0));
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
layout(binding=0) uniform sampler2D diffuseMap;
uniform vec3 specularColor;
layout(binding=1) uniform sampler2D specularMap;

uniform int hasSpecularMap;
uniform int hasSmoothnessAlpha; // else: Diffuse alpha channel
uniform float smoothness;

struct AmbientLight
{
	vec3 color;
};

struct DirLight
{
	vec3 direction;
	vec3 color;
	float intensity;
    int isActive;
};

struct PointLight
{
	vec3 pos;
	vec3 color;
	float intensity;
	float kc;
	float kl;
	float kq;
};

struct SpotLight
{
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

struct Light
{
	AmbientLight ambient;
	DirLight directional;
	PointLight points[8];
	int numPoints;
	SpotLight spots[8];
	int numSpots;
};

uniform Light light;
uniform vec3 viewPos;


float GGXNormalDistribution(float NH, float roughness)
{
    return roughness * roughness / (PI * pow(((NH * NH) * (roughness * roughness - 1) + 1), 2));
}

vec3 ProcessDirectionalLight(DirLight directional, vec3 fragNormal, vec3 viewDir, vec3 Cd, vec3 F0, float roughness)
{
    vec3 directionalDir = - normalize(directional.direction);

    float NL = max(dot(fragNormal, directionalDir), 0.0);
    float NV = max(dot(fragNormal, viewDir), 0.0);
    vec3 H = (directionalDir + viewDir) / length(directionalDir + viewDir);
    float NH = max(dot(fragNormal, H), 0.0);
    float LH = max(dot(directionalDir, H), 0.0);

    vec3 diffuse = directional.color * directional.intensity * NL;

    // Schlick Fresnel
    vec3 Fn = F0 + (1 - F0) * pow(1 - LH, 5);

    // Smith Visibility Function
    float V = 0.5 / (NL * (NV * (1 - roughness) + roughness) + NV * (NL * (1 - roughness) + roughness));

    float NDF = GGXNormalDistribution(NH, roughness);

   	// Cook-Torrance BRDF
    return (Cd * (1 - F0) + 0.25 * Fn * V * NDF) * diffuse;
}

vec3 ProcessPointLight(PointLight point, vec3 fragNormal, vec3 fragPos, vec3 viewDir, vec3 Cd, vec3 F0, float roughness)
{
    float pointDistance = length(point.pos - fragPos);
	float distAttenuation = 1.0 / (point.kc + point.kl * pointDistance + point.kq * pointDistance * pointDistance);

    vec3 pointDir = normalize(point.pos - fragPos);

    float NL = max(dot(fragNormal, pointDir), 0.0);
    float NV = max(dot(fragNormal, viewDir), 0.0);
    vec3 H = (pointDir + viewDir) / length(pointDir + viewDir);
    float NH = max(dot(fragNormal, H), 0.0);
    float LH = max(dot(pointDir, H), 0.0);

    vec3 diffuse = point.color * point.intensity * distAttenuation * NL;

    // Schlick Fresnel
    vec3 Fn = F0 + (1 - F0) * pow(1 - LH, 5);

    // Smith Visibility Function
    float V = 0.5 / (NL * (NV * (1 - roughness) + roughness) + NV * (NL * (1 - roughness) + roughness));

    float NDF = GGXNormalDistribution(NH, roughness);

   	// Cook-Torrance BRDF
    return (Cd * (1 - F0) + 0.25 * Fn * V * NDF) * diffuse;
}

vec3 ProcessSpotLight(SpotLight spot, vec3 fragNormal, vec3 fragPos, vec3 viewDir, vec3 Cd, vec3 F0, float roughness)
{
    float spotDistance = length(spot.pos - fragPos);
	float distAttenuation = 1.0 / (spot.kc + spot.kl * spotDistance + spot.kq * spotDistance * spotDistance);

    vec3 spotDir = normalize(spot.pos - fragPos);

    vec3 aimDir = normalize(spot.direction);
    float C = dot( aimDir , -spotDir);
    float cAttenuation = 0;
    float cosInner = cos(spot.innerAngle);
    float cosOuter = cos(spot.outerAngle);
    if( C > cosInner)
    {
        cAttenuation = 1;
    }
    else if (cosInner > C && C > cosOuter)
    {
    	cAttenuation = (C - cosOuter) / (cosInner - cosOuter);
    }

    float NL = max(dot(fragNormal, spotDir), 0.0);
    float NV = max(dot(fragNormal, viewDir), 0.0);
    vec3 H = (spotDir + viewDir) / length(spotDir + viewDir);
    float NH = max(dot(fragNormal, H), 0.0);
    float LH = max(dot(spotDir, H), 0.0);

    vec3 diffuse = spot.color * spot.intensity * distAttenuation * cAttenuation * NL;

    // Schlick Fresnel
    vec3 Fn = F0 + (1 - F0) * pow(1 - LH, 5);

    // Smith Visibility Function
    float V = 0.5 / (NL * (NV * (1 - roughness) + roughness) + NV * (NL * (1 - roughness) + roughness));

    float NDF = GGXNormalDistribution(NH, roughness);

   	// Cook-Torrance BRDF
    return (Cd * (1 - F0) + 0.25 * Fn * V * NDF) * diffuse;
}

void main()
{
	vec3 viewDir = normalize(viewPos - fragPos);

	vec4 colorDiffuse = pow(texture(diffuseMap, uv), vec4(2.2)) * vec4(diffuseColor, 1.0);
	vec4 colorSpecular = hasSpecularMap * pow(texture(specularMap, uv), vec4(2.2)) + (1 - hasSpecularMap) * vec4(specularColor, 1.0);

    float roughness = pow(1 - smoothness, 2) * pow(1 - smoothness * (hasSmoothnessAlpha * colorSpecular.a + (1 - hasSmoothnessAlpha) * colorDiffuse.a), 2);

    vec3 colorAccumulative = colorDiffuse.rgb * light.ambient.color;

    // Directional Light
    if (light.directional.isActive == 1)
    {
    	colorAccumulative += ProcessDirectionalLight(light.directional, fragNormal, viewDir, colorDiffuse.rgb, colorSpecular.rgb, roughness);
    }

	// Point Light
	for(int i = 0; i < light.numPoints; i++)
	{
    	colorAccumulative += ProcessPointLight(light.points[i], fragNormal, fragPos, viewDir, colorDiffuse.rgb, colorSpecular.rgb, roughness);
	}

    // Spot Light
	for(int i = 0; i < light.numSpots; i++)
	{
    	colorAccumulative += ProcessSpotLight(light.spots[i], fragNormal, fragPos, viewDir, colorDiffuse.rgb, colorSpecular.rgb, roughness);
	}

    vec3 ldr = colorAccumulative.rgb / (colorAccumulative.rgb + vec3(1.0)); // reinhard tone mapping
	ldr = pow(ldr, vec3(1/2.2)); // gamma correction
	outColor = vec4(ldr, 1.0);
  }
#endif