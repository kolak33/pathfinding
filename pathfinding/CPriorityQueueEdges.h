#pragma once
#include <vector>
#include "CPriorityQueueNode.h"
#include "CGridTile.h"

class CPriorityQueueEdges
{
public:
	CPriorityQueueEdges()
	{
	}
	~CPriorityQueueEdges()
	{
	}

	auto& GetNeighbours() { return m_Neighbours; }
	void AddNeighbour(CPriorityQueueNode node) { m_Neighbours.push_back(node); }
	
	auto GetIterEnd() { return m_Neighbours.end(); }

	void SetGridTileInfo(CGridTile gridTile) { m_GridTileInfo = gridTile; }
	CGridTile& GetGridTileInfo() { return m_GridTileInfo; }

private:
	std::vector<CPriorityQueueNode> m_Neighbours;
	std::vector<CPriorityQueueNode>::iterator m_nextNeighbourIter;

	CGridTile m_GridTileInfo;
};

