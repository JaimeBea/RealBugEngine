#version 460

uniform sampler2D diffuseTexture;

in vec2 uv0;

out vec3 color;

void main() {
	color = texture(diffuseTexture, uv0).xyz;
}
