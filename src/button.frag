#version 330 core

in vec2 uv;

out vec3 color;

uniform sampler2D image;
uniform vec2      mouse;

bool onHover = mouse.x > 0.0 && mouse.x < 1.0 && mouse.y > 0.0 && mouse.y < 1.0;
vec3 tColor  = texture(image, uv).xyz;
    
void main()
{
    color = onHover ? tColor + 0.2 : tColor;
}
