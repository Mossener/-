#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>

// 引入 GLM 数学库
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp> // 用于变换（平移、旋转、缩放）
#include <glm/gtc/type_ptr.hpp>         // 用于将 GLM 矩阵转换为 OpenGL 可用的格式

// 读取着色器文件的函数
std::string readShaderFile(const char* filePath) {
    std::ifstream shaderFile;
    std::stringstream shaderStream;

    shaderFile.open(filePath);
    if (shaderFile.is_open()) {
        shaderStream << shaderFile.rdbuf();
        shaderFile.close();
    } else {
        std::cerr << "无法打开着色器文件: " << filePath << std::endl;
    }
    return shaderStream.str();
}

// 编译着色器函数
unsigned int compileShader(GLenum type, const char* src) {
    unsigned int s = glCreateShader(type);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);

    // 检查编译错误
    int success;
    char infoLog[512];
    glGetShaderiv(s, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(s, 512, nullptr, infoLog);
        std::cerr << "着色器编译失败: " << infoLog << std::endl;
    }
    return s;
}

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

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);   

    // 正方体的 36 个顶点数据 (每个面 2 个三角形，共 6 个面)
    float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,
     0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,

    -0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f, -0.5f,  0.5f,

    -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f, -0.5f,
    -0.5f, -0.5f, -0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,

     0.5f,  0.5f,  0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f, -0.5f,
     0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,  0.5f,

    -0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f,  0.5f,
     0.5f, -0.5f,  0.5f, -0.5f, -0.5f,  0.5f, -0.5f, -0.5f, -0.5f,

    -0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f,  0.5f,
     0.5f,  0.5f,  0.5f, -0.5f,  0.5f,  0.5f, -0.5f,  0.5f, -0.5f,
    };

    unsigned int VAO, VBO;
    glGenVertexArrays(1, &VAO);   
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // 读取着色器代码
    std::string vertexShaderSource = readShaderFile("shaders/vertex_shader.glsl");
    std::string fragmentShaderSource = readShaderFile("shaders/fragment_shader.glsl");

    unsigned int vs = compileShader(GL_VERTEX_SHADER, vertexShaderSource.c_str());
    unsigned int fs = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource.c_str());

    unsigned int program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);

    // 检查链接错误
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, 512, nullptr, infoLog);
        std::cerr << "着色器程序链接失败: " << infoLog << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    // 获取着色器中 uniform 变量的位置
    unsigned int modelLoc = glGetUniformLocation(program, "model");
    unsigned int viewLoc = glGetUniformLocation(program, "view");
    unsigned int projLoc = glGetUniformLocation(program, "projection");

    while (!glfwWindowShouldClose(window)) {
        // 清空颜色缓冲和深度缓冲
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(program);

        // 计算并传递矩阵
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

        glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), 
                                     glm::vec3(0.0f, 0.0f, 0.0f), 
                                     glm::vec3(0.0f, 1.0f, 0.0f));
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 
                                               (float)800 / (float)600, 
                                               0.1f, 100.0f);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));

        glBindVertexArray(VAO);
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
