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

// TODO: Clean up these messy calculations

vec2 mouseOffset = vec2
    ( floor(mouse.x * TEXTURE_SIZE.x) / TEXTURE_SIZE.x
    , floor(mouse.y * TEXTURE_SIZE.y) / TEXTURE_SIZE.y
    );

vec2 plotStartPixel = vec2
    ( floor(plotStart.x * TEXTURE_SIZE.x) / TEXTURE_SIZE.x
    , floor(plotStart.y * TEXTURE_SIZE.y) / TEXTURE_SIZE.y
    );

vec2 plotStartPixelBorder = vec2
    ( mouseOffset.x > plotStartPixel.x ? 0u : PIXEL_SIZE.x
    , mouseOffset.y > plotStartPixel.y ? 0u : PIXEL_SIZE.y
    );

vec2 uvPixel = vec2
    ( floor(uv.x * TEXTURE_SIZE.x) / TEXTURE_SIZE.x + PIXEL_SIZE.x / 2.0
    , floor(uv.y * TEXTURE_SIZE.y) / TEXTURE_SIZE.y + PIXEL_SIZE.y / 2.0
    );

vec2 mousePixel = vec2
    ( floor(mouse.x * TEXTURE_SIZE.x) / TEXTURE_SIZE.x + PIXEL_SIZE.x / 2.0
    , floor(mouse.y * TEXTURE_SIZE.y) / TEXTURE_SIZE.y + PIXEL_SIZE.y / 2.0
    );

bool xEven = uint(mod(uv.x * TEXTURE_SIZE.x, 2u)) == 0u;
bool yEven = uint(mod(uv.y * TEXTURE_SIZE.y, 2u)) == 0u;

bool onCross = ( mod(mousePixel.x, 2u) == 0u && mod(mousePixel.y, 2u) == 1u // TODO: Review this
                 ? (  xEven && !yEven
                  || !xEven &&  yEven
                   )
                 : ( !xEven && !yEven
                  ||  xEven &&  yEven
                   )
               )
            && ( uv.x >= mouseOffset.x && uv.x < mouseOffset.x + PIXEL_SIZE.x
              || uv.y >= mouseOffset.y && uv.y < mouseOffset.y + PIXEL_SIZE.y
               )
            && uvPixel != mousePixel;

bool onHover = uv.x >= mouseOffset.x && uv.x < mouseOffset.x + PIXEL_SIZE.x
            && uv.y >= mouseOffset.y && uv.y < mouseOffset.y + PIXEL_SIZE.y;

bool onGrid = uint(mod(uv.x * TEXTURE_SIZE.x * 4u, 8u * 4u)) == 0u
           || uint(mod(uv.y * TEXTURE_SIZE.y * 4u, 8u * 4u)) == 8u * 4u - 1u;

bool onRectangleSurface =
    ( ( mouseOffset.x >= plotStartPixel.x
     && uv.x <= mouseOffset.x + PIXEL_SIZE.x
     && uv.x >= plotStartPixel.x + plotStartPixelBorder.x
      )
     || 
      ( mouseOffset.x <= plotStartPixel.x
     && uv.x <= plotStartPixel.x + plotStartPixelBorder.x
     && uv.x >= mouseOffset.x
      )
    )
   &&
    ( ( mouseOffset.y >= plotStartPixel.y
     && uv.y <= mouseOffset.y + PIXEL_SIZE.y
        && uv.y >= plotStartPixel.y + plotStartPixelBorder.y
      )
     ||
      ( mouseOffset.y <= plotStartPixel.y
        && uv.y <= plotStartPixel.y + plotStartPixelBorder.y
     && uv.y >= mouseOffset.y
      )
    );

// TODO: Restrict to grid
bool uvInX = (uv.x > plotStartPixel.x && uvPixel.x < mousePixel.x) || (uvPixel.x > mousePixel.x && uvPixel.x < plotStartPixel.x);
bool uvInY = (uv.y > plotStartPixel.y && uvPixel.y < mousePixel.y) || (uvPixel.y > mousePixel.y && uvPixel.y < plotStartPixel.y);

bool onRectangleFrame =
     ( plotStartPixel.x < mousePixel.x
       ? uvPixel.x > plotStartPixel.x && uvPixel.x < mousePixel.x && uvInY
       : uvPixel.x < plotStartPixel.x && uvPixel.x > mousePixel.x && uvInY
    && plotStartPixel.y < mousePixel.y
       ? uvPixel.y > plotStartPixel.y && uvPixel.y < mousePixel.y && uvInX
       : uvPixel.y < plotStartPixel.y && uvPixel.y > mousePixel.y && uvInX
     )
    && ( ( abs(uv.x - plotStartPixel.x)  < PIXEL_SIZE.x
        || abs(uv.x - mousePixel.x) < PIXEL_SIZE.x
         )
        ||
         ( abs(uv.y - plotStartPixel.y)  < PIXEL_SIZE.y
        || abs(uv.y - mousePixel.y) < PIXEL_SIZE.y
         )
       );

// Ellipse logic
vec2 plotDimensions = vec2(
    abs(mousePixel.x - plotStartPixel.x),
    abs(mousePixel.y - plotStartPixel.y)
    );

vec2 plotCenter = plotStartPixel + (mousePixel - plotStartPixel) / 2.0;

bool isXMajorAxis = abs(mousePixel.x - plotStartPixel.x) > abs(mousePixel.y - plotStartPixel.y);

// c^2 = a^2 - b^2 where c is the x dist to the focus,
// b is the y dist to the top vertex and a is the dist from the center to the leftmost vertex
float q = isXMajorAxis
    ? mousePixel.x - plotCenter.x
    : mousePixel.y - plotCenter.y;

float w = isXMajorAxis
    ? mousePixel.y - plotCenter.y
    : mousePixel.x - plotCenter.x;

float c = sqrt(pow(q, 2) - pow(w, 2));

vec2 f1 = isXMajorAxis
    ? vec2(plotCenter.x + c, plotCenter.y)
    : vec2(plotCenter.x, plotCenter.y + c);

// TODO: Probably incorrect. Please check and fix
vec2 f2 = isXMajorAxis
    ? vec2(plotCenter.x - c, plotCenter.y)
    : vec2(plotCenter.x, plotCenter.y - c);

float size = isXMajorAxis
    ? abs(mousePixel.x - plotStartPixel.x)
    : abs(mousePixel.y - plotStartPixel.y);

float distX = distance(uvPixel, f1) + distance(uvPixel, f2);
bool onEllipse = distX <= size; // && distX > size - 

// Code for the line tool
// vec2 a = uv - plotStart;
vec2 a = uvPixel - plotStartPixel;
vec2 b = mousePixel - plotStartPixel;

float d = dot(a, b);

vec2 p = d / pow(length(b), 2) * b;

float dist = distance(a, p);

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
    
    if(onCross)
    {
        color = colors[attributeValue] + vec3(0.1, 0.1, 0.0);
    }
    else if(onGrid)
    {
        color = colors[attributeValue] * 0.8;
    }
    else if(onHover)
    {
        color = colors[activeColorIndex];
    }
    
    if(tool == 0u)
    {
        
    }
    else if(tool == 1u)
    {
        if( plotting
        && d > 0
        && (plotStartPixel.x <= mousePixel.x
                ? (uvPixel.x >= plotStartPixel.x && uvPixel.x < mousePixel.x)
                : (uvPixel.x <= plotStartPixel.x && uvPixel.x > mousePixel.x)
           )
        && (plotStartPixel.y <= mousePixel.y
                ? (uvPixel.y >= plotStartPixel.y && uvPixel.y < mousePixel.y)
                : (uvPixel.y <= plotStartPixel.y && uvPixel.y > mousePixel.y)
           )
        && dist <= PIXEL_SIZE.x * 0.75
        )
        {
            color = colors[activeColorIndex];
        }
    }
    else if(tool == 2u && plotting && onRectangleFrame)
    {
        color = colors[activeColorIndex];
    }
    else if(tool == 3u && plotting && onRectangleSurface)
    {
        color = colors[activeColorIndex];
    }
    else if(/*tool == 5u &&*/ onEllipse)
    {
        color = vec3(1.0, 0.0, 0.0); //colors[attributeValue];
    }
}
