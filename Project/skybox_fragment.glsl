#version 460

in vec3 texcoords;

uniform samplerCube cube;

out vec4 outColor;

void main() 
{
   outColor = texture(cube, texcoords);
}