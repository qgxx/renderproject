#include "animation.h"

Animation::Animation(const aiAnimation* animation, StringBoneInfoMap& modelBoneInfoMap, UIntStringMap& modelShapeKeysNameID) {
    m_Name = animation->mName.data;
    m_Duration = animation->mDuration;
    m_TicksPerSecond = animation->mTicksPerSecond;

    ReadBonesAnims(animation, modelBoneInfoMap);
    ReadMorphAnims(animation, modelShapeKeysNameID);
}

Bone* Animation::FindBone(const std::string& name) {
    if (auto bone = m_BoneKeys.find(name); bone != m_BoneKeys.end()) {
        return &(bone->second);
    }
    return nullptr;
}

void Animation::ReadBonesAnims(const aiAnimation* animation, StringBoneInfoMap& modelBoneInfoMap) {
    for (unsigned int i = 0; i < animation->mNumChannels; i++) {
        auto channel = animation->mChannels[i];
        std::string name = channel->mNodeName.data;
        if (auto info = modelBoneInfoMap.find(name); info != modelBoneInfoMap.end()) {
            m_BoneKeys.insert(std::make_pair(name, Bone{name, info->second.id, channel}));
        }
    }
}

void Animation::ReadMorphAnims(const aiAnimation* animation, UIntStringMap& modelShapeKeysNameID) {
    if (animation->mNumMorphMeshChannels) {
        auto morphmeshchannels = animation->mMorphMeshChannels[0];
        for (unsigned int i = 0; i < morphmeshchannels->mNumKeys; ++i) {
            auto& keys = morphmeshchannels->mKeys[i];
            std::unordered_map<std::string, float> shapekeys;
            for (unsigned int j = 0; j < keys.mNumValuesAndWeights; ++j) {
                shapekeys.insert(std::make_pair(modelShapeKeysNameID[(keys.mValues[j])].data(), keys.mWeights[j]));
            }

            // insert missing from last frame
            if (m_MorphKeys.size())
                for (auto &lastShapeKey : m_MorphKeys.back().shapekeys) {
                    if (shapekeys.count(lastShapeKey.first) == 0 && lastShapeKey.second != 0)
                        shapekeys.insert(lastShapeKey);
                }

            m_MorphKeys.emplace_back(Morph{shapekeys, keys.mTime});
        }

        // If "Force Start/End Key" is selected when exporting the fbx model in blender
        // This will add all shapekeys to the start / end frames, we don't need these
        // Reomve first and last frame
        if (modelShapeKeysNameID.size() == (*(m_MorphKeys.begin())).shapekeys.size())
            m_MorphKeys.erase(m_MorphKeys.begin());
        if (modelShapeKeysNameID.size() == (*(m_MorphKeys.end() - 1)).shapekeys.size())
            m_MorphKeys.pop_back();
    }
}

std::unordered_map<std::string, float> Animation::morphAnimUpdate(float animationTime) {
    std::unordered_map<std::string, float> morphAnimKey;
    // Get morphAnim index
    unsigned int index;
    if (m_MorphKeys.size()) {
        for (index = 0; index < m_MorphKeys.size() - 1; ++index) {
            if (animationTime < m_MorphKeys[index + 1].timeStamp) break;
        }
    }
    else return morphAnimKey;

    // first key frame and last key frame
    if (index == 0 || index == m_MorphKeys.size() - 1) return m_MorphKeys[index].shapekeys;

    int p0Index = index;
    int p1Index = p0Index + 1;

    float scaleFactor = (animationTime - m_MorphKeys[p0Index].timeStamp) / (m_MorphKeys[p1Index].timeStamp - m_MorphKeys[p0Index].timeStamp);

    for (const auto &pair : m_MorphKeys[p0Index].shapekeys) {
        std::string shapeKeyName = pair.first;
        float p0_weight = pair.second;
        float fin_weight;
        if (m_MorphKeys[p1Index].shapekeys.count(pair.first) != 0) {
            float p1_weight = m_MorphKeys[p1Index].shapekeys[pair.first];
            fin_weight = glm::mix(p0_weight, p1_weight, scaleFactor);
        }
        else fin_weight = p0_weight;

        morphAnimKey.emplace(shapeKeyName, fin_weight);
        // cout << shapeKeyName << ":" << fin_weight << " ";
    }

    return morphAnimKey;
}

Animations::Animations(const std::string &animationPath, aModel *model)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(animationPath, aiProcess_Triangulate);

    // load bone nodes
    ReadHierarchyData(m_RootBoneNode, scene->mRootNode);

    // load bone morph animations
    for (unsigned int index = 0; index < scene->mNumAnimations; ++index)
    {
        auto animation = scene->mAnimations[index];
        m_Animations.emplace_back(Animation(animation, model->GetBoneInfoMap(), model->shapeKeysNameID));
        m_Names.push_back(animation->mName.data);
    }
}

Animations::Animations(const aiScene *scene, aModel *model)
{
    // load bone nodes
    ReadHierarchyData(m_RootBoneNode, scene->mRootNode);

    // load bone morph animations
    for (unsigned int index = 0; index < scene->mNumAnimations; ++index)
    {
        auto animation = scene->mAnimations[index];
        m_Animations.emplace_back(Animation(animation, model->GetBoneInfoMap(), model->shapeKeysNameID));
        m_Names.push_back(animation->mName.data);
    }
}

void Animations::ReadHierarchyData(BoneNode &dest, const aiNode *src)
{
    dest.name = src->mName.data;
    dest.transformation = AssimpGLMHelpers::ConvertMatrixToGLMFormat(src->mTransformation);
    dest.childrenCount = src->mNumChildren;

    for (unsigned int i = 0; i < src->mNumChildren; i++)
    {
        BoneNode newData;
        ReadHierarchyData(newData, src->mChildren[i]);
        dest.children.push_back(newData);
    }
}