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

uniform float currentFrame;


uniform sampler2D diffuse;


uniform vec4 initColor;
uniform vec4 finalColor;
out vec4 outColor;

void main()
{	
	if(currentFrame<1){
		float a = currentFrame - int(currentFrame);
		outColor = mix(initColor, finalColor, a) * texture2D(diffuse, uv0); 
	
	}else{
		outColor = finalColor * texture2D(diffuse, uv0); 
	}
}

#endif 