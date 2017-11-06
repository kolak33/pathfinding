#include "stdafx.h"
#include "CPriorityQueueNode.h"
#include <limits>

CPriorityQueueNode::CPriorityQueueNode()
{
}

CPriorityQueueNode::CPriorityQueueNode(int iNodeId, float fWeight) 
	: m_iNodeId(iNodeId)
	, m_fWeight(fWeight)
	, m_f(std::numeric_limits<double>::max())
	//, m_g(std::numeric_limits<double>::max())
	//, m_h(std::numeric_limits<double>::max())
{
}


CPriorityQueueNode::~CPriorityQueueNode()
{
}
