#ifdef VERTEX

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV0;


uniform mat4 proj;
uniform mat4 view;
uniform mat4 model;



out vec2 uv0;


void main()
{
	
	gl_Position = proj*view*model*vec4(vertexPosition.xyz , 1.0);
	uv0 = vertexUV0 ;

}

#endif

#ifdef FRAGMENT

in vec2 uv0;

uniform sampler2D diffuse;

uniform float currentFrame; 
float X;
float Y;
float u;
float v;
out vec4 outColor;


void main()
{	
	X = trunc(mod(currentFrame,6));
	Y = trunc(currentFrame/6);
	
	X = mix(X,X+1, uv0.x);
	Y = mix(Y,Y+1, uv0.y);
	u = X/6;
	v = Y/6;

	outColor = texture2D(diffuse,  vec2(u, v) ) ;
}

#endif 