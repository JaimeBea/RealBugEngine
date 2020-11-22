#version 330

uniform sampler2D diffuse;

in vec2 uv0;

layout(location = 0) out vec3 color;

void main()
{
	color = texture(diffuse, uv0).xyz;
}
