#version 460

in vec3 frag_normal;
in vec3 frag_pos;
in vec2 uv;
out vec4 out_color;

// Material
uniform vec3 diffuse_color;
uniform sampler2D diffuse_map;
uniform vec3 specular_color;
uniform sampler2D specular_map;
uniform float shininess;
	
uniform int has_diffuse_map;
uniform int has_specular_map;
uniform int shininess_alpha;

struct AmbientLight
{
	vec3 color;
};

struct DirLight
{
	vec3 direction;
	vec3 color;
	float intensity;
    int is_active;
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
	float inner_angle;
	float outer_angle;
};

struct Light
{
	AmbientLight ambient;
	DirLight directional;
	PointLight points[8];
	int num_points;
	SpotLight spots[8];
	int num_spots;
};

uniform Light light;
uniform vec3 view_pos;

void main()
{    
	vec3 frag_n = normalize(frag_normal);
	vec3 view_n = normalize(view_pos - frag_pos);
	
	// diffuse 
	vec3 diffuse_color = has_diffuse_map * pow(texture(diffuse_map, vec2(uv.x, 1 - uv.y)).rgb, vec3(2.2)) + (1 - has_diffuse_map) * diffuse_color;
	
	// specular
	vec4 specular_color = has_specular_map * pow(texture(specular_map, vec2(uv.x, 1 - uv.y)), vec4(2.2)) + (1 - has_specular_map) * vec4(specular_color, 0.0);
	vec3 Rf0 = specular_color.rgb;

	// shininess
	float shininess = shininess_alpha * exp2(specular_color.a * 7 + 1) + (1 - shininess_alpha) * shininess;
	
	// Ambient Color
	vec3 ambient_color = diffuse_color * light.ambient.color;
    
	vec3 accumulative_color = ambient_color;
	
    // Directional Light
    if (light.directional.is_active == 1)
    {
        vec3 directional_dir = normalize(light.directional.direction);
        float NL = max(dot(frag_n, -directional_dir), 0.0);
        vec3 diffuse = light.directional.color * light.directional.intensity * NL;
        
        vec3 reflect_dir = reflect(directional_dir, frag_n);  
        float VRn = pow(max(dot(view_n, reflect_dir), 0.0), shininess);  
    
        
        vec3 Rf = Rf0 + (1 - Rf0) * pow(1 - NL, 5); 
    
        vec3 directional_color = (diffuse_color * (1 - Rf0) + (shininess + 2) / 2 * Rf * VRn) * diffuse;
    
        accumulative_color = accumulative_color + directional_color;
    }
    
	// Point Light
	
	for(int i = 0; i < light.num_points; i++)
	{
	    float point_distance = length(light.points[i].pos - frag_pos);
	    float dist_attenuation = 1.0 / (light.points[i].kc + light.points[i].kl * point_distance + light.points[i].kq * point_distance * point_distance);
    
        vec3 point_dir = normalize(frag_pos - light.points[i].pos);
        float NL = max(dot(frag_n, -point_dir), 0.0);
        vec3 diffuse = light.points[i].color * light.points[i].intensity * dist_attenuation * NL;
    
        vec3 reflect_dir = reflect(point_dir, frag_n);  
        float VRn = pow(max(dot(view_n, reflect_dir), 0.0), shininess);
    
        vec3 Rf = Rf0 + (1 - Rf0) * pow(1 - NL, 5); 
    
        vec3 point_color = (diffuse_color * (1 - Rf0) + (shininess + 2) / 2 * Rf * VRn) * diffuse;
    
        accumulative_color = accumulative_color + point_color;
    }
    
    // Spot Light
	for(int i = 0; i < light.num_spots; i++)
	{
	    float spot_distance = length(light.spots[i].pos - frag_pos);
	    float dist_attenuation = 1.0 / (light.spots[i].kc + light.spots[i].kl * spot_distance + light.spots[i].kq * spot_distance * spot_distance);
        
        vec3 spot_dir = normalize(frag_pos - light.spots[i].pos);
    
        vec3 aim_dir = normalize(light.spots[i].direction);
        float C = dot( aim_dir ,spot_dir);
        float c_attenuation = 0;
        float cos_i = cos(light.spots[i].inner_angle);
        float cos_o = cos(light.spots[i].outer_angle);
        if( C > cos_i)
        {
            c_attenuation = 1;
        }
        else if (cos_i > C && C > cos_o)
        {
    	    c_attenuation = (C - cos_o) / (cos_i - cos_o);
        }
    
        float NL = max(dot(frag_n, -spot_dir), 0.0);
    
        vec3 diffuse = light.spots[i].color * light.spots[i].intensity * dist_attenuation * c_attenuation * NL;
    
        vec3 reflect_dir = reflect(spot_dir, frag_n);  
        float VRn = pow(max(dot(view_n, reflect_dir), 0.0), shininess);
    
        vec3 Rf = Rf0 + (1 - Rf0) * pow(1 - NL, 5); 
    
        vec3 spot_color = (diffuse_color * (1 - Rf0) + (shininess + 2) / 2 * Rf * VRn) * diffuse;
    
        accumulative_color = accumulative_color + spot_color;
    }
    
    vec3 ldr = accumulative_color.rgb / (accumulative_color.rgb + vec3(1.0)); // reinhard tone mapping
	ldr = pow(ldr, vec3(1/2.2)); // gamma correction
	out_color = vec4(ldr, 1.0);
  }