#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in vec3 aNormal;
layout (location = 3) in ivec4 boneIds; 
layout (location = 4) in vec4 weights;

uniform mat4 finalBonesMatrices[100];
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 lightSpaceMatrix;
uniform bool isAnimated;

out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;
out vec4 FragPosLightSpace;

void main() {
    vec4 localPos;
    vec3 localNormal;

    if(isAnimated) {
        // 针对科比：进行骨骼变换
        mat4 boneTransform = mat4(0.0);
        float totalWeight = 0.0;
        
        for(int i = 0; i < 4; i++) {
            if(boneIds[i] != -1 && boneIds[i] < 100) {
                boneTransform += finalBonesMatrices[boneIds[i]] * weights[i];
                totalWeight += weights[i];
            }
        }

        // 解决粘连：如果权重失效，回退到原始位置
        if(totalWeight < 0.01) {
            localPos = vec4(aPos, 1.0);
            localNormal = aNormal;
        } else {
            localPos = boneTransform * vec4(aPos, 1.0);
            localNormal = mat3(boneTransform) * aNormal;
        }
    } else {
        // 针对箱子和地板：直接使用原始坐标
        localPos = vec4(aPos, 1.0);
        localNormal = aNormal;
    }

    // 计算世界空间坐标
    vec4 worldPos = model * localPos;
    FragPos = worldPos.xyz;
    Normal = normalize(mat3(transpose(inverse(model))) * localNormal);
    TexCoord = aTexCoord;
    
    // 计算阴影贴图空间坐标
    FragPosLightSpace = lightSpaceMatrix * vec4(FragPos, 1.0);
    
    // 最终裁剪空间坐标
    gl_Position = projection * view * worldPos;
}