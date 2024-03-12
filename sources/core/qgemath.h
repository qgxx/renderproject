#ifndef __QGE_MATH_H__
#define __QGE_MATH_H__

#include <iostream>
#include <stdlib.h>
#include <glm/glm.hpp>

#define powi(base,exp) (int)powf((float)(base), (float)(exp))

inline bool isPowerOf2(int x) {
    return (x & (x - 1));
}

inline int next_power_of2(int x) {
    if (x == 1) return 2;
    int ret = 1;
    while (ret < x) ret <<= 1;
    return ret;
}

inline float RandomFloat() {
    float Max = RAND_MAX;
    return ((float)rand() / Max);
}

inline float RandomFloatRange(float Start, float End) {
    if (End == Start) {
        printf("Invalid random range: (%f, %f)\n", Start, End);
        exit(0);
    }
    float Delta = End - Start;
    float RandomValue = RandomFloat() * Delta + Start;
    return RandomValue;
}

class FrustumCulling {
public:
    FrustumCulling(const glm::mat4& ViewProj)
    {
        Update(ViewProj);
    }

    void Update(const glm::mat4& ViewProj) {
        glm::vec4 Row1(ViewProj[0][0], ViewProj[0][1], ViewProj[0][2], ViewProj[0][3]);
        glm::vec4 Row2(ViewProj[1][0], ViewProj[1][1], ViewProj[1][2], ViewProj[1][3]);
        glm::vec4 Row3(ViewProj[2][0], ViewProj[2][1], ViewProj[2][2], ViewProj[2][3]);
        glm::vec4 Row4(ViewProj[3][0], ViewProj[3][1], ViewProj[3][2], ViewProj[3][3]);

        m_leftClipPlane = Row1 + Row4;
        m_rightClipPlane = Row1 - Row4;
        m_bottomClipPlane = Row2 + Row4;
        m_topClipPlane = Row2 - Row4;
        m_nearClipPlane = Row3 + Row4;
        m_farClipPlane = Row3 - Row4;
    }

    bool IsPointInsideViewFrustum(const glm::vec3& p) const {
        glm::vec4 p4D(p, 1.0f);

        bool Inside = (glm::dot(m_leftClipPlane, p4D) >= 0) &&
            (glm::dot(m_rightClipPlane, p4D) <= 0) /*&&
            (glm::dot(m_nearClipPlane, p4D) >= 0) &&
            (glm::dot(m_farClipPlane, p4D) <= 0) && 
            (glm::dot(m_topClipPlane, p4D) <= 0) &&
            (glm::dot(m_bottomClipPlane, p4D) >= 0)*/;

        return Inside;
    }

private:
    glm::vec4 m_leftClipPlane;
    glm::vec4 m_rightClipPlane;
    glm::vec4 m_bottomClipPlane;
    glm::vec4 m_topClipPlane;
    glm::vec4 m_nearClipPlane;
    glm::vec4 m_farClipPlane;

};

#endif // !__QGE_MATH_H__