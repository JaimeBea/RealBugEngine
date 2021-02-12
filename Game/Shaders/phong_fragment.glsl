#version 460

in vec3 fragNormal;
in vec2 uv;
out vec4 color;

uniform sampler2D diffuseTexture;

uniform float Kd;
uniform float Ks;
uniform int n;
uniform vec3 ambientColor;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform vec3 cameraDirection;

void main() { 
	vec3 diffuseColor = texture(diffuseTexture, uv).rgb;
	vec3 fragN = normalize(fragNormal);
	vec3 lightN = normalize(-lightDirection);
	vec3 viewN = normalize(cameraDirection); 
		
	// diffuse
	float NL = max(dot(fragN, lightN),0); 
	vec3 diffuse = Kd * diffuseColor * lightColor * NL;

	// specular
	vec3 reflection = reflect(lightN, fragN);
	float VR = max(dot(viewN, reflection),0);
	vec3 specular = Ks * lightColor * pow(VR,n);
	
	// ambient
	vec3 ambient = ambientColor * diffuseColor;
	
	color.rgb = ambient + diffuse + specular; 
	color.a = 1.0;
}
