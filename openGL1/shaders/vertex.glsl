#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCord;
layout (location = 2) in int data;

vec3 getNormal(int dir){
    if(dir == 0) return vec3(0.f, 0.f, -1.f);
    if(dir == 1) return vec3(1.f, 0.f, 0.f);
    if(dir == 2) return vec3(0.f, 0.f, 1.f);
    if(dir == 3) return vec3(-1.f, 0.f, 0.f);
    if(dir == 4) return vec3(0.f, -1.f, 0.f);
    if(dir == 5) return vec3(0.f, 1.f, 0.f);
}

out vec3 worldPos;
out vec2 texCord;
out vec3 normal;
flat out int texID;
uniform vec3 chunkOffset;
uniform mat4 model; 
uniform mat4 view; 
uniform mat4 projection; 
uniform float slod;
void main(){
    int texture = int(data & 511);
    int sY = int((data >> 9) & 15) + 1;
    int sX = int((data >> 13) & 15) + 1;
    int direction = int((data >> 17) & 7);
    int dZ = int((data >> 20) & 15);
    int dY = int((data >> 24) & 15);
    int dX = int((data >> 28) & 15);
    vec3 S = aPos * vec3(float(sX), 0.f, float(sY));
    vec3 R = S;
    if(direction == 0){
       R = vec3(slod - S.x, S.z, 0.f);
    }
    else if(direction == 1){
        R = vec3(slod, S.z, slod - S.x);
    }
    else if(direction == 2){
        R = vec3(S.x, S.z, slod);
    }
    else if(direction == 3){
        R = vec3(0.f, S.z, S.x);
    }
    else if(direction == 5){
        R = vec3(slod - S.x, slod, S.z);
    }

    R = R + vec3(float(dX), float(dY), float(dZ));
    worldPos = R + 16 * chunkOffset;
    gl_Position = projection * view * model * vec4(R + 16 * chunkOffset, 1.0f);

    texCord = aTexCord * vec2(float(sX), float(sY));
    texID = texture;
    normal = getNormal(direction);
}