#pragma once
class CHierarchicalNode
{
public:
	CHierarchicalNode();
	~CHierarchicalNode();


	float  GetWeight() { return m_fWeight; }
	int	   GetLowerLevelNodeId() { return m_iLowerLevelNodeId; }
private:
	int		m_iLowerLevelNodeId;
	float	m_fWeight;
	int		m_iClusterId;
};

