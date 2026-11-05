#ifndef LOADERCPP
#define LOADERCPP

#include <iostream>

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
} Vertex;

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
        
        Model(char *path) {
            loadModel(path);
        }
        void Draw(unsigned int shaderProgram);
        glm::vec3 ComputeCentroid();
        float ComputeScale(float targetSize);
        void ActivateShader();

    private:
        void loadModel(std::string path);
        std::string directory;
        std::vector<Mesh> meshes;
        void processNode(aiNode *node, const aiScene *scene);
        Mesh processMesh(aiMesh *mesh, const aiScene *scene);
        std::vector<Texture> loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName);
};

#endif