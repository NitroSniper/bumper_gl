#version 460
out vec4 color;

in vec3 texCoord;


uniform samplerCube skybox;

void main()
{    
    color = texture(skybox, texCoord);
    // color = vec4(1, 0, 1, 1);
}