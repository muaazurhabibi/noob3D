#include "loader.hpp"

// helper function to load texture data
unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false)
{
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << filename << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// mesh class definitions
Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
{
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;

    //std::cout << "Vertices:" << std::endl; for (Vertex x : vertices) std::cout << x.Pos.x << " " << x.Pos.y << " " << x.Pos.z << std::endl;

    std::cout << "Setting up mesh..." << std::endl;
    setupMesh();
}

void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, Norm));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, UV));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
}

void Mesh::Draw(unsigned int shaderProgram)
{
    glUseProgram(shaderProgram);
    unsigned int diffN = 1;
    unsigned int specN = 1;

    for (size_t i = 0; i < textures.size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        std::string number, name = textures[i].type;

        if (name == "texture_diffuse")
        {
            number = std::to_string(diffN++);
        } else
        if (name == "texture_specular")
        {
            number = std::to_string(specN++);
        }
        
        glUniform1i(glGetUniformLocation(shaderProgram, ("material."+name+number).c_str()), i);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
    }
    glActiveTexture(GL_TEXTURE0);

    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}


// model class definitions
void Model::Draw(unsigned int shaderProgram)
{
    for (size_t i = 0; i < meshes.size(); i++)
    {
        meshes[i].Draw(shaderProgram);
    }
}

void Model::loadModel(std::string path)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
    if(!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << '\n';
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    
    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

std::vector<Texture> Model::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        Texture texture;
        texture.id = TextureFromFile(str.C_Str(), directory);
        texture.type = typeName;
        texture.path = str.C_Str();
        textures.push_back(texture);
    }
    return textures;
}

Mesh Model::processMesh(aiMesh *mesh, const aiScene *scene)
{
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex v;

        v.Pos.x = mesh->mVertices[i].x, v.Pos.y = mesh->mVertices[i].y, v.Pos.z = mesh->mVertices[i].z;
        v.Norm.x = mesh->mNormals[i].x, v.Norm.y = mesh->mNormals[i].y, v.Norm.z = mesh->mNormals[i].z;
        if (mesh->mTextureCoords[0])
        {
            v.UV.x = mesh->mTextureCoords[0][i].x;
            v.UV.y = mesh->mTextureCoords[0][i].y;
        } else {
            v.UV = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(v);
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
        {
            indices.push_back(face.mIndices[j]);
        }
    }
    
    if (mesh->mMaterialIndex >= 0)
    {
        aiMaterial *mat = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture> diffMaps = loadMaterialTextures(mat, aiTextureType_DIFFUSE, "texture_diffuse");
        std::vector<Texture> specMaps = loadMaterialTextures(mat, aiTextureType_SPECULAR, "texture_specular");

        textures.insert(textures.end(), diffMaps.begin(), diffMaps.end());
        textures.insert(textures.end(), specMaps.begin(), specMaps.end());
    }
    
    return Mesh(vertices, indices, textures);
}

glm::vec3 Model::ComputeCentroid()
{
    glm::vec3 minB(FLT_MAX), maxB(-FLT_MAX);
    for (auto &mesh : meshes)
        for (auto &v : mesh.vertices)
        {
            minB = glm::min(minB, v.Pos);
            maxB = glm::max(maxB, v.Pos);
        }
    return (minB + maxB) * 0.5f;
}


float Model::ComputeScale(float targetSize)
{
    glm::vec3 minB(FLT_MAX), maxB(-FLT_MAX);
    for (auto &mesh : meshes)
        for (auto &v : mesh.vertices)
        {
            minB = glm::min(minB, v.Pos);
            maxB = glm::max(maxB, v.Pos);
        }
    glm::vec3 extent = maxB - minB;
    float maxExtent = glm::max(extent.x, glm::max(extent.y, extent.z));
    return targetSize / maxExtent; // e.g. targetSize = 2.0f for a ~2-unit-tall model
}