#version 460

in vec3 frag_normal;
in vec3 frag_pos;
in vec2 uv;
out vec4 out_color;

struct Material
{
	vec3 diffuse_color;
	sampler2D diffuse_map;
	vec3 specular_color;
	sampler2D specular_map;
	float shininess;
	vec3 ambient;
	
	int has_diffuse_map;
	int has_specular_map;
	int shininess_alpha;
};

struct Light
{
	vec3 position;
	vec3 color;
};

uniform Material material;
uniform Light light;
uniform vec3 view_pos;

void main()
{    
    vec3 frag_n = normalize(frag_normal);
    vec3 light_n = normalize(light.position - frag_pos);
    vec3 view_n = normalize(view_pos - frag_pos);
   	
    // ambient
    vec3 ambient = light.color * material.ambient;
  	
    // diffuse 
    vec3 diffuse_color = material.has_diffuse_map * texture(material.diffuse_map, uv).rgb + (1 - material.has_diffuse_map) * material.diffuse_color;
    float NL = max(dot(frag_n, light_n), 0.0);
    vec3 diffuse = light.color * diffuse_color * NL;
    
    // specular
    vec4 specular_color = material.has_specular_map * texture(material.specular_map, uv) + (1 - material.has_specular_map) * vec4(material.specular_color, 0.0);
    float shininess = material.shininess_alpha * exp2(specular_color.a * 7+1) + (1 - material.shininess_alpha) * material.shininess;
    vec3 reflect_dir = reflect(-light_n, frag_n);  
    float VRn = pow(max(dot(view_n, reflect_dir), 0.0), shininess);
    vec3 specular = light.color * (VRn * specular_color.rgb);  
        
    vec3 color = ambient + diffuse + specular;
    out_color.rgb = pow(color.rgb, vec3(1/2.2));
    out_color.a = 1.0;
  }  