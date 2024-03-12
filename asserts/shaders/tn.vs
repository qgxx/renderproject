#version 330
layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexCoords;
layout (location = 2) in vec3 aNormal;

uniform mat4 view;
uniform mat4 model;

out VS_OUT {
    vec3 normal;
} vs_out;

void main() {
    gl_Position = view * model * vec4(Position, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
}
