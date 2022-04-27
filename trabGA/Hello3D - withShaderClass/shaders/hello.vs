#version 450 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;


uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float channelID;

out vec4 finalColor;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f); 
    if(channelID == 0.0){
    finalColor = vec4(0.5, 0.5, 1.0, 1.0);
    }

    if(channelID == 1.0){
    finalColor = vec4(0.5, 1.0, 0.5, 1.0);
    }
}
