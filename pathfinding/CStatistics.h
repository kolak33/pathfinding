#pragma once
#include <ostream>
#include <map>
#include <algorithm>
#include <string>

class CStatistics
{
public:
	CStatistics()
	{
	}

	~CStatistics()
	{
	}

	void	SetSearchTimeClock(double dSearchTime) { m_stats["searchTimeClock"] = dSearchTime; }
	void	SetSearchTimeChrono(double dSearchTime) { m_stats["searchTimeChrono"] = dSearchTime; }
	void	SetPathLength(double dPathLength) { m_stats["pathLength"] = dPathLength; }
	void	SetNodesExpanded(long lNodesExpanded) { m_stats["nodesExpanded"] = lNodesExpanded; }
	void	SetNodesVisited(long lNodesVisited) { m_stats["nodesVisited"] = lNodesVisited; }
	void	SetOpenSetMaxSize(long lSize) { m_stats["openSetMaxSize"] = lSize; }

	auto& GetStats() { return m_stats; }
	friend std::ostream& operator<<(std::ostream& os, CStatistics& stats);

private:
	std::map<std::string, double> m_stats;
};