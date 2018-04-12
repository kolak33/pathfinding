#pragma once
#include "CAstar.h"

struct CompareFValQueueNode
{
	bool operator()(const CPriorityQueueNode &lhs, const CPriorityQueueNode &rhs)
	{
		return (lhs.GetF() > rhs.GetF());
	}
};

class CFringeSearch :
	public CAstar
{
public:
	CFringeSearch();
	~CFringeSearch();

	void FindShortestPath(int iStartNode, int iGoalNode);


protected:

	//std::unordered_map<int, int> m_previous;
	//std::unordered_map<int, double> m_distSoFar2;
	std::unordered_map<int, std::list<CPriorityQueueNode>::iterator> m_frontierIterPos;


};

