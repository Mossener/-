#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <ctime>

#include "SkinnedModel.h"
#include "SkeletonHandler.h"

#include <fstream>
#include <sstream>

enum GameStatus
{
    PLAYING,
    WIN_ANIMATING
};
GameStatus currentStatus = PLAYING;
float winTimer = 0.0f;
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;
// 如果没有图片，可以先随便用一张现有的，或者用一张纯绿色的图片
glm::vec3 cameraPos = glm::vec3(0, 12, 12);   // 摄像机当前实际位置
glm::vec3 cameraOffset = glm::vec3(0, 10, 8); // 摄像机相对于玩家的固定偏移量值
float smoothSpeed = 5.0f;                     // 平滑系数，数值越大跟随越快

// 全局音频引擎
ma_engine audioEngine;
bool hasStartedWinMusic = false; // 放在 main 函数外
ma_sound winBGM;
// --- 函数声明 ---
unsigned int LoadYourShaders(const char *vertexPath, const char *fragmentPath);
unsigned int loadTexture(const char *path);
void RenderQuad(); // 用于画地板和简单的方块
void RenderCube()
{
    static unsigned int cubeVAO = 0;
    static unsigned int cubeVBO = 0;
    if (cubeVAO == 0)
    {
        float vertices[] = {
            // Position            // TexCoords  // Normals
            // Back face
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, -1.0f,
            // Front face
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
            // Left face
            -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
            // Right face
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
            // Bottom face
            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
            // Top face
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f};
        glGenVertexArrays(1, &cubeVAO);
        glGenBuffers(1, &cubeVBO);
        glBindVertexArray(cubeVAO);
        glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

        // 注意：步长（Stride）现在是 8 * sizeof(float)
        // 位置 (location = 0)
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
        // 纹理 (location = 1)
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float)));
        // 法线 (location = 2) - 核心新增
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(5 * sizeof(float)));
    }
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glBindVertexArray(0);
}
// --- 简单的游戏地图与箱子逻辑 ---
struct GameState
{
    glm::vec3 playerPos = glm::vec3(0, 0, 0);
    std::vector<glm::vec3> boxes = {
        glm::vec3(0, 0, -2.0f),
        glm::vec3(2.0f, 0, -2.0f),
        glm::vec3(-2.0f, 0, -4.0f)};
} gameState;
// 阴影配置
const unsigned int SHADOW_WIDTH = 2048, SHADOW_HEIGHT = 2048;
unsigned int depthMapFBO, depthMap;

void initShadows()
{
    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    // 注意：这里使用 GL_DEPTH_COMPONENT，只存深度，不存颜色
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = {1.0, 1.0, 1.0, 1.0}; // 边缘设为白色，防止视锥体外也是阴影
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE); // 告诉OpenGL不渲染颜色
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}
// 简化的地图格点系统：0-空地, 1-墙体, 2-目的地
int gameMap[11][11] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 2, 0, 0, 0, 2, 0, 1}, // 2 是目的地
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1}, // 中心 (0,0) 对应 [5][5]
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}};

bool CheckWin()
{
    int countOnTarget = 0;
    for (const auto &box : gameState.boxes)
    {
        // 使用 round 处理精度问题
        int ix = (int)glm::round(box.x / 2.0f) + 5;
        int iz = (int)glm::round(box.z / 2.0f) + 5;

        // 调试打印：如果判定不出来，看控制台输出的坐标对不对
        // std::cout << "Box at Index: [" << iz << "][" << ix << "] MapValue: " << gameMap[iz][ix] << std::endl;

        if (ix >= 0 && ix < 11 && iz >= 0 && iz < 11)
        {
            if (gameMap[iz][ix] == 2)
            {
                countOnTarget++;
            }
        }
    }
    // 只有当所有箱子（这里是3个）都在目的地时才算赢
    return countOnTarget == gameState.boxes.size();
}
// 工具函数：世界坐标转地图数组下标
bool isWall(glm::vec3 pos)
{
    int ix = (int)(pos.x / 2.0f) + 5;
    int iz = (int)(pos.z / 2.0f) + 5;
    if (ix < 0 || ix > 10 || iz < 0 || iz > 10)
        return true;
    return gameMap[iz][ix] == 1;
}

int TryMove(glm::vec3 direction, float &outRot)
{
    glm::vec3 nextP = gameState.playerPos + direction;

    // 设置旋转
    if (direction.z < 0)
        outRot = 180.0f;
    else if (direction.z > 0)
        outRot = 0.0f;
    else if (direction.x < 0)
        outRot = -90.0f;
    else if (direction.x > 0)
        outRot = 90.0f;

    // 1. 撞墙了吗？
    if (isWall(nextP))
        return 0;

    // 2. 撞箱子了吗？
    for (int i = 0; i < gameState.boxes.size(); i++)
    {
        if (glm::length(gameState.boxes[i] - nextP) < 0.1f)
        {
            glm::vec3 boxNextP = gameState.boxes[i] + direction;

            // 箱子的前方是墙吗？
            if (isWall(boxNextP))
                return 0;

            // 箱子的前方有另一个箱子吗？
            for (int j = 0; j < gameState.boxes.size(); j++)
            {
                if (i != j && glm::length(gameState.boxes[j] - boxNextP) < 0.1f)
                    return 0;
            }

            // 可以推动
            gameState.boxes[i] = boxNextP;
            gameState.playerPos = nextP;
            return 2;
        }
    }

    // 3. 只是普通的走路
    gameState.playerPos = nextP;
    return 1;
}

// --- 全局配置 ---
glm::vec3 lightPos(-2.0f, 15.0f, -1.0f);

// 提取出的渲染函数：供 Pass 1 和 Pass 2 调用
unsigned int lightCubeShader;

void RenderAllObjects(unsigned int shaderID, SkinnedModel &kobeModel, SkeletonHandler &skel, float animAngle, int currentAction, float playerRot, std::vector<glm::mat4> &identityMatrices, unsigned int kobeTex, unsigned int boxTex, unsigned int bactTex, unsigned int endTex)
{

    // 获取 uniform 位置
    int isAnimLoc = glGetUniformLocation(shaderID, "isAnimated");
    int modelLoc = glGetUniformLocation(shaderID, "model");
    int boneLoc = glGetUniformLocation(shaderID, "finalBonesMatrices");

    // --- 1. 绘制玩家 (带骨骼) ---
    // 在 RenderAllObjects 函数内
    // --- 1. 绘制玩家 (带骨骼) ---
    glUniform1i(isAnimLoc, 1);

    // 计算骨骼动画
    skel.CalculateTransforms(kobeModel.m_Scene->mRootNode, glm::mat4(1.0f), animAngle, kobeModel, currentAction);

    // 【关键修改点】：处理模型浮空
    glm::mat4 modelMat = glm::translate(glm::mat4(1.0f), gameState.playerPos);

    // 假设你的地面在 Y = -1.1，如果模型悬浮，我们在这里向下压低位置
    // 尝试修改这个 -1.1f，直到脚踩在地上为止
    modelMat = glm::translate(modelMat, glm::vec3(0.0f, -0.9f, 0.0f));

    // 处理旋转
    modelMat = glm::rotate(modelMat, glm::radians(playerRot), glm::vec3(0, 1, 0));

    // 如果模型太大，取消下面这行的注释并调整数值
    // modelMat = glm::scale(modelMat, glm::vec3(0.01f));

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(modelMat));

    if (!skel.m_FinalBoneMatrices.empty())
    {
        glUniformMatrix4fv(boneLoc, (GLsizei)skel.m_FinalBoneMatrices.size(), GL_FALSE, glm::value_ptr(skel.m_FinalBoneMatrices[0]));
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, kobeTex);
    glBindVertexArray(kobeModel.VAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)kobeModel.vertices.size());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, kobeTex);
    glBindVertexArray(kobeModel.VAO);
    glDrawArrays(GL_TRIANGLES, 0, (GLsizei)kobeModel.vertices.size());

    // --- 2. 绘制静态物体 ---
    glUniform1i(isAnimLoc, 0); // 关闭骨骼变形
    glUniformMatrix4fv(boneLoc, 100, GL_FALSE, glm::value_ptr(identityMatrices[0]));

    // 绘制地图格点
    for (int x = -5; x <= 5; x++)
    {
        for (int z = -5; z <= 5; z++)
        {
            int tileType = gameMap[z + 5][x + 5];
            glm::mat4 m = glm::translate(glm::mat4(1.0f), glm::vec3(x * 2.0f, -1.1f, z * 2.0f));

            if (tileType == 1)
            { // 墙
                glBindTexture(GL_TEXTURE_2D, boxTex);
                m = glm::translate(m, glm::vec3(0, 1.0f, 0));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
                RenderCube();
            }
            else if (tileType == 2)
            { // 目的地
                glBindTexture(GL_TEXTURE_2D, endTex);
                m = glm::scale(m, glm::vec3(1.0f, 0.05f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
                RenderCube();
            }
            else
            { // 地板
                glBindTexture(GL_TEXTURE_2D, bactTex);
                m = glm::scale(m, glm::vec3(1.0f, 0.1f, 1.0f));
                glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(m));
                RenderCube();
            }
        }
    }
    // 绘制箱子
    glBindTexture(GL_TEXTURE_2D, boxTex);
    for (const auto &boxPos : gameState.boxes)
    {
        glm::mat4 bMat = glm::translate(glm::mat4(1.0f), boxPos);
        bMat = glm::scale(bMat, glm::vec3(0.8f));
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(bMat));
        RenderCube();
    }
}
int main()
{
    srand((unsigned int)time(0));
    if (!glfwInit())
        return -1;
    GLFWwindow *window = glfwCreateWindow(800, 600, "Kobe Sokoban Pro", NULL, NULL);
    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glEnable(GL_DEPTH_TEST);

    if (ma_engine_init(NULL, &audioEngine) != MA_SUCCESS)
    {
        std::cout << "无法初始化音频引擎" << std::endl;
    }
    // 必须调用！初始化阴影帧缓冲
    initShadows();
    bool hasPlayedSound = false; // 放在 while 循环外
    // 1. 加载资源
    SkinnedModel kobeModel;
    kobeModel.LoadFBX("T-Pose.fbx");
    SkeletonHandler skeletonHandler;
    unsigned int kobeTex = loadTexture("shaded_fbx.png");
    unsigned int boxTex = loadTexture("box.jpeg");
    unsigned int bactTex = loadTexture("bac.jpeg");
    unsigned int endTex = loadTexture("end.jpeg");
    unsigned int winTex = loadTexture("kobe_win.jpeg");

    // 加载 Shader
    unsigned int mainShader = LoadYourShaders("shaders/skinned_vshader.glsl", "shaders/fshader.glsl");
    unsigned int depthShader = LoadYourShaders("shaders/depth_v.glsl", "shaders/depth_f.glsl");
    lightCubeShader = LoadYourShaders("shaders/light_cube_v.glsl", "shaders/light_cube_f.glsl");

    // 2. 变量
    float lastFrame = 0.0f;
    float animAngle = 0.0f, attackTimer = 0.0f, playerRot = 180.0f;
    bool isAnimating = false;
    int currentAction = 2;
    std::vector<glm::mat4> identityMatrices(100, glm::mat4(1.0f));
    glm::vec3 smoothedTargetPos = gameState.playerPos;
    GameStatus currentStatus = PLAYING;
    float winTimer = 0.0f;
    glm::vec3 smoothedLookAt = gameState.playerPos;
    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = (float)glfwGetTime();
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // --- 1. 基础逻辑更新 ---
        smoothedTargetPos = glm::mix(smoothedTargetPos, gameState.playerPos, 5.0f * deltaTime);

        // 关键修复：在这里检测胜利并切换状态
        if (currentStatus == PLAYING && CheckWin())
        {
            currentStatus = WIN_ANIMATING;
            winTimer = 0.0f;
        }

        // --- 2. 动画与输入逻辑 ---
        // 在这里定义 moved，确保它的作用域在整个 while 循环内（如果需要的话）
        bool moved = false;
        glm::vec3 dir(0.0f);

        if (currentStatus == PLAYING)
        {
            if (!isAnimating)
            {
                if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
                {
                    dir.z = -2.0f;
                    moved = true;
                }
                else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
                {
                    dir.z = 2.0f;
                    moved = true;
                }
                else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
                {
                    dir.x = -2.0f;
                    moved = true;
                }
                else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
                {
                    dir.x = 2.0f;
                    moved = true;
                }

                if (moved)
                {
                    int res = TryMove(dir, playerRot);
                    if (res > 0)
                    {
                        isAnimating = true;
                        attackTimer = 0.0f;
                        hasPlayedSound = false; // 重置 Man 的声音开关
                        // res == 2 是推箱子动作(肘击)，否则是走路动作(Action 2)
                        currentAction = (res == 2) ? (rand() % 2) : 2;
                    }
                }
            }
        }
        else if (currentStatus == WIN_ANIMATING)
        {
            winTimer += deltaTime;
            playerRot += deltaTime * 50.0f; // 胜利旋转
            isAnimating = true;             // 强制开启肘击

            if (!hasStartedWinMusic)
            {
                ma_result result = ma_sound_init_from_file(&audioEngine, "seeYouAgain.mp3", MA_SOUND_FLAG_DECODE, NULL, NULL, &winBGM);
                if (result == MA_SUCCESS)
                {
                    ma_sound_set_looping(&winBGM, MA_TRUE);
                    ma_sound_start(&winBGM);
                }
                hasStartedWinMusic = true;
            }
        }

        // --- 3. 动画计时器与声音触发 ---
        if (isAnimating)
        {
            attackTimer += deltaTime;
            float animDuration = 0.6f;
            float progress = attackTimer / animDuration;

            // 触发 Man 声音
            if (progress >= 0.1f && !hasPlayedSound)
            {
                if (currentAction != 2 || currentStatus == WIN_ANIMATING)
                {
                    ma_engine_play_sound(&audioEngine, "man.mp3", NULL);
                }
                hasPlayedSound = true;
            }

            animAngle = sin(progress * 3.14159f) * 110.0f;

            if (attackTimer >= animDuration)
            {
                if (currentStatus == WIN_ANIMATING)
                {
                    attackTimer = 0.0f;
                    hasPlayedSound = false;
                }
                else
                {
                    isAnimating = false;
                    animAngle = 0.0f;
                    hasPlayedSound = false;
                }
            }
        }

        // 灯光转动
        lightPos.x = sin(glfwGetTime() * 0.5f) * 10.0f;

        // --- 3. 相机参数计算 (核心特写逻辑) ---
        glm::vec3 targetCameraPos;
        glm::vec3 targetLookAt;

        if (currentStatus == WIN_ANIMATING)
        {
            // 计算老大的脸部位置 (考虑了之前 -1.1f 的模型偏移)
            glm::vec3 facePos = gameState.playerPos + glm::vec3(0.0f, 1.7f - 1.1f, 0.0f);

            // 计算脸部前方的特写位置 (根据旋转计算前方)
            float angleRad = glm::radians(playerRot);
            glm::vec3 forwardVector = glm::vec3(sin(angleRad), 0.3f, cos(angleRad));

            targetCameraPos = facePos + forwardVector * 2.5f; // 距离脸部2.5单位
            targetLookAt = facePos;
        }
        else
        {
            // 正常跟随视角
            targetCameraPos = smoothedTargetPos + cameraOffset;
            targetLookAt = smoothedTargetPos;
        }

        // 应用平滑差值
        float camSmooth = (currentStatus == WIN_ANIMATING) ? 3.0f : 5.0f;
        cameraPos = glm::mix(cameraPos, targetCameraPos, camSmooth * deltaTime);
        smoothedLookAt = glm::mix(smoothedLookAt, targetLookAt, camSmooth * deltaTime);

        glm::mat4 proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
        glm::mat4 view = glm::lookAt(cameraPos, smoothedLookAt, glm::vec3(0, 1, 0));

        // --- 4. PASS 1: 生成阴影贴图 ---
        glm::mat4 lightProjection = glm::ortho(-20.0f, 20.0f, -20.0f, 20.0f, 0.1f, 60.0f);
        glm::mat4 lightView = glm::lookAt(lightPos, smoothedTargetPos, glm::vec3(0, 1, 0));
        glm::mat4 lightSpaceMatrix = lightProjection * lightView;

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(depthShader);
        glUniformMatrix4fv(glGetUniformLocation(depthShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        RenderAllObjects(depthShader, kobeModel, skeletonHandler, animAngle, currentAction, playerRot, identityMatrices, 0, 0, 0, 0);

        // --- 5. PASS 2: 最终渲染 ---
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(mainShader);
        glUniformMatrix4fv(glGetUniformLocation(mainShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(mainShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(mainShader, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        glUniform3fv(glGetUniformLocation(mainShader, "lightPos"), 1, &lightPos[0]);
        glUniform3fv(glGetUniformLocation(mainShader, "viewPos"), 1, &cameraPos[0]);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, depthMap);
        glUniform1i(glGetUniformLocation(mainShader, "shadowMap"), 1);
        glUniform1i(glGetUniformLocation(mainShader, "texture1"), 0);

        RenderAllObjects(mainShader, kobeModel, skeletonHandler, animAngle, currentAction, playerRot, identityMatrices, kobeTex, boxTex, bactTex, endTex);

        // --- 6. 辅助渲染 (灯光方块与 UI) ---
        glUseProgram(lightCubeShader);
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "projection"), 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glm::mat4 lModel = glm::translate(glm::mat4(1.0f), lightPos);
        lModel = glm::scale(lModel, glm::vec3(0.5f));
        glUniformMatrix4fv(glGetUniformLocation(lightCubeShader, "model"), 1, GL_FALSE, glm::value_ptr(lModel));
        RenderCube();

        // 胜利 UI 特效
        if (currentStatus == WIN_ANIMATING && winTimer > 1.5f)
        {
            glDisable(GL_DEPTH_TEST);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, winTex);
            // 使用简单的渲染逻辑绘制全屏胜利图
            RenderQuad();
            glEnable(GL_DEPTH_TEST);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();
    return 0;
}

// 辅助函数：绘制一个简单的平面
void RenderQuad()
{
    static unsigned int quadVAO = 0;
    static unsigned int quadVBO;
    if (quadVAO == 0)
    {
        float vertices[] = {
            // 位置            // 纹理 (由于 Shader 要求位置属性 location=0)
            -0.8f, 0.0f, -0.8f, 0.0f, 0.0f,
            0.8f, 0.0f, -0.8f, 1.0f, 0.0f,
            0.8f, 0.0f, 0.8f, 1.0f, 1.0f,
            -0.8f, 0.0f, 0.8f, 0.0f, 1.0f};
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

// --- Shader 和 Texture 加载函数保持原样 ---
unsigned int loadTexture(const char *path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(path, &width, &height, &nrChannels, 4);
    if (data)
    {
        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }
    return textureID;
}

unsigned int LoadYourShaders(const char *vertexPath, const char *fragmentPath)
{
    std::string vertexCode, fragmentCode;
    std::ifstream vShaderFile(vertexPath), fShaderFile(fragmentPath);
    std::stringstream vShaderStream, fShaderStream;
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();

    const char *vShaderCode = vertexCode.c_str();
    const char *fShaderCode = fragmentCode.c_str();

    unsigned int vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);

    unsigned int fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);

    unsigned int ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    glLinkProgram(ID);
    return ID;
}