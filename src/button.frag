#version 330 core

in vec2 uv;

out vec3 color;

uniform sampler2D image;
uniform vec2      mouse;
uniform uint      activeSide;
uniform uint      isTwoSided;

bool onHover   = mouse.x != -1.0;
bool onTopLeft = uv.x + uv.y < 1.0;
vec3 tColor    = texture(image, uv).xyz;

void main()
{
    color = onHover
         || activeSide == 1u && onTopLeft
         || activeSide == 2u && !onTopLeft
          ? isTwoSided == 1u
            ? onTopLeft && mouse.x + mouse.y < 1.0
              ? tColor + 0.2
              : mouse.x + mouse.y > 1.0
                ? tColor + 0.2
                : tColor
            : tColor + 0.2
          : tColor;
}
