#ifndef ANIMATIONHPP
#define ANIMATIONHPP

#include <iostream>
#include <map>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "../../stb_image/stb_image.h"

#include "../../glm/glm.hpp"
#include "../../glm/gtc/matrix_transform.hpp"
#include "../../glm/gtc/type_ptr.hpp"

#include "../loader.hpp"
#include "../bone/bone.hpp"

typedef struct {
    glm::mat4 transform;
    std::string name;
    int nChildren;
    std::vector<AssimpNodeData> children;
} AssimpNodeData;

class Animation
{
    private:
        float duration;
        float ticksPerSec;
        std::vector<Bone> bones;
        AssimpNodeData root;
        std::map<std::string, BoneInfo> boneInfoMap;

    public:
        Animation() = default;
        Animation(const std::string& animPath, Model *model);
        Bone *findBone(const std::string &name);
};

#endif