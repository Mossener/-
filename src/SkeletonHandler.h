#ifndef SKELETON_HANDLER_H
#define SKELETON_HANDLER_H

#include "SkinnedModel.h"
#include <vector>

class SkeletonHandler {
public:
    std::vector<glm::mat4> m_FinalBoneMatrices;

    SkeletonHandler() { m_FinalBoneMatrices.resize(100, glm::mat4(1.0f)); }
public:
    void CalculateTransforms(aiNode* node, glm::mat4 parentTransform, float angle, SkinnedModel& model, int actionType);
};

#endif