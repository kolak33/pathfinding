#pragma once
#include "CStatistics.h"
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include "Config.h"

class CStatisticsSummary
{
public:
	CStatisticsSummary(bool bAutomatedTests = false)
		: m_lStatsCount(0)
		, m_bSearchTimeInMS(bAutomatedTests)
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
			if (m_bSearchTimeInMS == false && iter.first.find(std::string("searchTime")) != std::string::npos)
				std::cout << iter.first << " " << iter.second / (double)m_lStatsCount / NANO_TO_MILI_FACTOR << std::endl;
			else
				std::cout << iter.first << " " << iter.second / (double)m_lStatsCount << std::endl;
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

	void AddAvgStats(std::map<std::string, double> &stats)
	{
		for (auto &iter : stats)
		{
			m_avgStats[iter.first] += iter.second;
		}
	}

	void IncreaseAvgStatsCount()
	{
		++m_lStatsCount;
	}

	//used in automated experiments, stores search time in ms
	auto& GetAvgStats()
	{
		if (m_bSearchTimeInMS == false)
		{
			for (auto &iter : m_avgStats)
			{
				if (iter.first.find(std::string("searchTime")) != std::string::npos)
					iter.second = iter.second / m_lStatsCount / NANO_TO_MILI_FACTOR;
				else
					iter.second = iter.second / m_lStatsCount;
			}
			m_bSearchTimeInMS = true;
		}

		return m_avgStats;
	}
private:
	std::vector<CStatistics> m_stats;
	std::map<std::string, double> m_avgStats;

	long m_lStatsCount; //in case i don't want to store all statistics in vector
	bool m_bSearchTimeInMS;
};

