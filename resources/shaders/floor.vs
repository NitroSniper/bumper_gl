#version 460
layout (location = 0) in vec3 pos;

uniform mat4 model;
uniform mat4 camera;
uniform float size;

out vec3 mdl_pos;
out vec2 texCoords;



void main()
{
	mdl_pos = vec3(model * vec4(pos, 1));
	texCoords = vec2(mdl_pos.x, mdl_pos.z);
	gl_Position = camera * vec4(mdl_pos*size, 1);
}    