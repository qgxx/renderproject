#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Tangent;
in vec3 BiTangent;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_normal1;

void main() {    
    FragColor = texture(texture_diffuse1, TexCoords);
}