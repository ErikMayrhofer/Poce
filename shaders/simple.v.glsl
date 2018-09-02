#version 450

layout(location = 2) uniform mat4 mvp;
in vec2 texcoord;
in vec2 position;

out vec2 Texcoord;
out vec2 Coords;

void main()
{
    gl_Position = mvp * vec4(position, 0.0, 1.0);
    Texcoord = texcoord;
    Coords = position;
}