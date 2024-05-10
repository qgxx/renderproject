#ifndef __BONE_H__
#define __BONE_H__

#include <vector>
#include <list>
#include <string>
#include <unordered_map>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "core/utils/depends_helper.hpp"

struct BoneNode
{
    glm::mat4 transformation;
    std::string name;
    int childrenCount;
    std::vector<BoneNode> children;
};

struct BoneInfo
{
    // id is index in finalBoneMatrices
    int id;
    // offset matrix transforms vertex from model space to bone space
    glm::mat4 offset;
};

struct KeyPosition
{
    glm::vec3 position;
    float timeStamp;
};

struct KeyRotation
{
    glm::quat orientation;
    float timeStamp;
};

struct KeyScale
{
    glm::vec3 scale;
    float timeStamp;
};

class Bone final {
public:
    Bone(const std::string& name, int ID, const aiNodeAnim* channel);
    void Update(float animationTime);

    int GetPositionIndex(float animationTime);
    int GetRotationIndex(float animationTime);
    int GetScaleIndex(float animationTime);
	glm::mat4 GetLocalTransform() { return m_LocalTransform; }
	std::string GetBoneName() const { return m_Name; }
	int GetBoneID() { return m_ID; }

	float GetScaleFactor(float lastTimeStamp, float nextTimeStamp, float animationTime);
    glm::mat4 InterpolatePosition(float animationTime);
    glm::mat4 InterpolateRotation(float animationTime);
    glm::mat4 InterpolateScaling(float animationTime); 

	std::vector<KeyPosition> m_Positions;
    std::vector<KeyRotation> m_Rotations;
    std::vector<KeyScale> m_Scales;

private:
	int m_NumPositions;
	int m_NumRotations;
	int m_NumScalings;

	glm::mat4 m_LocalTransform;
	std::string m_Name;
	int m_ID;

};

#endif // !__BONE_H__