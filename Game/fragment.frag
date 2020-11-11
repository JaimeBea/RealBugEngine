#version 460

uniform sampler2D mytexture;

in vec2 uv0;

out vec4 color;

void main()
{
	color = texture2D(mytexture, uv0);
}
