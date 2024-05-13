#version 460
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 aTexCoord;

out vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 camera;

void main()
{
    texCoord = aTexCoord;    
    gl_Position = camera * model * vec4(pos, 1.0);
}