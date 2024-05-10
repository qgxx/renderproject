#include "bone.h"

Bone::Bone(const std::string& name, int ID, const aiNodeAnim* channel) : m_Name(name), 
m_ID(ID), m_LocalTransform(1.0f) {
    m_NumPositions = channel->mNumPositionKeys;

    for (int positionIndex = 0; positionIndex < m_NumPositions; ++positionIndex) {
        aiVector3D aiPosition = channel->mPositionKeys[positionIndex].mValue;
        float timeStamp = channel->mPositionKeys[positionIndex].mTime;
        m_Positions.push_back({AssimpGLMHelpers::GetGLMVec(aiPosition), timeStamp});
    }

    m_NumRotations = channel->mNumRotationKeys;
    for (int rotationIndex = 0; rotationIndex < m_NumRotations; ++rotationIndex) {
        aiQuaternion aiOrientation = channel->mRotationKeys[rotationIndex].mValue;
        float timeStamp = channel->mRotationKeys[rotationIndex].mTime;
        m_Rotations.push_back({AssimpGLMHelpers::GetGLMQuat(aiOrientation), timeStamp});
    }

    m_NumScalings = channel->mNumScalingKeys;
    for (int keyIndex = 0; keyIndex < m_NumScalings; ++keyIndex) {
        aiVector3D scale = channel->mScalingKeys[keyIndex].mValue;
        float timeStamp = channel->mScalingKeys[keyIndex].mTime;
        m_Scales.push_back({AssimpGLMHelpers::GetGLMVec(scale), timeStamp});
    }
}

void Bone::Update(float animationTime) {
    glm::mat4 translation = InterpolatePosition(animationTime);
        glm::mat4 rotation = InterpolateRotation(animationTime);
        glm::mat4 scale = InterpolateScaling(animationTime);
        m_LocalTransform = translation * rotation * scale;
}

int Bone::GetPositionIndex(float animationTime) {
    int index = 0;
    for (; index < m_NumPositions - 1; ++index) {
        if (animationTime < m_Positions[index + 1].timeStamp) return index;
    }
    return index;
}

int Bone::GetRotationIndex(float animationTime) {
    int index = 0;
    for (; index < m_NumRotations - 1; ++index) {
        if (animationTime < m_Rotations[index + 1].timeStamp) return index;
    }
    return index;
}

int Bone::GetScaleIndex(float animationTime) {
    int index = 0;
    for (; index < m_NumScalings - 1; ++index) {
        if (animationTime < m_Scales[index + 1].timeStamp) return index;
    }
    return index;
}

float Bone::GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime) {
    return ((animationTime - lastTimeStamp) / (nextTimeStamp - lastTimeStamp));
}

glm::mat4 Bone::InterpolatePosition(float animationTime) {
    if (1 == m_NumPositions)
        return glm::translate(glm::mat4(1.0f), m_Positions[0].position);

    int pIndex = GetPositionIndex(animationTime);

    // If "Force Start/End Keying" is not selected when exporting the model in blender
    if ((pIndex == 0 && m_Positions[pIndex].timeStamp >= animationTime) || pIndex == m_NumPositions - 1)
        return glm::translate(glm::mat4(1.0f), m_Positions[pIndex].position);

    float scaleFactor = GetScaleFactor(m_Positions[pIndex].timeStamp, m_Positions[pIndex + 1].timeStamp, animationTime);
    glm::vec3 finalPosition = glm::mix(m_Positions[pIndex].position, m_Positions[pIndex + 1].position, scaleFactor);

    return glm::translate(glm::mat4(1.0f), finalPosition);
}

glm::mat4 Bone::InterpolateRotation(float animationTime) {
    if (1 == m_NumRotations)
        return glm::toMat4(m_Rotations[0].orientation);

    int pIndex = GetRotationIndex(animationTime);

    if ((pIndex == 0 && m_Rotations[pIndex].timeStamp >= animationTime) || pIndex == m_NumRotations - 1)
        return glm::toMat4(m_Rotations[pIndex].orientation);

    float scaleFactor = GetScaleFactor(m_Rotations[pIndex].timeStamp, m_Rotations[pIndex + 1].timeStamp, animationTime);
    glm::quat finalRotation = glm::slerp(m_Rotations[pIndex].orientation, m_Rotations[pIndex + 1].orientation, scaleFactor);

    return glm::toMat4(finalRotation);
}

glm::mat4 Bone::InterpolateScaling(float animationTime) {
    if (1 == m_NumScalings)
        return glm::scale(glm::mat4(1.0f), m_Scales[0].scale);

    int pIndex = GetScaleIndex(animationTime);

    if ((pIndex == 0 && m_Scales[pIndex].timeStamp >= animationTime) || pIndex == m_NumScalings - 1)
        return glm::scale(glm::mat4(1.0f), m_Scales[pIndex].scale);

    float scaleFactor = GetScaleFactor(m_Scales[pIndex].timeStamp, m_Scales[pIndex + 1].timeStamp, animationTime);
    glm::vec3 finalScale = glm::mix(m_Scales[pIndex].scale, m_Scales[pIndex + 1].scale, scaleFactor);

    return glm::scale(glm::mat4(1.0f), finalScale);
} 