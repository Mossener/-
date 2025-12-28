#include "SkeletonHandler.h"
#include <iostream>

// 辅助函数：将 aiMatrix 转换为 glm::mat4
glm::mat4 aiToGlm(aiMatrix4x4 from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

void SkeletonHandler::CalculateTransforms(aiNode* node, glm::mat4 parentTransform, float angle, SkinnedModel& model, int actionType) {
    std::string nodeName = node->mName.data;
    glm::mat4 nodeTransform = aiToGlm(node->mTransformation);
    
    // 基础进度计算 (0.0 - 1.0)
    float strikeRatio = glm::clamp(angle / 110.0f, 0.0f, 1.0f);

    // ================= [分支 0：肘击动作逻辑] =================
    if(actionType == 0){
        float bodyTurnAngle = strikeRatio * 60.0f; 
        float bodyLeanAngle = strikeRatio * 20.0f; 
        float bodyForwardOffset = strikeRatio * 0.25f;

        if (nodeName == "mixamorig:Spine1") {
            nodeTransform = glm::translate(nodeTransform, glm::vec3(0.0f, 0.0f, bodyForwardOffset));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(bodyTurnAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(bodyLeanAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (nodeName == "mixamorig:Head") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(-bodyTurnAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (nodeName == "mixamorig:RightArm") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(-75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (nodeName == "mixamorig:RightForeArm") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (nodeName == "mixamorig:LeftArm") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (nodeName == "mixamorig:LeftForeArm") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
    }
    // ================= [分支 1：踢击动作逻辑] =================
    else if (actionType == 1) {
        float bodyTurnAngle = strikeRatio * 60.0f; 
        float bodyLeanAngle = strikeRatio * 20.0f; 
        float bodyForwardOffset = strikeRatio * 0.25f;

        if (nodeName == "mixamorig:Spine1") {
            nodeTransform = glm::translate(nodeTransform, glm::vec3(0.0f, 0.0f, bodyForwardOffset));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(bodyTurnAngle), glm::vec3(0.0f, 1.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(bodyLeanAngle), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (nodeName == "mixamorig:Head") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(-bodyTurnAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (nodeName == "mixamorig:RightArm") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(-75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (nodeName == "mixamorig:RightForeArm") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (nodeName == "mixamorig:LeftArm") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (nodeName == "mixamorig:LeftForeArm") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        // 右大腿：转胯与前踢
        if (nodeName == "mixamorig:RightUpLeg") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        // 右小腿：微调
        if (nodeName == "mixamorig:RightLeg") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 15.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        // 左大腿：支撑
        if (nodeName == "mixamorig:LeftUpLeg") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        // 左膝盖：重心下沉
        if (nodeName == "mixamorig:LeftLeg") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
    }
// ... 前面部分保持不变 ...

    // ================= [分支 2：移动/滑步动作逻辑] =================
    else if (actionType == 2) {
        float bodyTurnAngle = strikeRatio * 60.0f; 
        float bodyLeanAngle = strikeRatio * 20.0f; 
        float bodyForwardOffset = strikeRatio * 0.75f;
        if (nodeName == "mixamorig:RightArm") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(-75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (nodeName == "mixamorig:RightForeArm") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        if (nodeName == "mixamorig:LeftArm") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
            nodeTransform = glm::rotate(nodeTransform, glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        if (nodeName == "mixamorig:LeftForeArm") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
        }
        float moveRatio = strikeRatio; // 复用 0-1 的进度
        
        // 1. 整体重心移动 (通常修改 Hips 或 Pelvis)
        // 假设位移轴是 Z 轴。0.0 -> 1.5f 表示向前滑出一大步
        if (nodeName == "mixamorig:Hips") {
            float forwardMove = moveRatio * 0.5f; 
            // 增加一点上下起伏 (Bounce)，让移动更真实
            float verticalBounce = sin(moveRatio * 3.14159f) * 0.1f; 
            nodeTransform = glm::translate(nodeTransform, glm::vec3(0.0f, verticalBounce, forwardMove));
        }

        // 2. 身体前倾补偿
        if (nodeName == "mixamorig:Spine") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(moveRatio * 10.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }

        // 3. 腿部交替动作 (模拟垫步)
        if (nodeName == "mixamorig:RightUpLeg") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(moveRatio * -20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (nodeName == "mixamorig:LeftUpLeg") {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(moveRatio * 20.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
    }

// ... 后面的握拳和递归部分保持不变 ...
    // ================= [公共逻辑：握拳] =================
    if (nodeName.find("Hand") != std::string::npos) {
        bool isFinger = (nodeName.find("Index") != std::string::npos || nodeName.find("Middle") != std::string::npos || 
                         nodeName.find("Ring") != std::string::npos || nodeName.find("Pinky") != std::string::npos);
        if (isFinger) {
            nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        }
    }

    // --- 递归与矩阵合成 ---
    glm::mat4 globalTransformation = parentTransform * nodeTransform;
    if (model.m_BoneInfoMap.count(nodeName)) {
        int index = model.m_BoneInfoMap[nodeName].id;
        m_FinalBoneMatrices[index] = globalTransformation * model.m_BoneInfoMap[nodeName].offset;
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        CalculateTransforms(node->mChildren[i], globalTransformation, angle, model, actionType);
    }
}

// void SkeletonHandler::CalculateTransforms(aiNode* node, glm::mat4 parentTransform, float elbowAngle, SkinnedModel& model) {
//     std::string nodeName = node->mName.data;
//     glm::mat4 nodeTransform = aiToGlm(node->mTransformation);
    
//     // --- 1. 时间轴切分 ---
//     float totalRatio = glm::clamp(elbowAngle / 110.0f, 0.0f, 1.0f);
//     float strikeRatio = glm::clamp(totalRatio * 2.0f, 0.0f, 1.0f); 
//     float legRatio = glm::clamp((totalRatio -0.2f) * 2.0f, 0.0f, 1.0f); 

// // --- 2. 上半身旋转、倾斜与【向前位移】 ---
//     float bodyTurnAngle = strikeRatio * 60.0f; 
//     float bodyLeanAngle = strikeRatio * 20.0f; 
//     float bodyForwardOffset = strikeRatio *0.75f; // 上半身探出的距离

//     if (nodeName == "mixamorig:Spine1") {
//         // 先进行位移 (Translate)，让上半身相对于骨盆向前探出
//         // 注意：根据你的模型坐标系，位移轴可能是 vec3(0, 0, offset) 或 vec3(0, offset, 0)
//         nodeTransform = glm::translate(nodeTransform, glm::vec3(0.0f, 0.0f, bodyForwardOffset));

//         // 再进行旋转
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(bodyTurnAngle), glm::vec3(0.0f, 1.0f, 0.0f));
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(bodyLeanAngle), glm::vec3(1.0f, 0.0f, 0.0f));
//     }

//     // --- 头部补偿 ---
//     if (nodeName == "mixamorig:Head") {
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(-bodyTurnAngle), glm::vec3(0.0f, 1.0f, 0.0f));
//         // 如果前倾 bodyLeanAngle 导致头埋得太深，可以取消下面这行的注释来让头抬起来
//         // nodeTransform = glm::rotate(nodeTransform, glm::radians(-bodyLeanAngle), glm::vec3(1.0f, 0.0f, 0.0f));
//     }
//     // --- 3. 右手臂逻辑 ---
//     if (nodeName == "mixamorig:RightArm") {
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(-75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -45.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//     }
//     if (nodeName == "mixamorig:RightForeArm") {
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(-10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//     }

//     // --- 4. 左手臂逻辑 (防御架势) ---
//     if (nodeName == "mixamorig:LeftArm") {
//         // 左大臂稍微抬起并向前
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * -60.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(75.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//     }
//     if (nodeName == "mixamorig:LeftForeArm") {
//         // 左小臂向上弯曲成格挡状
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(10.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio * 90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//         // 这里可以根据需要增加更多弯曲，比如：
//         // nodeTransform = glm::rotate(nodeTransform, glm::radians(-40.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//     }
// // --- 5. 腿部动作 (随肘击同步发力) ---
//     // 右大腿 (RightUpLeg): 随着转身，右腿应该有一个向内扣（转胯）的动作
//     if (nodeName == "mixamorig:RightUpLeg") {
//         float legTurn = strikeRatio * 90.0f; 
//         // 绕 Y 轴旋转实现转胯
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(legTurn), glm::vec3(1.0f, 0.0f, 0.0f));
//     }

//     // 右小腿 (RightLeg): 蹬地时，膝盖会微调
//     if (nodeName == "mixamorig:RightLeg") {
//         float kneeBend = strikeRatio * 15.0f; 
//         // X 轴通常是膝盖弯曲轴
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(kneeBend), glm::vec3(1.0f, 0.0f, 0.0f));
//     }

//     // 左大腿 (LeftUpLeg): 支撑腿，随身体旋转微动
//     if (nodeName == "mixamorig:LeftUpLeg") {
//         float leftLegTurn = strikeRatio * 90.0f;
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(leftLegTurn), glm::vec3(1.0f, 0.0f, 0.0f));
//     }

//     // 左膝盖 (LeftLeg): 稍微弯曲降低重心，显得动作稳重
//     if (nodeName == "mixamorig:LeftLeg") {
//         float leftKneeBend = strikeRatio * 10.0f;
//         nodeTransform = glm::rotate(nodeTransform, glm::radians(leftKneeBend), glm::vec3(1.0f, 0.0f, 0.0f));
//     }



//     // --- 6. 左右手统一握拳逻辑 ---
//     // 通过判断字符串中包含 "Hand"，同时包含手指关键字
//     if (nodeName.find("Hand") != std::string::npos) {
        
//         bool isFinger = (nodeName.find("Index") != std::string::npos || 
//                          nodeName.find("Middle") != std::string::npos || 
//                          nodeName.find("Ring") != std::string::npos || 
//                          nodeName.find("Pinky") != std::string::npos ||
//                          nodeName.find("Finger") != std::string::npos);

//         if (isFinger) {
//             // 左右手手指通常绕局部 X 轴弯曲
//             // 如果左手弯曲方向反了，可能需要对左手单独判断正负号
//             float fingerAngle =strikeRatio* -90.0f; 
//             nodeTransform = glm::rotate(nodeTransform, glm::radians(fingerAngle), glm::vec3(1.0f, 0.0f, 0.0f));
//         }

//         if (nodeName.find("Thumb") != std::string::npos) {
//             // 大拇指
//             nodeTransform = glm::rotate(nodeTransform, glm::radians(strikeRatio* 45.0f), glm::vec3(0.0f, 0.0f, 1.0f));
//         }
//     }


//     // --- 7. 递归合成 ---
//     glm::mat4 globalTransformation = parentTransform * nodeTransform;
//     if (model.m_BoneInfoMap.count(nodeName)) {
//         int index = model.m_BoneInfoMap[nodeName].id;
//         m_FinalBoneMatrices[index] = globalTransformation * model.m_BoneInfoMap[nodeName].offset;
//     }
//     for (unsigned int i = 0; i < node->mNumChildren; i++) {
//         CalculateTransforms(node->mChildren[i], globalTransformation, elbowAngle, model);
//     }
// }