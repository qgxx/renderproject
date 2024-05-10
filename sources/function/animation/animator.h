#ifndef __ANIMATOR_H__
#define __ANIMATOR_H__

#include <glm/glm.hpp>
#include <assimp/scene.h>
#include <vector>
#include <map>
#include <functional>
#include "animation.h"
#include "bone.h"

class Animator {
public:
    Animator(Animations *animations, aModel *model);

    void UpdateAnimation(float dt);
    void PlayAnimation(Animation* pAnimation) {
        m_CurrentAnimation = pAnimation;
        m_CurrentTime = 0.0f;
    }
    void CalculateBoneTransform(const BoneNode *node, glm::mat4 parentTransform);

    inline void SetCurrentTime(float time) { m_CurrentTime = time; }
    inline std::vector<glm::mat4> &GetFinalBoneMatrices() { return m_FinalBoneMatrices; }
    inline float GetAnimationDuration() { return m_CurrentAnimation->m_Duration; }
    inline float GetCurrentFrame() { return m_CurrentTime; }
    inline std::string GetAnimationName() { return m_CurrentAnimation->m_Name; }

private:
	std::vector<glm::mat4> m_FinalBoneMatrices;
    Animation *m_CurrentAnimation;
    Animations *m_Animations;
    aModel *m_Model;
    float m_CurrentTime;
    unsigned int boneMatrixTexture;

};

static void ModelImport(const std::string path, aModel **model, Animations **animations, Animator **animator, Shader &shader)
{
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    shader.use();
    shader.setInt("boneMatrixImage", 10);

    if (*model != nullptr || *animations != nullptr || *animator != nullptr)
        std::cout << __FUNCTION__ << ":error" << std::endl;

    *model = new aModel(scene, path);
    *animations = new Animations(scene, *model);
    *animator = new Animator(*animations, *model);
}

#endif // !__ANIMATOR_H__