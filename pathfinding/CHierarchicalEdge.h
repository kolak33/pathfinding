#pragma once
#include "CHierarchicalNode.h"
#include <vector>
class CHierarchicalEdge
{
public:
	CHierarchicalEdge();
	~CHierarchicalEdge();


	auto& GetNeighbours() { return m_Neighbours; }
	void AddNeighbour(CHierarchicalNode node) { m_Neighbours.push_back(node); }

	auto GetIterEnd() { return m_Neighbours.end(); }


private:
	std::vector<CHierarchicalNode> m_Neighbours;
	std::vector<CHierarchicalNode>::iterator m_nextNeighbourIter;


	int m_iFirstLevelNodeId;
};

