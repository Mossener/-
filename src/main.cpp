#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>

// 引入 GLM 数学库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // 用于变换（平移、旋转、缩放）
#include <glm/gtc/type_ptr.hpp>         // 用于将 GLM 矩阵转换为 OpenGL 可用的格式

// 顶点着色器：现在接收三个矩阵
const char* vShader = R"(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;      // 模型矩阵：物体在世界中的位置、旋转、缩放
uniform mat4 view;       // 观察矩阵：摄像机的位置和朝向
uniform mat4 projection; // 投影矩阵：定义透视（近大远小）

void main() {
    // 矩阵乘法顺序很重要：先模型，再观察，最后投影
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

// 片段着色器：颜色不变，我们后面可以修改它
const char* fShader = R"(
#version 330 core
out vec4 FragColor;
void main() {
    FragColor = vec4(1.0, 0.5, 0.2, 1.0); // 橘黄色
}
)";

// 窗口大小改变时的回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "3D Cube Demo", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "GLAD init failed\n";
        return -1;
    }

    // ====================================================================
    // 新增：启用深度测试！
    // 这样近的物体会遮挡远的物体，否则会看到奇怪的“透视”效果
    glEnable(GL_DEPTH_TEST);   
    // ====================================================================

    // 正方体的 36 个顶点数据 (每个面 2 个三角形，共 6 个面)
    float vertices[] = {
        -0.5f, -0.5f, -0.5f, // 后下左
         0.5f, -0.5f, -0.5f, // 后下右
         0.5f,  0.5f, -0.5f, // 后上右
         0.5f,  0.5f, -0.5f, // 后上右
        -0.5f,  0.5f, -0.5f, // 后上左
        -0.5f, -0.5f, -0.5f, // 后下左

        -0.5f, -0.5f,  0.5f, // 前下左
         0.5f, -0.5f,  0.5f, // 前下右
         0.5f,  0.5f,  0.5f, // 前上右
         0.5f,  0.5f,  0.5f, // 前上右
        -0.5f,  0.5f,  0.5f, // 前上左
        -0.5f, -0.5f,  0.5f, // 前下左

        -0.5f,  0.5f,  0.5f, // 上左前
        -0.5f,  0.5f, -0.5f, // 上左后
         0.5f,  0.5f, -0.5f, // 上右后
         0.5f,  0.5f, -0.5f, // 上右后
         0.5f,  0.5f,  0.5f, // 上右前
        -0.5f,  0.5f,  0.5f, // 上左前

        -0.5f, -0.5f,  0.5f, // 下左前
        -0.5f, -0.5f, -0.5f, // 下左后
         0.5f, -0.5f, -0.5f, // 下右后
         0.5f, -0.5f, -0.5f, // 下右后
         0.5f, -0.5f,  0.5f, // 下右前
        -0.5f, -0.5f,  0.5f, // 下左前

         0.5f, -0.5f, -0.5f, // 右下后
         0.5f, -0.5f,  0.5f, // 右下前
         0.5f,  0.5f,  0.5f, // 右上前
         0.5f,  0.5f,  0.5f, // 右上前
         0.5f,  0.5f, -0.5f, // 右上后
         0.5f, -0.5f, -0.5f, // 右下后

        -0.5f, -0.5f, -0.5f, // 左下后
        -0.5f, -0.5f,  0.5f, // 左下前
        -0.5f,  0.5f,  0.5f, // 左上前
        -0.5f,  0.5f,  0.5f, // 左上前
        -0.5f,  0.5f, -0.5f, // 左上后
        -0.5f, -0.5f, -0.5f  // 左下后
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);   
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 顶点属性指针，现在是 3 个 float 为一个位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 编译着色器函数
    auto compileShader = [](GLenum type, const char* src) {
        unsigned int s = glCreateShader(type);
        glShaderSource(s, 1, &src, nullptr);
        glCompileShader(s);
        // 检查编译错误 (此处省略，实际项目中应加入)
        return s;
    };

    unsigned int vs = compileShader(GL_VERTEX_SHADER, vShader);
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fShader);

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    // 检查链接错误 (此处省略，实际项目中应加入)

    glDeleteShader(vs);
    glDeleteShader(fs);

    // 获取着色器中 uniform 变量的位置 (为了后面向它们传值)
    unsigned int modelLoc = glGetUniformLocation(program, "model");
    unsigned int viewLoc = glGetUniformLocation(program, "view");
    unsigned int projLoc = glGetUniformLocation(program, "projection");


    while (!glfwWindowShouldClose(window)) {
        // ====================================================================
        // 新增：清空颜色缓冲和深度缓冲
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // 清除深度缓冲
        // ====================================================================

        glUseProgram(program);

        // ====================================================================
        // 计算并传递矩阵
        // 1. 模型矩阵 (Model Matrix)
        glm::mat4 model = glm::mat4(1.0f); // 初始为单位矩阵
        // 让正方体围绕 Y 轴旋转，速度由时间决定
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        // 将矩阵传给着色器
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        // 2. 观察矩阵 (View Matrix)
        // 摄像机位置: (0,0,3)，看向 (0,0,0)，上方向 (0,1,0)
        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), 
                                     glm::vec3(0.0f, 0.0f, 0.0f), 
                                     glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        // 3. 投影矩阵 (Projection Matrix)
        // 透视投影：45度视场角，宽高比，近裁剪面，远裁剪面
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                               (float)800 / (float)600, 
                                               0.1f, 100.0f);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
        // ====================================================================

        glBindVertexArray(VAO);
        // 绘制 36 个顶点
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(program);

    glfwTerminate();
    return 0;
}