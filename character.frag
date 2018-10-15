#version 330 core

in vec2 uv;

out vec3 color;

const uvec2 TEXTURE_SIZE = uvec2(256, 128);
const uint  SAMPLES_SIZE = 26u;
const uvec2 PALETTE_SIZE = uvec2(16u, 4u);
const vec2  PIXEL_SIZE   = vec2(1.0 / TEXTURE_SIZE.x, 1.0 / TEXTURE_SIZE.y);

uniform vec2      mouse;
uniform uint      samples[SAMPLES_SIZE];
uniform uint      activeSample;
uniform uint      activeColor;
uniform sampler2D paletteTexture;
uniform sampler2D characterTexture;

uint sampleOffset = activeSample * 3u;

vec2 mouseOffset =
    vec2(
        floor(mouse.x * TEXTURE_SIZE.x) / TEXTURE_SIZE.x,
        floor(mouse.y * TEXTURE_SIZE.y) / TEXTURE_SIZE.y
        );

bool onHover = uv.x >= mouseOffset.x && uv.x < mouseOffset.x + PIXEL_SIZE.x
            && uv.y >= mouseOffset.y && uv.y < mouseOffset.y + PIXEL_SIZE.y;

void main()
{
    vec3 colors[4];
    
    colors[0] =
        texture(paletteTexture,
                vec2(
                    mod(samples[SAMPLES_SIZE - 1u], PALETTE_SIZE.x)    / PALETTE_SIZE.x,
                    floor(samples[SAMPLES_SIZE - 1u] / PALETTE_SIZE.x) / PALETTE_SIZE.y
                    )
            ).xyz;
    
    colors[1] =
        texture(paletteTexture,
                vec2(
                    mod(samples[sampleOffset], PALETTE_SIZE.x)    / PALETTE_SIZE.x,
                    floor(samples[sampleOffset] / PALETTE_SIZE.x) / PALETTE_SIZE.y
                    )
            ).xyz;
    
    colors[2] =
        texture(paletteTexture,
                vec2(
                    mod(samples[sampleOffset + 1u], PALETTE_SIZE.x)    / PALETTE_SIZE.x,
                    floor(samples[sampleOffset + 1u] / PALETTE_SIZE.x) / PALETTE_SIZE.y
                    )
            ).xyz;
    
    colors[3] =
        texture(paletteTexture,
                vec2(
                    mod(samples[sampleOffset + 2u], PALETTE_SIZE.x)    / PALETTE_SIZE.x,
                    floor(samples[sampleOffset + 2u] / PALETTE_SIZE.x) / PALETTE_SIZE.y
                    )
            ).xyz;
    
    uint attributeValue = uint(texture(characterTexture, vec2(uv.x, 1.0 - uv.y)) * 3.0);
    
    color = onHover ? vec3(1.0, 1.0, 0.0) //colors[activeColor]
                    : colors[attributeValue];
}
