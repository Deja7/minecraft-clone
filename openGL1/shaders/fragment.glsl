#version 450 core
out vec4 FragColor;  
in vec2 texCord;
in vec3 normal;
in vec3 worldPos;
flat in int texID;
uniform sampler2DArray textures;
uniform vec3 sunPos;

void main(){
    vec3 toSun = normalize(1000 * sunPos - worldPos);
    float k = dot(normal, toSun);
    vec4 preColor = (0.6 + 0.4 * k) * texture(textures, vec3(texCord, texID));
    FragColor = vec4(pow(preColor.r, 1.0), pow(preColor.g, 1.0), pow(preColor.b, 1.0), 1.0);
}