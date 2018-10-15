#version 330 core

in vec2 uv;

out vec3 color;

const vec2  SIZE     = vec2(16.0, 4.0);
const vec2  PART     = vec2(1.0 / SIZE.x, 1.0 / SIZE.y);
const float B_FACTOR = 0.25;
const vec2  B_SIZE   = vec2(PART.x * B_FACTOR, PART.y * B_FACTOR);
const vec3  B_COLOR  = vec3(1.0, 1.0, 0.0);

uniform sampler2D paletteTexture;
uniform vec2      mouse;

float left   = floor(mouse.x / PART.x) * PART.x;
float right  = left + PART.x;
float bottom = ceil(mouse.y / PART.y) * PART.y;
float top    = bottom - PART.y;

bool onHover = uv.x >= left   - PART.x * B_FACTOR
            && uv.x <  right  + PART.x * B_FACTOR
            && uv.y >= top    - PART.y * B_FACTOR
            && uv.y <  bottom + PART.y * B_FACTOR;

// bool onBorder = onHover
//     && ( abs(left + PART.x / 2 - uv.x) > (PART.x / 2) - B_SIZE.x / 16
//       || abs(top  + PART.y / 2 - uv.y) > (PART.y / 2) - B_SIZE.y / 16
//        );

vec3 tColor = texture(paletteTexture, vec2(uv.x, 1.0 - uv.y)).xyz;

void main()
{
    color = //onBorder ? B_COLOR
            onHover  ? texture(paletteTexture, vec2(mouse.x, 1.0 - mouse.y)).xyz
          : tColor;
}
