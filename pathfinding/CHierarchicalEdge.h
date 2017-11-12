#pragma once
#include "CHierarchicalNode.h"
#include <vector>
class CHierarchicalEdge
{
public:
	CHierarchicalEdge();
	~CHierarchicalEdge();

	CHierarchicalEdge(int iId)
		: m_iId(iId)
	{
	}


	auto& GetNeighbours() { return m_Neighbours; }
	void AddNeighbour(CHierarchicalNode node) { m_Neighbours.push_back(node); }

	auto GetIterEnd() { return m_Neighbours.end(); }
	int GetId() { return m_iId; }
	void	SetClusterId(int iClusterId) { m_iClusterId = iClusterId; }
	int		GetClusterId() { return m_iClusterId; }
private:
	std::vector<CHierarchicalNode> m_Neighbours;
	std::vector<CHierarchicalNode>::iterator m_nextNeighbourIter;

	int m_iId; // to jest chyba zero lvl node id
	int m_iClusterId;
	int m_iFirstLevelNodeId; // nie uzywane
};

