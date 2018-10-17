#version 330 core

in vec2 uv;

out vec3 color;

const uint TILES_SIZE   = 256u;
const uint PALETTE_SIZE = 64u;

uniform sampler2D paletteTexture;
uniform vec2      mouse;
uniform uint      tiles[TILES_SIZE];

vec3 tColor = texture(paletteTexture, vec2(1.0)).xyz;

uint y = uint(uv.y * sqrt(TILES_SIZE));
uint x = uint(uv.x * sqrt(TILES_SIZE));
uint i = uint(y * sqrt(TILES_SIZE) + x);
vec3 c = vec3(tiles[i] / float(TILES_SIZE));

void main()
{
    color = tColor;
    color = c;
}
