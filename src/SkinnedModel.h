#ifndef SKINNED_MODEL_H
#define SKINNED_MODEL_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <string>
#include <vector>
#include <map>

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    glm::vec3 Position;
    glm::vec2 TexCoords;
    glm::vec3 Normal;
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    float m_Weights[MAX_BONE_INFLUENCE];

    Vertex() {
        for (int i = 0; i < MAX_BONE_INFLUENCE; i++) {
            m_BoneIDs[i] = -1;
            m_Weights[i] = 0.0f;
        }
    }
};

struct BoneInfo {
    int id;
    glm::mat4 offset;
};

class SkinnedModel {
public:
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::map<std::string, BoneInfo> m_BoneInfoMap;
    int m_BoneCounter = 0;
    unsigned int VAO;

    void LoadFBX(const std::string& path);
    const aiScene* GetScene() { return m_Scene; }

public:
    const aiScene* m_Scene;
    Assimp::Importer m_Importer;

    void ProcessNode(aiNode* node, const aiScene* scene);
    void ProcessMesh(aiMesh* mesh, const aiScene* scene);
    void SetupMesh();
};

#endif