#pragma once
#include "CStatistics.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>

class CStatisticsSummary
{
public:
	CStatisticsSummary()
		: m_lStatsCount(0)
	{
	}
	~CStatisticsSummary()
	{
	}

	void AddStats(CStatistics &st)
	{
		auto stats =  st.GetStats();
		for (auto &iter : stats)
		{
			m_avgStats[iter.first] += iter.second;
		}
		m_stats.push_back(st);
		++m_lStatsCount;
	}

	void PrintAvgStats()
	{
		for (auto &iter : m_avgStats)
		{
			std::cout << iter.first << " " << iter.second / m_lStatsCount << std::endl;
		}
	}

	void PrintAllStats()
	{
		for (int i=0; i<m_stats.size(); ++i)
		{
			std::cout << m_stats[i] << std::endl;
		}
	}

	void ClearStats()
	{
		m_lStatsCount = 0;
		m_stats.clear();
		m_avgStats.clear();
	}
private:
	std::vector<CStatistics> m_stats;
	std::map<std::string, double> m_avgStats;

	long m_lStatsCount; //in case i don't want to store all statistics in vector
};

