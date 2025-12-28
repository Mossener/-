#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;      // 模型矩阵：物体在世界中的位置、旋转、缩放
uniform mat4 view;       // 观察矩阵：摄像机的位置和朝向
uniform mat4 projection; // 投影矩阵：定义透视（近大远小）

void main() {
    // 矩阵乘法顺序很重要：先模型，再观察，最后投影
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
