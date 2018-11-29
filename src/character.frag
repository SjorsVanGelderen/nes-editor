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

vec2 mouseOffset = vec2
    ( floor(mouse.x * TEXTURE_SIZE.x) / TEXTURE_SIZE.x
    , floor(mouse.y * TEXTURE_SIZE.y) / TEXTURE_SIZE.y
    );

bool xEven = uint(mod(uv.x * TEXTURE_SIZE.x, 2u)) == 0u;
bool yEven = uint(mod(uv.y * TEXTURE_SIZE.y, 2u)) == 0u;

bool onCross = (  xEven && !yEven
              || !xEven &&  yEven
               )
            && ( uv.x >= mouseOffset.x && uv.x < mouseOffset.x + PIXEL_SIZE.x
              || uv.y >= mouseOffset.y && uv.y < mouseOffset.y + PIXEL_SIZE.y
               );

bool onHover = uv.x >= mouseOffset.x && uv.x < mouseOffset.x + PIXEL_SIZE.x
            && uv.y >= mouseOffset.y && uv.y < mouseOffset.y + PIXEL_SIZE.y;

// TODO: Could be improved
bool onGrid = uint(mod(uv.x * TEXTURE_SIZE.x * 4u, 8u * 4u)) == 0u
           || uint(mod(uv.y * TEXTURE_SIZE.y * 4u, 8u * 4u)) == 8u * 4u - 1u;

vec2 plotCenter = plotStart + (mouse - plotStart) / 2;

// TODO: Doesn't yield the correct coordinates yet
vec2 plotStartPixels = vec2
    ( floor(plotStart.x * TEXTURE_SIZE.x) / TEXTURE_SIZE.x
    , floor(plotStart.y * TEXTURE_SIZE.y) / TEXTURE_SIZE.y
    );

bool onRectangleSurface =
    ( ( mouseOffset.x > plotStartPixels.x
     && uv.x <= mouseOffset.x
     && uv.x >= plotStartPixels.x
      )
     || 
      ( mouseOffset.x <= plotStartPixels.x
     && uv.x <= plotStartPixels.x
     && uv.x >= mouseOffset.x
      )
    )
   &&
    ( ( mouseOffset.y > plotStartPixels.y
     && uv.y <= mouseOffset.y
     && uv.y >= plotStartPixels.y
      )
     ||
      ( mouseOffset.y <= plotStartPixels.y
     && uv.y <= plotStartPixels.y
     && uv.y >= mouseOffset.y
      )
    );

// TODO: Adjust so it matches only the frame
bool onRectangleFrame = false;
//     ( ( mouseOffset.x > plotStartPixels.x
//      && uv.x <= mouseOffset.x
//      && uv.x >= plotStartPixels.x
//       )
//      || 
//       ( mouseOffset.x <= plotStartPixels.x
//      && uv.x <= plotStartPixels.x
//      && uv.x >= mouseOffset.x
//       )
//     )
//    &&
//     ( ( mouseOffset.y > plotStartPixels.y
//      && uv.y <= mouseOffset.y
//      && uv.y >= plotStartPixels.y
//       )
//      ||
//       ( mouseOffset.y <= plotStartPixels.y
//      && uv.y <= plotStartPixels.y
//      && uv.y >= mouseOffset.y
//       )
//     );

float rad = length(mouse.x - plotCenter.x) / 2.0;

bool onCircleFrame =
       abs(uv.x - plotCenter.x) / 2.0 < rad
    && abs(uv.y - plotCenter.y)       < rad;

    // length(uv - plotCenter) < length(mouse - plotCenter);

uint sampleOffset    = activeSample > 3u ? 13u : 0u;
uint sampleIndex     = sampleOffset + uint(mod(activeSample, 4u)) * 3u;
uint backgroundIndex = activeSample > 3u ? 25u : 12u;

uint activeColorIndex = activeColor == SAMPLES_SIZE / 2u - 1u 
                     || activeColor == SAMPLES_SIZE - 1u
                        ? 0u
                        : activeColor < SAMPLES_SIZE / 2u
                          ? uint(mod(activeColor, 3u)) + 1u
                          : uint(mod(activeColor, 3u));

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

    color = colors[attributeValue];

    if(onGrid)
    {
        color = colors[attributeValue] * 0.8;
    }
    
    if(tool == 0u)
    {
        if(onHover)
        {
            color = vec3(1.0, 1.0, 0.0);
        }
        else if(onCross)
        {
            color = colors[attributeValue] + vec3(0.1, 0.1, 0.0);
        }
    }
    else if(tool == 1u)
    {
        
    }
    else if(tool == 2u)
    {
        if(plotting && onRectangleSurface)
        {
            if(onRectangleFrame)
            {
                color = vec3(1.0, 0.0, 0.0);
            }
            else
            {
                color = colors[activeColorIndex];
            }
        } 
    }
}
