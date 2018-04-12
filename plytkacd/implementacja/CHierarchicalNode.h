#pragma once
class CHierarchicalNode
{
public:
	CHierarchicalNode();
	~CHierarchicalNode();

	CHierarchicalNode(int iClusterId, float fWeight, int iLowerLevelNodeId, bool bIsInterEdge, int iZeroLevelNodeId, int iOwnId)
		: m_iClusterId(iClusterId)
		, m_fWeight(fWeight)
		, m_iLowerLevelNodeId(iLowerLevelNodeId)
		, m_bIsInterEdge(bIsInterEdge)
		, m_iZeroLevelNodeId(iZeroLevelNodeId)
		, m_iOwnId(iOwnId)
	{
	}


	float   GetWeight() { return m_fWeight; }
	int	    GetLowerLevelNodeId() { return m_iLowerLevelNodeId; }
	void	SetClusterId(int iClusterId) { m_iClusterId = iClusterId; }
	int		GetClusterId() { return m_iClusterId; }
	bool	GetIsInterEdge() { return m_bIsInterEdge; }
	int		GetZeroLevelNodeId() { return m_iZeroLevelNodeId; }
	int		GetOwnId() { return m_iOwnId; }
private:
	int		m_iLowerLevelNodeId;
	int		m_iZeroLevelNodeId;
	float	m_fWeight;
	int		m_iClusterId;
	bool	m_bIsInterEdge;
	
	//TODO powiazanie z m_iOwnId z CHierarchicalEdge
	int		m_iOwnId;
};

