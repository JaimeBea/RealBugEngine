#ifdef VERTEX
uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

layout ( location = 0 ) in vec3 vertexPosition;
layout ( location = 1 ) in vec3 vertexNormals;
layout ( location = 2 ) in vec2 vertexUv0;

out vec2 uv0;

void main() {
	gl_Position = proj * view * model * vec4(vertexPosition, 1.0);
	uv0 = vertexUv0;
}
#endif

#ifdef FRAGMENT


uniform sampler2D diffuseTexture;

in vec2 uv0;

out vec3 color;

void main() {
	color = texture(diffuseTexture, uv0).rgb;
}
#endif