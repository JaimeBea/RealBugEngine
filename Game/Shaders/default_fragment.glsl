#version 330

uniform sampler2D diffuse_texture;

in vec2 uv0;

out vec3 color;

void main()
{
	color = texture(diffuse_texture, uv0).xyz;
}
