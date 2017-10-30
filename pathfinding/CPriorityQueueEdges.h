#pragma once
#include <vector>
#include "CPriorityQueueNode.h"

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
	void AddNeighbour(CPriorityQueueNode &node) { m_Neighbours.push_back(node); }
	//TODO uwazac na to xD
	auto GetNextNeighbour() 
	{
		return m_nextNeighbourIter == m_Neighbours.end() ? ( m_nextNeighbourIter = m_Neighbours.begin() ) : ++m_nextNeighbourIter;
	}

	auto GetIterEnd() { return m_Neighbours.end(); }
	void SetNeighbourIterInit()
	{
		m_nextNeighbourIter = m_Neighbours.end();
	}

private:
	std::vector<CPriorityQueueNode> m_Neighbours;
	std::vector<CPriorityQueueNode>::iterator m_nextNeighbourIter;

};

