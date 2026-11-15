#ifndef BONEHPP
#define BONEHPP

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

typedef struct {
    glm::vec3 Position;
    float timestamp;
} KeyPos;

typedef struct {
    glm::quat Orientation;
    float timestamp;
} KeyRot;

typedef struct {
    glm::vec3 Scale;
    float timestamp;
} KeyScale;

class Bone
{
    private:
        std::vector<KeyPos> positions;
        std::vector<KeyRot> rotations;
        std::vector<KeyScale> scales;
        int n_poss;
        int n_rots;
        int n_scales;

        glm::mat4 localTransform;
        std::string name;
        int id;

        glm::mat4 InterpolatePos(float animTime);
        glm::mat4 InterpolateRot(float animTime);
        glm::mat4 InterpolateScale(float animTime);

    public:
        Bone(const std::string &name, int id, const aiNodeAnim *channel);
        void update(float animTime);
        glm::mat4 getLocalTransform() { return localTransform; }
        std::string getName() { return name; }
        int getId() { return id; }
        int getPosIdx(float animTime);
        int getRotIdx(float animTime);
        int getScaleIdx(float animTime);
        float getScaleFactor(float lastTime, float nextTime, float animTime);
};


#endif