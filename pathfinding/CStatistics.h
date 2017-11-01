#pragma once


class CStatistics
{
public:
	CStatistics()
		: m_dPathLength(-1)
		, m_dSearchTimeChrono(-1.0)
		, m_dSearchTimeClock(-1.0)
		, m_lNodesExpanded(-1)
		, m_lNodesVisited(-1)
		, m_lOpenSetMaxSize(-1)
	{
	}

	~CStatistics()
	{
	}

	void	SetSearchTimeClock(double dSearchTime) { m_dSearchTimeClock = dSearchTime; }
	void	SetSearchTimeChrono(double dSearchTime) { m_dSearchTimeChrono = dSearchTime; }
	void	SetPathLength(double dPathLength) { m_dPathLength = dPathLength; }
	void	SetNodesExpanded(long lNodesExpanded) { m_lNodesExpanded = lNodesExpanded; }
	void	SetNodesVisited(long lNodesVisited) { m_lNodesVisited = lNodesVisited; }
	void	SetOpenSetMaxSize(long lSize) { m_lOpenSetMaxSize = lSize; }

	double GetSearchTimeClock() { return m_dSearchTimeClock; }
	double GetSearchTimeChrono() { return m_dSearchTimeChrono; }
	double GetPathLength() { return m_dPathLength; }
	long   GetNodesExpanded() { return m_lNodesExpanded; }

private:
	double m_dSearchTimeChrono; //TODO CPU time not elapsed time, need to think about it
	double m_dSearchTimeClock;
	double m_dPathLength;
	long   m_lNodesExpanded;
	long   m_lNodesVisited;
	long   m_lOpenSetMaxSize;
};