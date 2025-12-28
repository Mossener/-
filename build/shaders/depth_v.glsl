#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 3) in ivec4 boneIds; 
layout (location = 4) in vec4 weights;

uniform mat4 finalBonesMatrices[100];
uniform mat4 model;
uniform mat4 lightSpaceMatrix;
uniform bool isAnimated; // 只有玩家渲染时设为 true

void main() {
    vec4 totalPosition = vec4(0.0);
    if(isAnimated) {
        for(int i = 0 ; i < 4 ; i++) {
            if(boneIds[i] == -1) continue;
            totalPosition += (finalBonesMatrices[boneIds[i]] * vec4(aPos, 1.0)) * weights[i];
        }
    } else {
        totalPosition = vec4(aPos, 1.0);
    }
    gl_Position = lightSpaceMatrix * model * totalPosition;
}