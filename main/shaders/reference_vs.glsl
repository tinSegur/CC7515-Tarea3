#version 330

layout (location = 0) in vec3 Pos0;

void main()
{
    gl_Position = vec4(Pos0, 1.0);
}