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
uniform uint      tool;
uniform vec2      plotStart;
uniform bool      plotting;
uniform sampler2D paletteTexture;
uniform sampler2D characterTexture;

vec2 mouseOffset =
    vec2(
        floor(mouse.x * TEXTURE_SIZE.x) / TEXTURE_SIZE.x,
        floor(mouse.y * TEXTURE_SIZE.y) / TEXTURE_SIZE.y
        );

bool onHover = uv.x >= mouseOffset.x && uv.x < mouseOffset.x + PIXEL_SIZE.x
            && uv.y >= mouseOffset.y && uv.y < mouseOffset.y + PIXEL_SIZE.y;

vec2 distanceToGrid = vec2(mod(uv.x * 128.0, 4.0), mod(uv.y * 64.0, 4.0));

bool onGrid = abs(distanceToGrid.x) < 0.025 || abs(distanceToGrid.y) < 0.025;

vec2 plotCenter = plotStart + (mouse - plotStart) / 2;

// Should draw only on pixels, too free now
bool onRectangleFrame =
       abs(uv.x - plotCenter.x) < abs(mouse.x - plotCenter.x)
    && abs(uv.y - plotCenter.y) < abs(mouse.y - plotCenter.y)
    && ( abs(uv.x - plotStart.x) < PIXEL_SIZE.x
      || abs(uv.x - mouse.x)     < PIXEL_SIZE.x
      || abs(uv.y - plotStart.y) < PIXEL_SIZE.y
      || abs(uv.y - mouse.y)     < PIXEL_SIZE.y
       );

float rad = length(mouse.x - plotCenter.x) / 2.0;

bool onCircleFrame =
       abs(uv.x - plotCenter.x) / 2.0 < rad
    && abs(uv.y - plotCenter.y)       < rad;

    // length(uv - plotCenter) < length(mouse - plotCenter);

uint sampleOffset    = activeSample > 3u ? 13u : 0u;
uint sampleIndex     = sampleOffset + uint(mod(activeSample, 4u)) * 3u;
uint backgroundIndex = activeSample > 3u ? 25u : 12u;

void main()
{
    vec3 colors[4];
    
    colors[0] =
        texture(paletteTexture,
                vec2(
                    mod(samples[backgroundIndex], PALETTE_SIZE.x)    / PALETTE_SIZE.x,
                    floor(samples[backgroundIndex] / PALETTE_SIZE.x) / PALETTE_SIZE.y
                    )
            ).xyz;
    
    colors[1] =
        texture(paletteTexture,
                vec2(
                    mod(samples[sampleIndex], PALETTE_SIZE.x)    / PALETTE_SIZE.x,
                    floor(samples[sampleIndex] / PALETTE_SIZE.x) / PALETTE_SIZE.y
                    )
            ).xyz;
    
    colors[2] =
        texture(paletteTexture,
                vec2(
                    mod(samples[sampleIndex + 1u], PALETTE_SIZE.x)    / PALETTE_SIZE.x,
                    floor(samples[sampleIndex + 1u] / PALETTE_SIZE.x) / PALETTE_SIZE.y
                    )
            ).xyz;
    
    colors[3] =
        texture(paletteTexture,
                vec2(
                    mod(samples[sampleIndex + 2u], PALETTE_SIZE.x)    / PALETTE_SIZE.x,
                    floor(samples[sampleIndex + 2u] / PALETTE_SIZE.x) / PALETTE_SIZE.y
                    )
            ).xyz;

    // Translate character tone to color index
    uint attributeValue = uint(texture(characterTexture, vec2(uv.x, 1.0 - uv.y)) * 3.0);
    
    color =
        tool == 0u && onHover
          ? vec3(1.0, 1.0, 0.0) //colors[activeColor]
        : tool == 2u // RectangleFrame
       && plotting
        && onCircleFrame // onRectangleFrame
          ? vec3(1.0, 1.0, 0.0)
        : tool == 3u // RectangleFill
       && plotting
       && uv.x >= plotStart.x
       && uv.x <  mouse.x
       && uv.y >= plotStart.y
       && uv.y <  mouse.y
          ? vec3(1.0, 1.0, 0.0)
        : onGrid
          ? vec3(1.0, 0.0, 1.0)
        : colors[attributeValue];

    // color = texture(paletteTexture,
    //         vec2(
    //             mod(samples[sampleIndex], PALETTE_SIZE.x)    / PALETTE_SIZE.x,
    //             floor(samples[sampleIndex] / PALETTE_SIZE.x) / PALETTE_SIZE.y
    //             )
    //     ).xyz;
}
