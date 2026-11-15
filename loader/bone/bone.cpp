#include "bone.hpp"

Bone::Bone(const std::string &name, int id, const aiNodeAnim *channel) : name(name), id(id), localTransform(1.0f)
{
    n_poss = channel->mNumPositionKeys;
    for (size_t i = 0; i < n_poss; i++)
    {
        aiVector3D pos = channel->mPositionKeys[i].mValue;
        float timestamp = channel->mPositionKeys[i].mTime;
        KeyPos data;

        data.Position = glm::vec3(pos.x, pos.y, pos.z);
        data.timestamp = timestamp;
        positions.push_back(data);
    }

    n_rots = channel->mNumRotationKeys;
    for (size_t i = 0; i < n_rots; i++)
    {
        aiQuaternion rot = channel->mRotationKeys[i].mValue;
        float timestamp = channel->mRotationKeys[i].mTime;
        KeyRot data;

        data.Orientation = glm::quat(rot.w, rot.x, rot.y, rot.z);
        data.timestamp = timestamp;
        rotations.push_back(data);
    }

    n_scales = channel->mNumScalingKeys;
    for (size_t i = 0; i < n_scales; i++)
    {
        aiVector3D scale = channel->mScalingKeys[i].mValue;
        float timestamp = channel->mScalingKeys[i].mTime;
        KeyScale data;

        data.Scale = glm::vec3(scale.x, scale.y, scale.z);
        data.timestamp = timestamp;
        scales.push_back(data);
    }
}

int Bone::getPosIdx(float animTime)
{
    for (size_t i = 0; i < n_poss; i++)
    {
        if (animTime < positions[i+1].timestamp)
        {
            return i;
        }
    }
    assert(0);
}

int Bone::getRotIdx(float animTime)
{
    for (size_t i = 0; i < n_rots; i++)
    {
        if (animTime < rotations[i+1].timestamp)
        {
            return i;
        }
    }
    assert(0);
}

int Bone::getScaleIdx(float animTime)
{
    for (size_t i = 0; i < n_scales; i++)
    {
        if (animTime < scales[i+1].timestamp)
        {
            return i;
        }
    }
    assert(0);
}

float Bone::getScaleFactor(float lastTime, float nextTime, float animTime)
{
    float sf = 0.0f;
    float mid = animTime - lastTime;
    float framesDiff = nextTime - lastTime;
    sf = mid / framesDiff;

    return sf;
}

glm::mat4 Bone::InterpolatePos(float animTime)
{
    if (n_poss == 1)
    {
        return glm::translate(glm::mat4(1.0f), positions[0].Position);
    }
    
    int p0Idx = getPosIdx(animTime);
    int p1Idx = p0Idx + 1;

    float sf = getScaleFactor(positions[p0Idx].timestamp, positions[p1Idx].timestamp, animTime);
    glm::vec3 finalPos = glm::mix(positions[p0Idx].Position, positions[p1Idx].Position, sf);

    return glm::translate(glm::mat4(1.0f), finalPos);
}

glm::mat4 Bone::InterpolateRot(float animTime)
{
    if (n_rots == 1)
    {
        return glm::mat4_cast(glm::normalize(rotations[0].Orientation));
    }
    
    int p0Idx = getRotIdx(animTime);
    int p1Idx = p0Idx + 1;

    float sf = getScaleFactor(rotations[p0Idx].timestamp, rotations[p1Idx].timestamp, animTime);
    glm::quat finalRot = glm::normalize(glm::slerp(rotations[p0Idx].Orientation, rotations[p1Idx].Orientation, sf));

    return glm::mat4_cast(finalRot);
}

glm::mat4 Bone::InterpolatePos(float animTime)
{
    if (n_scales == 1)
    {
        return glm::scale(glm::mat4(1.0f), scales[0].Scale);
    }
    
    int p0Idx = getPosIdx(animTime);
    int p1Idx = p0Idx + 1;

    float sf = getScaleFactor(scales[p0Idx].timestamp, scales[p1Idx].timestamp, animTime);
    glm::vec3 finalScale = glm::mix(scales[p0Idx].Scale, scales[p1Idx].Scale, sf);

    return glm::scale(glm::mat4(1.0f), finalScale);
}

void Bone::update(float animTime)
{
    glm::mat4 translation = InterpolatePos(animTime);
    glm::mat4 rotation = InterpolateRot(animTime);
    glm::mat4 scale = InterpolateScale(animTime);

    localTransform = translation * rotation * scale;
}