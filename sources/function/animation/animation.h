#ifndef __ANIMATION_H__
#define __ANIMATION_H__

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <functional>
#include "bone.h"
#include "model.h"

struct Morph {
    // shapekey
    std::unordered_map<std::string, float> shapekeys;
    double timeStamp;
};

typedef std::unordered_map<unsigned int, std::string> UIntStringMap;
typedef std::unordered_map<std::string, BoneInfo> StringBoneInfoMap;
typedef std::unordered_map<std::string, Bone> StringBoneMap;

class Animation {
public:
    Animation() = default;
    Animation(const aiAnimation* animation, StringBoneInfoMap& modelBoneInfoMap, UIntStringMap& modelShapeKeysNameID);

    Bone* FindBone(const std::string& name);

	inline float GetTicksPerSecond() { return m_TicksPerSecond; }
	inline float GetDuration() { return m_Duration; }

    std::unordered_map<std::string, float> morphAnimUpdate(float animationTime);

    std::string m_Name;
	float m_Duration;
	int m_TicksPerSecond;

private:
	StringBoneMap m_BoneKeys;
    std::vector<Morph> m_MorphKeys;

    // laod bone animations, also check missing bone
    void ReadBonesAnims(const aiAnimation* animation, StringBoneInfoMap& modelBoneInfoMap);
    void ReadMorphAnims(const aiAnimation* animation, UIntStringMap& modelShapeKeysNameID);

};

class Animations {
public:
    Animations(const std::string &animationPath, aModel *model);
    Animations(const aiScene *scene, aModel *model);
    
    std::vector<Animation> &GetAnimations() { return m_Animations; }
    std::vector<std::string> GetAnimationNames() { return m_Names; }

    void ReadHierarchyData(BoneNode &dest, const aiNode *src);

    inline const BoneNode &GetBoneRootNode() { return m_RootBoneNode; }

private:
    std::vector<Animation> m_Animations;
    std::vector<std::string> m_Names;
    BoneNode m_RootBoneNode;

};

#endif // !__ANIMATION_H__