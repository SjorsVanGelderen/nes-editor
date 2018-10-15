#version 330 core

in vec2 uv;

out vec3 color;

uniform sampler2D paletteTexture;
uniform vec2      mouse;

vec3 tColor = texture(paletteTexture, vec2(uv.x, 1.0 - uv.y)).xyz;

void main()
{
    color = tColor;
}
