--- vertVarCommon

in layout(location=0) vec3 pos;
in layout(location=1) vec3 norm;
in layout(location=2) vec2 uvs;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 fragNormal;
out vec3 fragPos;
out vec2 uv;

--- vertVarSkinning

#define MAX_BONES 100

in layout(location=3) uvec4 boneIndices;
in layout(location=4) vec4 boneWeitghts;

uniform mat4 palette[MAX_BONES];
uniform bool hasBones;

--- vertMainCommon

void main()
{
    gl_Position = proj * view * model * vec4(pos, 1.0);
    fragNormal = normalize(transpose(inverse(mat3(model))) * norm.xyz);
    fragPos = vec3(model * vec4(pos, 1.0));
    uv = uvs;
}

--- vertMainSkinning

void main()
{
    vec4 position;
    vec4 normal;
    mat4 skinT = palette[boneIndices[0]] * boneWeitghts[0] + palette[boneIndices[1]] * boneWeitghts[1]
        + palette[boneIndices[2]] * boneWeitghts[2] + palette[boneIndices[3]] * boneWeitghts[3];

    position = skinT * vec4(pos, 1.0);
    normal = skinT * vec4(norm, 0.0);

    gl_Position = proj * view * model * position;
    fragNormal = normalize(transpose(inverse(mat3(model))) * normal.xyz);
    fragPos = vec3(model * position);
    uv = uvs;
}