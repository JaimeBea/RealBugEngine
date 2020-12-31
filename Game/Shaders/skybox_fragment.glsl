#version 460

in vec3 texcoords;

uniform samplerCube cubemap;

out vec4 out_color;

void main() 
{
	out_color = texture(cubemap, texcoords);
}