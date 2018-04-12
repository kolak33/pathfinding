#pragma once
#include <ostream>
#include <map>
#include <algorithm>
#include <string>

class CStatistics
{
public:
	CStatistics(std::string &algorithmName)
	{
		m_algName = algorithmName;
	}

	CStatistics()
	{
		m_algName = std::string("algNameNotSet");
	}

	~CStatistics()
	{
	}

	void	SetSearchTimeClock(double dSearchTime, std::string statName = " searchTimeClock") { m_stats[m_algName + statName] = dSearchTime; }
	void	SetSearchTimeChrono(double dSearchTime, std::string statName = " searchTime") { m_stats[m_algName + statName] = dSearchTime; }
	void	SetPathLength(double dPathLength, std::string statName = " pathLength") { m_stats[m_algName + statName] = dPathLength; }
	void	SetNodesExpanded(long lNodesExpanded, std::string statName = " nodesExpanded") { m_stats[m_algName + statName] = lNodesExpanded; }
	void	SetNodesVisited(long lNodesVisited, std::string statName = " nodesVisited") { m_stats[m_algName + statName] = lNodesVisited; }
	void	SetOpenSetMaxSize(long lSize, std::string statName = " openSetMaxSize") { m_stats[m_algName + statName] = lSize; }

	auto& GetStats() { return m_stats; }
	//auto& GetExpNodes() { return m_stats.find("nodesExpanded")->second; }
	//friend std::ostream& operator<<(std::ostream& os, CStatistics& stats);

	void SetHPAStats(std::map<std::string, double> stats)
	{
		m_stats = stats;
	}
	void SetAlgName(std::string strAlgName)
	{
		m_algName = strAlgName;
	}

private:
	std::map<std::string, double> m_stats;
	std::string m_algName;
};