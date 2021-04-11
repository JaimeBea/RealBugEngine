#version 460

in vec2 uv0;

uniform sampler2D diffuse;
uniform vec4 inputColor;
uniform vec4 tintColor;

out vec4 outColor;

void main()
{
	outColor = texture2D(diffuse, uv0) * inputColor * tintColor;
}