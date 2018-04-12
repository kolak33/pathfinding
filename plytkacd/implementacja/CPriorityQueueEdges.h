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


	bool GetUsed() { return m_bUsed; }
	void SetUsed(bool b) { m_bUsed = b; }
private:
	std::vector<CPriorityQueueNode> m_Neighbours;

	CGridTile m_GridTileInfo;
	bool m_bUsed = false;
};

