#version 460

in vec2 uv0;

uniform sampler2D diffuse;

out vec4 outColor;

void main()
{
 color = texture2D(diffuse, uv0);
}