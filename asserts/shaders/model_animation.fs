#version 420 core
in vec2 TexCoords;
out vec4 color;
uniform sampler2D image;
// uniform vec4 colorOnly = vec4(.0,.0,.0,-1.0);
void main()
{
    // color = (colorOnly.a != -1) ? colorOnly : (texture(image, TexCoords));
    color = texture(image, TexCoords);
} 