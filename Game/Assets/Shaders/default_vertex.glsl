#version 460

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 vertexNormals;
layout(location=2) in vec2 vertexUv0;

out vec2 uv0;

void main() {
	gl_Position = proj * view * model * vec4(vertexPosition, 1.0);
	uv0 = vertexUv0;
}
