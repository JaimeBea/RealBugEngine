#version 460

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;

uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;

out vec2 uv0;

void main()
{
	gl_Position = proj*view*model*vec4(vertex_position.xy, 0.0, 1.0);
	uv0 = vertex_uv0;

	//gl_Position = proj*view*model*vec4(vertex_position.xy, 0.0, 1.0);
	//uv0 = vec2((vertex_position.x+1.0)/2.0, 1 - (vertex_position.y+1.0)/2.0);
}
