#pragma once
#include "CHierarchicalNode.h"
#include <vector>
class CHierarchicalEdge
{
public:
	CHierarchicalEdge();
	~CHierarchicalEdge();

	CHierarchicalEdge(int iId, int iOwnId)
		: m_iId(iId)
		, m_iOwnId(iOwnId)
	{
	}


	auto& GetNeighbours() { return m_Neighbours; }
	void AddNeighbour(CHierarchicalNode node) { m_Neighbours.push_back(node); }

	auto GetIterEnd() { return m_Neighbours.end(); }
	int GetId() { return m_iId; }
	void SetId(int iId) { m_iId = iId; }
	void SetOwnId(int iOwnId) { m_iOwnId = iOwnId; }
	void	SetClusterId(int iClusterId) { m_iClusterId = iClusterId; }
	int		GetClusterId() { return m_iClusterId; }
	int		GetOwnId() { return m_iOwnId; }
private:
	std::vector<CHierarchicalNode> m_Neighbours;
	std::vector<CHierarchicalNode>::iterator m_nextNeighbourIter;

	int m_iId; // to jest zero lvl node id
	int m_iClusterId;
	int m_iFirstLevelNodeId; // nie uzywane

	//TODO zmienic nazwy z m_iId
	//teraz: m_iId to tileId z zerowego lvla
	//m_iOwnId to wlasny id uzywany przy abstr wyszukiwaniu A*, odpowiada miejcu w abstr grafie, node z m_iOwnId = 5, jest w m_Graph[5]
	//potrzebny jest zeby nie trzeba bylo przy kazdym wyszukiwaniu A* alokowac pamieci
	//na wszystkie mozliwe nody z zerowego lvla, oraz pewnie przyspieszy to wyszukiwanie std::find_if, ale to trzeba sprawdzic
	int m_iOwnId;
};

