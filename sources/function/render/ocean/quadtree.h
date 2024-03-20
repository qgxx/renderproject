#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include <vector>
#include <functional>
#include <glm/glm.hpp>
#include "core/qgemath.h"

#define CHOOPY_SCALE_CORRECTION	1.35f

class QuadTree {
public:
    struct Node {
        int subNodes[4];
        glm::vec2 start;
        float length;
        int lod;
        
        Node();
        inline bool isLeaf() const {
            return (subNodes[0] == -1 && subNodes[1] == -1 && subNodes[2] == -1 && subNodes[3] == -1);
        }
    };
    QuadTree();

	void FindSubsetPattern(int outindices[4], const Node& node);
	void Initialize(const glm::vec2& start, float size, int lodcount, int meshsize, float patchsize, float maxgridcoverage, float screensize);
	void Rebuild(const glm::mat4& viewproj, const glm::mat4& proj, const glm::vec3& eye);

private:
    typedef std::vector<Node> NodeList;
    typedef std::function<void (const QuadTree::Node&)> NodeCallback;

    NodeList nodes;
    Node root;
    int numlods;  // number of LOD levels
    int meshdim;  // patch mesh resolution
    float patch_length;  // world space patch size
    float max_coverage;
    float screen_area;

	float CalculateCoverage(const Node& node, const glm::mat4& proj, const glm::vec3& eye) const;
	bool IsVisible(const Node& node, const glm::mat4& viewproj) const;
	void InternalTraverse(const Node& node, NodeCallback callback) const;

	int FindLeaf(const Vector2& point) const;
	int BuildTree(Node& node, const glm::mat4& viewproj, const glm::mat4& proj, const glm::vec3& eye);

public:
    void Traverse(NodeCallback callback) const;

};

#endif // !__QUADTREE_H__