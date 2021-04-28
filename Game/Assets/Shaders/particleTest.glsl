#ifdef VERTEX

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoords;

out vec2 TexCoords;

uniform mat4 proj;
uniform mat4 view;

void main()
{
    TexCoords = aTexCoords;
    gl_Position = proj * view  * vec4(aPos, 1.0f); 
}
#endif

#ifdef FRAGMENT
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D diffuse;

void main()
{
    FragColor = texture2D(diffuse, TexCoords);
}
#endif 