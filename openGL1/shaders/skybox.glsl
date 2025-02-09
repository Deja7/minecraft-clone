#version 450 core
out vec4 FragColor; 
in vec4 color;
in vec2 pos;
uniform vec3 sun;
uniform vec3 front;
uniform mat4 rot;
void main(){
    vec3 right = normalize(cross(front, vec3(0.f, 1.f, 0.f)));
    vec3 rtop = -normalize(cross(front, right));
    vec2 plane = vec2(pos.x * tan(radians(40.f)), pos.y * tan(radians(40.f)) * 4.f / 3.f);
    vec3 F = normalize(front + plane.y * rtop + plane.x * right);
    float sunK = (dot(-sun, F) + 1.f) / 2.f;
    sunK = pow(sunK, 0.2f);
    vec4 sunColor = mix(vec4(1.f, 1.f, 0.f, 1.f), vec4(0.f, 0.f, 0.f, 0.f), sunK);
    FragColor = color + sunColor;
}