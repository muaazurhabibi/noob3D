#ifndef LOADERCPP
#define LOADERCPP

#define MAX_BONE_INFLUENCES 4

#include <iostream>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../stb_image/stb_image.h"

#include "../glm/glm.hpp"
#include "../glm/gtc/matrix_transform.hpp"
#include "../glm/gtc/type_ptr.hpp"

typedef struct {
    glm::vec3 Pos;
    glm::vec3 Norm;
    glm::vec2 UV;
    glm::vec3 Tangent;
    glm::vec3 BiTangent;

    int m_BoneIDS[MAX_BONE_INFLUENCES];
    float m_BoneWeights[MAX_BONE_INFLUENCES];
} Vertex;

typedef struct {
    int id;
    glm::mat4 offset;
} BoneInfo;

typedef struct
{
    unsigned int id;
    std::string type;
    std::string path;
} Texture;

class Mesh {
    public:
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
        std::vector<Texture> textures;

        glm::mat4 model = glm::mat4(1.0f);
        unsigned int modelUniform, viewUniform;

        Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures);
        void Draw(unsigned int shaderProgram);
        void ActivateShader();

    private:
        unsigned int VAO, VBO, EBO;
        void setupMesh();
};

class Model {
    public:
        glm::mat4 model = glm::mat4(1.0f);
        unsigned int modelUniform, viewUniform;
        
        Model(char *path) {
            loadModel(path);
        }
        void Draw(unsigned int shaderProgram);
        glm::vec3 ComputeCentroid();
        float ComputeScale(float targetSize);
        void ActivateShader();

    private:
        std::map<std::string, BoneInfo> BoneInfoMap;
        int nBones = 0;
        std::string directory;
        std::vector<Mesh> meshes;

        void loadModel(std::string path);
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
        void setVertexBones(Vertex *vertex, int boneID, float weight);
        void defaultVertexBones(Vertex *v);
        void extractVerticesBoneWeight(std::vector<Vertex> *vertices, aiMesh *mesh, const aiScene *scene);
};

#endif