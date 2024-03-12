#version 330
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 aNormal;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

uniform float gMinHeight;
uniform float gMaxHeight;

out vec4 Color;
out vec2 Tex;
out vec3 Pos;
out vec3 Normal;

void main() {
    gl_Position = projection * view * model * vec4(Position, 1.0);
    Pos = mat3(model) * Position;
    Tex = TexCoords;
    float DeltaHeight = gMaxHeight - gMinHeight;
    float HeightRatio = (Position.y - gMinHeight) / DeltaHeight;
    float c = HeightRatio * 0.8 + 0.2;
    Color = vec4(c, c, c, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    Normal =  normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));;
}
