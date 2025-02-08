#version 330 core
layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;
out vec4 color;
out vec2 pos;
void main(){
    gl_Position = vec4(aPos.x, aPos.y, 0.f, 1.0f);
    pos = aPos;
    color = vec4(aColor, 1.f);
}