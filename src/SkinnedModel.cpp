#include "SkinnedModel.h"
#include <assimp/postprocess.h>
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

// 辅助函数：将 Assimp 的 4x4 矩阵转换为 GLM 格式
// 注意：Assimp 是行优先存储，GLM 是列优先，所以需要转置处理
static inline glm::mat4 aiMatrixToGlm(const aiMatrix4x4& from) {
    glm::mat4 to;
    to[0][0] = from.a1; to[1][0] = from.a2; to[2][0] = from.a3; to[3][0] = from.a4;
    to[0][1] = from.b1; to[1][1] = from.b2; to[2][1] = from.b3; to[3][1] = from.b4;
    to[0][2] = from.c1; to[1][2] = from.c2; to[2][2] = from.c3; to[3][2] = from.c4;
    to[0][3] = from.d1; to[1][3] = from.d2; to[2][3] = from.d3; to[3][3] = from.d4;
    return to;
}

void SkinnedModel::LoadFBX(const std::string& path) {
    // 增加 aiProcess_GlobalScale 确保 Mixamo 模型比例正常
    m_Scene = m_Importer.ReadFile(path, 
        aiProcess_Triangulate | 
        aiProcess_FlipUVs | 
        aiProcess_LimitBoneWeights | 
        aiProcess_GlobalScale);

    if (!m_Scene || m_Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !m_Scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << m_Importer.GetErrorString() << std::endl;
        return;
    }
    
    // 清空旧数据（如果是重新加载）
    vertices.clear();
    m_BoneInfoMap.clear();
    m_BoneCounter = 0;

    ProcessNode(m_Scene->mRootNode, m_Scene);
    SetupMesh();
}

void SkinnedModel::ProcessNode(aiNode* node, const aiScene* scene) {
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        ProcessMesh(mesh, scene);
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        ProcessNode(node->mChildren[i], scene);
    }
}

void SkinnedModel::ProcessMesh(aiMesh* mesh, const aiScene* scene) {
    // 1. 处理顶点基本数据
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        // 初始化骨骼 ID 为 -1，权重为 0
        for(int j = 0; j < MAX_BONE_INFLUENCE; j++) {
            vertex.m_BoneIDs[j] = -1;
            vertex.m_Weights[j] = 0.0f;
        }

        vertex.Position = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        if (mesh->mTextureCoords[0])
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);
    }

    // 2. 处理骨骼和权重
    for (unsigned int i = 0; i < mesh->mNumBones; i++) {
        aiBone* bone = mesh->mBones[i];
        std::string boneName = bone->mName.C_Str();
        
        // 如果该骨骼未记录，则创建新的 BoneInfo
        if (m_BoneInfoMap.find(boneName) == m_BoneInfoMap.end()) {
            BoneInfo info;
            info.id = m_BoneCounter++;
            // 【关键修改】：获取骨骼的 Offset Matrix
            info.offset = aiMatrixToGlm(bone->mOffsetMatrix);
            m_BoneInfoMap[boneName] = info;
        }

        int boneID = m_BoneInfoMap[boneName].id;
        aiVertexWeight* weights = bone->mWeights;
        int numWeights = bone->mNumWeights;

        for (int weightIndex = 0; weightIndex < numWeights; weightIndex++) {
            int vertexId = weights[weightIndex].mVertexId;
            float weight = weights[weightIndex].mWeight;
            
            // 找到该顶点的 vertices 索引（考虑之前可能已经加载了多个 Mesh）
            // 如果只有一个 Mesh，baseIndex 就是 0，否则需要记录偏移量
            int currentVertexPos = vertices.size() - mesh->mNumVertices + vertexId;
            
            for (int k = 0; k < MAX_BONE_INFLUENCE; k++) {
                if (vertices[currentVertexPos].m_BoneIDs[k] < 0) {
                    vertices[currentVertexPos].m_BoneIDs[k] = boneID;
                    vertices[currentVertexPos].m_Weights[k] = weight;
                    break;
                }
            }
        }
    }
}

void SkinnedModel::SetupMesh() {
    glGenVertexArrays(1, &VAO);
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    
    // 0: 位置 (vec3)
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    
    // 1: 纹理 (vec2)
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    
    // 2: 法线 (vec3) - 检查你的 Vertex 结构体里是否有 Normal，如果有，必须加上
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    
    // 3: 骨骼 ID (ivec4) - 必须使用 IPointer
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
    
    // 4: 骨骼权重 (vec4)
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));

    glBindVertexArray(0);
}