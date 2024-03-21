#ifndef __QUADTREE_H__
#define __QUADTREE_H__

#include <vector>
#include <functional>
#include <glm/glm.hpp>

class QuadTree {
public:
    struct Node {
		int subnodes[4];
		glm::vec2 start;		
		float length;			
		int	lod;
		Node();
		inline bool IsLeaf() const {
			return (subnodes[0] == -1 && subnodes[1] == -1 && subnodes[2] == -1 && subnodes[3] == -1);
		}
    };

private:
	typedef std::vector<Node> NodeList;
	typedef std::function<void (const QuadTree::Node&)> NodeCallback;

	NodeList	nodes;
	Node		root;
	int			numlods;		// number of LOD levels
	int			meshdim;		// patch mesh resolution
	float		patchlength;	// world space patch size
	float		maxcoverage;	// any node larger than this will be subdivided
	float		screenarea;

	float CalculateCoverage(const Node& node, const glm::mat4& proj, const glm::vec3& eye) const;
	bool IsVisible(const Node& node, const glm::mat4& viewproj) const;
	void InternalTraverse(const Node& node, NodeCallback callback) const;

	int FindLeaf(const glm::vec2& point) const;
	int BuildTree(Node& node, const glm::mat4& viewproj, const glm::mat4& proj, const glm::vec3& eye);

public:
    QuadTree();

	void FindSubsetPattern(int outindices[4], const Node& node);
	void Initialize(const glm::vec2& start, float size, int lodcount, int meshsize, float patchsize, 
                    float maxgridcoverage, float screensize);
	void Rebuild(const glm::mat4& viewproj, const glm::mat4& proj, const glm::vec3& eye);
	void Traverse(NodeCallback callback) const;
};

#endif  // !__QUADTREE_H__