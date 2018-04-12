#include "stdafx.h"
#include "CHPAStar.h"

void CHPAStar::Preprocessing()
{
	BuildFirstLevel();
	AddNextLevels();

	auto &graph = m_AllAbstrLevels[0].GetGraph();
	for (auto edge : graph)
	{
		auto &neighbours = edge.GetNeighbours();
		m_lMemoryUsage += neighbours.size() * 24; // neighbours upper size
		
		m_lMemoryUsage += 16; // edge size
	}

	auto& clusters = m_AllAbstrLevels[0].GetClusters();
	for (auto &cluster : clusters)
	{
		m_lMemoryUsage += cluster.GetEntrances().size() * 4 + 24; // clusterId's size + cluster class size
	}
}

void CHPAStar::BuildFirstLevel()
{
	int iFirstLevel = 1;
	CHPAAbstrLevel firstAbstrLevel(iFirstLevel, m_iClustersInGroup, m_GraphLowLevelPtr, m_iFirstLvlClusterWidth, m_iFirstLvlClusterHeight);
	firstAbstrLevel.BuildClusters();
	firstAbstrLevel.ClusterByIdPrepocessing();
	firstAbstrLevel.BuildVerticalEntrances(m_iMaxSingleEntranceLen);
	firstAbstrLevel.BuildHorizontalEntrances(m_iMaxSingleEntranceLen);
	firstAbstrLevel.BuildIntraEdges(firstAbstrLevel.GetGraph());// 'firstAbstrLevel.GetGraph()' to taki zapychacz, zeby tylko przekazac parametr, nie jest wykorzystywany w 1 lvlu

	m_AllAbstrLevels.push_back(firstAbstrLevel);
}

void CHPAStar::AddNextLevels()
{
	for (int iLvl = 2; iLvl <= m_iMaxLevel; ++iLvl)
	{
		CHPAAbstrLevel nextLevel(iLvl, m_iClustersInGroup, m_GraphLowLevelPtr, m_iFirstLvlClusterWidth, m_iFirstLvlClusterHeight);
		nextLevel.ClusterByIdPrepocessing();
		nextLevel.BuildClustersNextLevel(&m_AllAbstrLevels[m_AllAbstrLevels.size() - 1]);
		nextLevel.BuildEntrancesNextLevel(&m_AllAbstrLevels[m_AllAbstrLevels.size() - 1]);
		nextLevel.BuildIntraEdges(m_AllAbstrLevels[m_AllAbstrLevels.size() - 1].GetGraph());

		m_AllAbstrLevels.push_back(nextLevel);
	}
}

void CHPAStar::FindShortestPath(int iStartId, int iGoalId)
{
	m_StatSummarySimpleSearch.ClearStats();
	m_timeAlloc = 0;
	m_timeSimpleRefineSearch = 0;
	m_refineNodesExpanded = 0;

	if (iStartId == iGoalId)
		return;

	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	AddStartAndGoalNodesToGraph(iStartId, iGoalId);
	std::chrono::steady_clock::time_point end1 = std::chrono::steady_clock::now();
	double timeElapsedMS1 = std::chrono::duration_cast<std::chrono::nanoseconds>(end1 - begin).count();
	m_StatAddStart.SetSearchTimeChrono(timeElapsedMS1);


	if (m_iMaxStartLvl == 0) //situation when two nodes would end up being added in same cluster //TODO sprawdzic to
	{
		m_AStar.FindShortestPath(iStartId, iGoalId);
		m_shortestPath = m_AStar.GetShortestPath();

		auto& stats = m_AStar.GetStatistics();
		m_StatSummarySimpleSearch.AddStats(stats);
	}
	else
	{
		auto &edgePos = m_AllAbstrLevels[m_iMaxStartLvl - 1].GetEdgePositions();

		std::chrono::steady_clock::time_point begin2 = std::chrono::steady_clock::now();
		m_AStar.FindShortestPathAbstract(iStartId, iGoalId, m_AllAbstrLevels[m_iMaxStartLvl - 1].GetGraph(), edgePos[iStartId], edgePos[iGoalId]);
		std::chrono::steady_clock::time_point end2 = std::chrono::steady_clock::now();
		double timeElapsedMS2 = std::chrono::duration_cast<std::chrono::nanoseconds>(end2 - begin2).count();
		m_SearchAtFirstAbstractLvl.SetSearchTimeChrono(timeElapsedMS2);

		auto& stats = m_AStar.GetStatistics();
		m_StatSummarySimpleSearch.AddStats(stats);

		ATLASSERT(m_AStar.GetPathFound());
		m_shortestPath = m_AStar.GetShortestPath();
		m_pathEntr = m_shortestPath;
		std::reverse(m_shortestPath.begin(), m_shortestPath.end());
		std::vector<int> resultPath;
		for (int iLvl = m_iMaxStartLvl - 1; iLvl >= 0; --iLvl)
		{
			RefinePath(m_AStar, m_shortestPath, iLvl, resultPath);
			m_shortestPath = resultPath;
		}
	}
	m_StatTimeAlloc.SetSearchTimeChrono(m_timeAlloc);

	RemoveStartAndGoalNodesFromGraph(iStartId, iGoalId);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	double timeElapsedMS = std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
	
	m_Statistics.SetHPAStats(m_StatSummarySimpleSearch.GetNotAveragedStats());
	m_Statistics.SetSearchTimeChrono(timeElapsedMS); //overwrite timeElapsed
}

void CHPAStar::RefinePath(CAstar &AStar, std::vector<int> &path, int iLvl, std::vector<int> &resultPath)
{
	resultPath.clear();
	resultPath.push_back(path[0]); //push iStart

	for (int iIter = 0; iIter < path.size() - 1; ++iIter)
	{
		if (m_AllAbstrLevels[iLvl].CalcClusterId(path[iIter]) == m_AllAbstrLevels[iLvl].CalcClusterId(path[iIter + 1])) // if same cluster on current level, otherwise entrance crossing
		{
			if (iLvl == 0) // final refinement to low lvl
			{
				std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
				int iStartClusterId = m_AllAbstrLevels[iLvl].CalcClusterId(path[iIter]);
				CCluster *clusterPtr = &m_AllAbstrLevels[iLvl].GetClusters()[iStartClusterId];
				AStar.FindShortestPath(path[iIter], path[iIter + 1], true, clusterPtr);
				m_timeAlloc += AStar.m_timeAllocation;
				
				std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
				m_timeSimpleRefineSearch += std::chrono::duration_cast<std::chrono::nanoseconds>(end - begin).count();
			}
			else
			{
				auto &edgePos = m_AllAbstrLevels[iLvl - 1].GetEdgePositions();
				AStar.FindShortestPathAbstract(path[iIter], path[iIter + 1], m_AllAbstrLevels[iLvl - 1].GetGraph(), edgePos[path[iIter]], edgePos[path[iIter+1]]); // find path on lower lvl
			}

			auto& stats = AStar.GetStatistics();
			m_StatSummarySimpleSearch.AddStats(stats);

			ATLASSERT(AStar.GetPathFound());
			auto &tmpResult = AStar.GetShortestPath();
			for (int iLowerIter = tmpResult.size() - 1; iLowerIter >= 0 ; --iLowerIter) // shortestPath is reversed
			{
				if (tmpResult[iLowerIter] != path[0] && tmpResult[iLowerIter] != path[path.size() - 1]) // nor start nor goal id
					resultPath.push_back(tmpResult[iLowerIter]);
			}
		}
		m_StatSummarySimpleSearch.AddHPAEntranceLength();
	}
	resultPath.push_back(path[path.size() - 1]);
}

void CHPAStar::AddStartAndGoalNodesToGraph(int iStartId, int iGoalId)
{
	bool bAddedToLevel;
	m_iMaxStartLvl = 0;
	for (int iLvl = 1; iLvl <= m_iMaxLevel; ++iLvl)
	{
		auto &graphLowerLevel = iLvl - 2 < 0 ? m_AllAbstrLevels[0].GetGraph() : m_AllAbstrLevels[iLvl - 2].GetGraph(); //jezeli lower lvl nie istnieje to wyslij smieci

		if(iLvl >= 2)
			m_AllAbstrLevels[iLvl - 1].SetEdgePosLowerLvl(&(m_AllAbstrLevels[iLvl - 2].GetEdgePositions()));

		bAddedToLevel = m_AllAbstrLevels[iLvl - 1].AddStartAndGoalNodesToGraph(m_AStar, iStartId, iGoalId, graphLowerLevel);
		if (bAddedToLevel)
			m_iMaxStartLvl = iLvl;
	}
}

void CHPAStar::RemoveStartAndGoalNodesFromGraph(int iStartId, int iGoalId)
{
	for (int iLvl = m_iMaxStartLvl; iLvl >= 1; --iLvl)
	{
		m_AllAbstrLevels[iLvl - 1].DeleteStartAndGoalNodesFromGraph(iStartId, iGoalId);
	}
}