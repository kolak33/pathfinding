#pragma once
#include "CAstar.h"

struct ComparePriorityQueueNode
{
	bool operator()(const CPriorityQueueNode &lhs, const CPriorityQueueNode &rhs)
	{
		return (lhs.GetF() > rhs.GetF());
	}
};

class CJPSearch :
	public CAstar
{
public:
	CJPSearch();
	~CJPSearch();

	void FindShortestPath(int iStartNode, int iGoalNode);
	void IdentifySuccessors(int iCurrNode, int iStartNode, int iGoalNode, 
		std::priority_queue<CPriorityQueueNode, std::vector<CPriorityQueueNode>, ComparePriorityQueueNode> &dijQueue);
	void FindNeighbours(std::vector<int> &neighbours, int iCurrNode);
	int Jump(int iCurrNode, int iNeighbour, int iGoal);
	void AddNeighbourIfExists(std::vector<int> &neighbours, int iCurrNode);

	bool AddNeighbourOnCondition(std::vector<int> &neighbours, int iXPos, int iYPos, bool bExpectedVal, int iXPosNeigh, int iYPosNeigh);
	


	std::map<int, int> m_previous;
	std::map<int, double> m_distSoFar2;
	std::map<int, bool> m_closedList;
};
