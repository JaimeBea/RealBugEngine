#version 460

in layout(location=0) vec3 pos;
in layout(location=1) vec3 normal;
in layout(location=2) vec2 uvs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 frag_normal;
out vec2 uv;

void main()
{
	gl_Position = proj * view * model * vec4(pos, 1.0);
	frag_normal = transpose(inverse(mat3(model))) * normal;
	uv = uvs;
}
