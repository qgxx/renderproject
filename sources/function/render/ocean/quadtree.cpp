#include "quadtree.h"

QuadTree::Node::Node() {
	subNodes[0] = -1;
	subNodes[1] = -1;
	subNodes[2] = -1;
	subNodes[3] = -1;

	lod = 0;
	length = 0;
}

QuadTree::QuadTree() {
    numlods = 0;
	meshdim = 0;
	patch_length = 0;
	max_coverage = 0;
	screen_area = 0;
}

void QuadTree::Initialize(const glm::vec2& start, float size, int lodcount, int meshsize, float patchsize, float maxgridcoverage, float screensize) {
	root.start = start;
	root.length = size;

	numlods = lodcount;
	meshdim = meshsize;
	patch_length = patchsize;
	max_coverage = maxgridcoverage;
	screen_area = screensize;
}

void QuadTree::Rebuild(const glm::mat4& viewproj, const glm::mat4& proj, const glm::vec3& eye) {
	nodes.clear();
	BuildTree(root, viewproj, proj, eye);
}

int QuadTree::BuildTree(Node& node, const glm::mat4& viewproj, const glm::mat4& proj, const glm::vec3& eye) {
	if (!IsVisible(node, viewproj))
		return -1;

	float coverage = CalculateCoverage(node, proj, eye);
	bool visible = true;

	if (coverage > max_coverage && node.length > patch_length) {
		Node subnodes[4];

		subnodes[0].start = node.start;
		subnodes[1].start = { node.start.x + 0.5f * node.length, node.start.y };
		subnodes[2].start = { node.start.x + 0.5f * node.length, node.start.y + 0.5f * node.length };
		subnodes[3].start = { node.start.x, node.start.y + 0.5f * node.length };

		subnodes[0].length = subnodes[1].length = subnodes[2].length = subnodes[3].length = 0.5f * node.length;

		node.subNodes[0] = BuildTree(subnodes[0], viewproj, proj, eye);
		node.subNodes[1] = BuildTree(subnodes[1], viewproj, proj, eye);
		node.subNodes[2] = BuildTree(subnodes[2], viewproj, proj, eye);
		node.subNodes[3] = BuildTree(subnodes[3], viewproj, proj, eye);

		visible = !node.isLeaf();
	}

	if (visible) {
		int lod = 0;

		for (lod = 0; lod < numlods - 1; ++lod) {
			if (coverage > max_coverage)
				break;

			coverage *= 4.0f;
		}

		node.lod = std::min(lod, numlods - 2);
	} else {
		return -1;
	}

	int position = (int)nodes.size();
	nodes.push_back(node);

	return position;
}

float QuadTree::CalculateCoverage(const Node& node, const glm::mat4& proj, const glm::vec3& eye) const {
	const static Vector2 samples[16] = {
		{ 0, 0 },
		{ 0, 1 },
		{ 1, 0 },
		{ 1, 1 },
		{ 0.5f, 0.333f },
		{ 0.25f, 0.667f },
		{ 0.75f, 0.111f },
		{ 0.125f, 0.444f },
		{ 0.625f, 0.778f },
		{ 0.375f, 0.222f },
		{ 0.875f, 0.556f },
		{ 0.0625f, 0.889f },
		{ 0.5625f, 0.037f },
		{ 0.3125f, 0.37f },
		{ 0.8125f, 0.704f },
		{ 0.1875f, 0.148f },
	};

	Vector3 test;
	Vector3 vdir;
	float length		= node.length;
	float gridlength	= length / meshdim;
	float worldarea		= gridlength * gridlength;
	float maxprojarea	= 0;
	Matrix pr(proj[0][0], proj[0][1], proj[0][2], proj[0][3],
	proj[1][0], proj[1][1], proj[1][2], proj[1][3],
	proj[2][0], proj[2][1], proj[2][2], proj[2][3],
	proj[3][0], proj[3][1], proj[3][2], proj[3][3]);

	// NOTE: from nVidia (sample patch at given points and estimate coverage)
	for (int i = 0; i < 16; ++i) {
		test.x = (node.start.x - CHOOPY_SCALE_CORRECTION) + (length + 2 * CHOOPY_SCALE_CORRECTION) * samples[i].x;
		test.y = 0;
		test.z = (node.start.y - CHOOPY_SCALE_CORRECTION) + (length + 2 * CHOOPY_SCALE_CORRECTION) * samples[i].y;

		Vector3 eyeb(eye[0], eye[1], eye[2]);
		Vec3Subtract(vdir, test, eyeb);

		float dist = Vec3Length(vdir);
		float projarea = (worldarea * pr._11 * pr._22) / (dist * dist);

		if (maxprojarea < projarea)
			maxprojarea = projarea;
	}

	return maxprojarea * screen_area * 0.25f;
}

bool QuadTree::IsVisible(const Node& node, const glm::mat4& viewproj) const {
	AABox box;
	Vector4 planes[6];
	float length = node.length;

	// NOTE: depends on choopy scale
	box.Add(node.start.x - CHOOPY_SCALE_CORRECTION, -0.01f, node.start.y - CHOOPY_SCALE_CORRECTION);
	box.Add(node.start.x + length + CHOOPY_SCALE_CORRECTION, 0.01f, node.start.y + length + CHOOPY_SCALE_CORRECTION);
	Matrix vp(viewproj[0][0], viewproj[0][1], viewproj[0][2], viewproj[0][3],
	viewproj[1][0], viewproj[1][1], viewproj[1][2], viewproj[1][3],
	viewproj[2][0], viewproj[2][1], viewproj[2][2], viewproj[2][3],
	viewproj[3][0], viewproj[3][1], viewproj[3][2], viewproj[3][3]);
	FrustumPlanes(planes, vp);
	int result = FrustumIntersect(planes, box);

	return (result > 0);
}

void QuadTree::FindSubsetPattern(int outindices[4], const Node& node) {
	outindices[0] = 0;
	outindices[1] = 0;
	outindices[2] = 0;
	outindices[3] = 0;

	// NOTE: bottom: +Z, top: -Z
	Vector2 point_left	= { node.start.x - 0.5f * patch_length, node.start.y + 0.5f * node.length };
	Vector2 point_right	= { node.start.x + node.length + 0.5f * patch_length, node.start.y + 0.5f * node.length };
	Vector2 point_bottom	= { node.start.x + 0.5f * node.length, node.start.y + node.length + 0.5f * patch_length };
	Vector2 point_top		= { node.start.x + 0.5f * node.length, node.start.y - 0.5f * patch_length };

	int adjacency[4] = {
		FindLeaf(point_left),
		FindLeaf(point_right),
		FindLeaf(point_bottom),
		FindLeaf(point_top)
	};

	// NOTE: from nVidia (chooses the closest LOD degrees)
	for (int i = 0; i < 4; ++i) {
		if (adjacency[i] != -1 && nodes[adjacency[i]].length > node.length * 0.999f) {
			const Node& adj = nodes[adjacency[i]];
			float scale = adj.length / node.length * (meshdim >> node.lod) / (meshdim >> adj.lod);
		
			if (scale > 3.999f)
				outindices[i] = 2;
			else if (scale > 1.999f)
				outindices[i] = 1;
		}
	}
}

int QuadTree::FindLeaf(const Vector2& point) const
{
	int index = -1;
	int size = (int)nodes.size();

	Node node = nodes[size - 1];

	while (!node.isLeaf()) {
		bool found = false;

		for (int i = 0; i < 4; ++i) {
			index = node.subNodes[i];

			if (index == -1)
				continue;

			Node subnode = nodes[index];

			if (point.x >= subnode.start.x && point.x <= subnode.start.x + subnode.length &&
				point.y >= subnode.start.y && point.y <= subnode.start.y + subnode.length)
			{
				node = subnode;
				found = true;

				break;
			}
		}

		if (!found)
			return -1;
	}

	return index;
}

void QuadTree::InternalTraverse(const Node& node, NodeCallback callback) const
{
	if (node.isLeaf()) {
		callback(node);
	} else {
		if (node.subNodes[0] != -1)
			InternalTraverse(nodes[node.subNodes[0]], callback);

		if (node.subNodes[1] != -1)
			InternalTraverse(nodes[node.subNodes[1]], callback);

		if (node.subNodes[2] != -1)
			InternalTraverse(nodes[node.subNodes[2]], callback);

		if (node.subNodes[3] != -1)
			InternalTraverse(nodes[node.subNodes[3]], callback);
	}
}

void QuadTree::Traverse(NodeCallback callback) const
{
	if (nodes.size() == 0)
		return;

	InternalTraverse(nodes.back(), callback);
}
