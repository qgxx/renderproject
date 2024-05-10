#version 420 core
layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 norm;
layout(location = 2) in vec2 tex;
layout(location = 5) in ivec4 boneIds;
layout(location = 6) in vec4 weights;
uniform mat4 pvm;
// const int MAX_BONES = 100;
uniform int MAX_BONES = 200;
const int MAX_BONE_INFLUENCE = 4;
// uniform mat4 finalBonesMatrices[MAX_BONES];
uniform sampler2D boneMatrixImage;
out vec2 TexCoords;
mat4 getBoneMatrix(int row)
{
    return mat4(
        texelFetch(boneMatrixImage, ivec2(0, row), 0),
        texelFetch(boneMatrixImage, ivec2(1, row), 0),
        texelFetch(boneMatrixImage, ivec2(2, row), 0),
        texelFetch(boneMatrixImage, ivec2(3, row), 0));
}
void main()
{
    vec4 totalPosition = vec4(.0f);
    for(int i = 0; i< MAX_BONE_INFLUENCE; ++i)
    {
        if(boneIds[i] == -1)
            continue;
        // if(boneIds[i] >= MAX_BONES)
        // {
        //     totalPosition = vec4(pos, 1.0f);
        //     break;
        // }
        mat4 boneMatrix = getBoneMatrix(boneIds[i]);
        vec4 localPosition = boneMatrix * vec4(pos,1.0f);
        // vec4 localPosition = finalBonesMatrices[boneIds[i]] * vec4(pos, 1.0f);
        totalPosition += localPosition * weights[i];
        // vec3 localNormal = mat3(finalBonesMatrices[boneIds[i]]) * norm;
    }
    gl_Position = pvm * totalPosition;
    TexCoords = tex;
}
