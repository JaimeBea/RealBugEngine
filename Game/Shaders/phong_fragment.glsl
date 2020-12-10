#version 460

in vec3 frag_normal;
in vec2 uv;
out vec4 color;

uniform sampler2D diffuse_texture;

uniform float Kd;
uniform float Ks;
uniform int n;
uniform vec3 ambient_color;
uniform vec3 light_direction;
uniform vec3 light_color;
uniform vec3 camera_direction;

void main()
{ 
    vec3 diffuse_color = texture(diffuse_texture, uv).rgb;
    vec3 frag_n = normalize(frag_normal);
    vec3 light_n = normalize(-light_direction);
    vec3 view_n = normalize(camera_direction); 
		
    // diffuse
    float NL = max(dot(frag_n, light_n),0); 
    vec3 diffuse = Kd * diffuse_color * light_color * NL;

    // specular
    vec3 reflection = reflect(light_n, frag_n);
    float VR = max(dot(view_n, reflection),0);
    vec3 specular = Ks * light_color * pow(VR,n);
	
    // ambient
    vec3 ambient = ambient_color * diffuse_color;
	
    color.rgb  = ambient + diffuse + specular; 
    color.a = 1.0;
}
