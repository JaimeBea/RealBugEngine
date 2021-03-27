#ifdef VERTEX

layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 vertex_uv0;

uniform mat4 proj;
uniform mat4 model;

out vec2 uv0;

void main()
{
	gl_Position = proj*model*vec4(vertex_position.xy, 0.0 , 1.0);
	uv0 = vertex_uv0;
}


#endif

#ifdef FRAGMENT

in vec2 uv0;

uniform sampler2D diffuse;

out vec4 outColor;

void main()
{
	outColor = texture2D(diffuse, uv0);
}

#endif 