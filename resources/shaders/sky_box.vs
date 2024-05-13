#version 460
layout (location = 0) in vec3 pos;

out vec3 texCoord;

uniform mat4 camera;

void main()
{
    // flip the z due to the different coordinate systems on images
    texCoord = vec3(pos.x, pos.y, -pos.z);

    vec4 mdl_pos = camera * vec4(pos, 1.0f);
    // Having z equal w will always result in a depth of 1
    gl_Position = vec4(mdl_pos.x, mdl_pos.y, mdl_pos.w, mdl_pos.w);
}    