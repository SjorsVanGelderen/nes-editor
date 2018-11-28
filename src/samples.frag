#version 330 core

in vec2 uv;

out vec3 color;

const uvec2 PALETTE_SIZE  = uvec2(16u, 4u);
const uint  SAMPLES_SIZE  = 26u;
const uint  SAMPLE_GROUPS = 8u;
const vec2  PART          = vec2(1.0 / (SAMPLES_SIZE / 2.0), 0.5);
const vec2  B_SIZE        = vec2(PART.x * 0.15, 0.15 / 2.0);
const float B_FACTOR      = 0.25;
const vec3  HOVER_B_COLOR = vec3(1.0, 1.0, 0.0);
const vec3  AS_B_COLOR    = vec3(1.0, 0.0, 1.0);
const vec3  AC_B_COLOR    = vec3(0.0, 1.0, 1.0);

uniform uint      samples[SAMPLES_SIZE];
uniform sampler2D paletteTexture;
uniform vec2      mouse;
uniform uint      activeSample;
uniform uint      activeColor;

float left   = floor(mouse.x / PART.x) * PART.x;
float right  = left + PART.x;
float top    = mouse.y > 0.5 ? 0.5 : 0.0;
float bottom = top + 0.5;

float asLeft   = mod(activeSample, SAMPLE_GROUPS / 2.0) * PART.x * 3;
float asRight  = asLeft + PART.x * 3;
float asTop    = activeSample < SAMPLE_GROUPS / 2.0 ? 1.0 : 0.5;
float asBottom = asTop - 0.5;

float acLeft   = mod(activeColor, SAMPLES_SIZE / 2.0) * PART.x;
float acRight  = acLeft + PART.x;
float acTop    = activeColor < SAMPLES_SIZE / 2.0 ? 1.0 : 0.5;
float acBottom = acTop - 0.5;

bool onHover = uv.x >= left
            && uv.x <= right
            && uv.y >= top
            && uv.y <= bottom;

bool onBorder = abs(left   - uv.x) < B_SIZE.x * B_FACTOR
             || abs(right  - uv.x) < B_SIZE.x * B_FACTOR
             || abs(top    - uv.y) < B_SIZE.y * B_FACTOR
             || abs(bottom - uv.y) < B_SIZE.y * B_FACTOR;

bool onAsBorder =
       uv.y <= asTop && uv.y >= asBottom
       && ( abs(asLeft  - uv.x) < B_SIZE.x * B_FACTOR
         || abs(asRight - uv.x) < B_SIZE.x * B_FACTOR
          )
    || uv.x >= asLeft && uv.x < asRight
       && ( abs(asTop    - uv.y) < B_SIZE.y * B_FACTOR
         || abs(asBottom - uv.y) < B_SIZE.y * B_FACTOR
          );

bool onAcBorder =
       uv.y <= acTop && uv.y >= acBottom
       && ( abs(acLeft  - uv.x) < B_SIZE.x * B_FACTOR
         || abs(acRight - uv.x) < B_SIZE.x * B_FACTOR
          )
    || uv.x >= acLeft && uv.x < acRight
       && ( abs(acTop    - uv.y) < B_SIZE.y * B_FACTOR
         || abs(acBottom - uv.y) < B_SIZE.y * B_FACTOR
          );

uint offset       = uint(uv.y <= 0.5 ? SAMPLES_SIZE / 2.0 : 0);
uint sampleIndex  = uint(offset + uv.x * float(SAMPLES_SIZE / 2.0));
uint paletteIndex = samples[sampleIndex];

vec3 tColor =
    texture(paletteTexture,
            vec2(
                mod(paletteIndex, PALETTE_SIZE.x)    / PALETTE_SIZE.x,
                floor(paletteIndex / PALETTE_SIZE.x) / PALETTE_SIZE.y
                )
        ).xyz;

void main()
{
    color = onHover && onBorder ? HOVER_B_COLOR
          : onAcBorder          ? AC_B_COLOR
          : onAsBorder          ? AS_B_COLOR
          : tColor;
}
