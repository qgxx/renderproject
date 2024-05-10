#include "animator.h"

Animator::Animator(Animations *animations, aModel *model)
{
    if (animations == nullptr || animations->GetAnimations().empty())
    {
        std::cout << "animations is empty" << std::endl;
        exit(-1);
    }

    m_Animations = animations;
    m_Model = model;

    // default first animation
    m_CurrentTime = 0.0;
    m_CurrentAnimation = &m_Animations->GetAnimations()[0];
    m_FinalBoneMatrices.resize(m_Model->GetBoneInfoMap().size());

    // ---------------------------------- sampler2D boneMatrixImage;
    glGenTextures(1, &boneMatrixTexture);
    glBindTexture(GL_TEXTURE_2D, boneMatrixTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, 4, m_Model->GetBoneInfoMap().size(), 0, GL_RGBA, GL_FLOAT, NULL);
}

void Animator::UpdateAnimation(float dt) {
    if (m_CurrentAnimation) {
        m_CurrentTime += m_CurrentAnimation->GetTicksPerSecond() * dt;
        m_CurrentTime = fmod(m_CurrentTime, m_CurrentAnimation->GetDuration());

        auto morphAnimKeys = m_CurrentAnimation->morphAnimUpdate(m_CurrentTime);
        m_Model->SetMorphAnimKeys(morphAnimKeys);

        CalculateBoneTransform(&m_Animations->GetBoneRootNode(), glm::mat4(1.0f));
    }

    // ---------------------------  sampler2D boneMatrixImage;
    std::vector<float> pixelData(m_FinalBoneMatrices.size() * 16);
    memcpy(pixelData.data(), m_FinalBoneMatrices.data(), pixelData.size() * sizeof(float));

    glActiveTexture(GL_TEXTURE10);
    glBindTexture(GL_TEXTURE_2D, boneMatrixTexture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 4, m_FinalBoneMatrices.size(), GL_RGBA, GL_FLOAT, &pixelData[0]);
}

void Animator::CalculateBoneTransform(const BoneNode *node, glm::mat4 parentTransform)
{
    std::string nodeName = node->name;
    glm::mat4 nodeTransform = node->transformation;
    Bone *Bone = m_CurrentAnimation->FindBone(nodeName);

    if (Bone)
    {
        Bone->Update(m_CurrentTime);
        nodeTransform = Bone->GetLocalTransform();
    }

    glm::mat4 globalTransformation = parentTransform * nodeTransform;

    auto &boneInfoMap = m_Model->GetBoneInfoMap();
    if (boneInfoMap.find(nodeName) != boneInfoMap.end())
    {
        m_FinalBoneMatrices[boneInfoMap.at(nodeName).id] = globalTransformation * boneInfoMap.at(nodeName).offset;
    }

    for (int i = 0; i < node->childrenCount; i++)
    {
        CalculateBoneTransform(&node->children[i], globalTransformation);
    }
}