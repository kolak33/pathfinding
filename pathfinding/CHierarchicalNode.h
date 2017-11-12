#pragma once
class CHierarchicalNode
{
public:
	CHierarchicalNode();
	~CHierarchicalNode();

	CHierarchicalNode(int iClusterId, float fWeight, int iLowerLevelNodeId, bool bIsInterEdge, int iZeroLevelNodeId)
		: m_iClusterId(iClusterId)
		, m_fWeight(fWeight)
		, m_iLowerLevelNodeId(iLowerLevelNodeId)
		, m_bIsInterEdge(bIsInterEdge)
		, m_iZeroLevelNodeId(iZeroLevelNodeId)
	{
	}


	float   GetWeight() { return m_fWeight; }
	int	    GetLowerLevelNodeId() { return m_iLowerLevelNodeId; }
	void	SetClusterId(int iClusterId) { m_iClusterId = iClusterId; }
	int		GetClusterId() { return m_iClusterId; }
	bool	GetIsInterEdge() { return m_bIsInterEdge; }
	int		GetZeroLevelNodeId() { return m_iZeroLevelNodeId; }
private:
	int		m_iLowerLevelNodeId;
	int		m_iZeroLevelNodeId;
	float	m_fWeight;
	int		m_iClusterId;
	bool	m_bIsInterEdge;
};

