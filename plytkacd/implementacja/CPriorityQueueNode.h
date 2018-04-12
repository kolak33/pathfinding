#pragma once
class CPriorityQueueNode
{
public:
	CPriorityQueueNode();

	CPriorityQueueNode(int iNodeId, float fWeight);

	~CPriorityQueueNode();

	float  GetWeight() { return m_fWeight; }
	double GetF() const { return m_f; }
	void   SetF(double f) { m_f = f; }
	//double GetG() { return m_g; }
	//double GetH() { return m_h; }
	int	   GetNodeId() { return m_iNodeId; }
private:
	int		m_iNodeId;
	float	m_fWeight;
	double	m_f;
	//double	m_g;
	//double	m_h;
};

