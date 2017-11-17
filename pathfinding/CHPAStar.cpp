#include "stdafx.h"
#include "CHPAStar.h"

void CHPAStar::Preprocessing()
{
	BuildFirstLevel();
	AddNextLevels();
}

void CHPAStar::BuildFirstLevel()
{
	int iFirstLevel = 1;
	CHPAAbstrLevel firstAbstrLevel(iFirstLevel, m_iClustersInGroup, m_GraphLowLevelPtr, m_iFirstLvlClusterWidth, m_iFirstLvlClusterHeight);
	firstAbstrLevel.BuildClusters();
	firstAbstrLevel.BuildVerticalEntrances(m_iMaxSingleEntranceLen);
	firstAbstrLevel.BuildHorizontalEntrances(m_iMaxSingleEntranceLen);
	firstAbstrLevel.BuildIntraEdges(firstAbstrLevel.GetGraph());//TODO 'firstAbstrLevel.GetGraph()' to taki zapychacz, zeby tylko przekazac parametr, nie jest wykorzystywany w 1 lvlu

	m_AllAbstrLevels.push_back(firstAbstrLevel);
}

void CHPAStar::AddNextLevels()
{
	for (int iLvl = 2; iLvl <= m_iMaxLevel; ++iLvl)
	{
		CHPAAbstrLevel nextLevel(iLvl, m_iClustersInGroup, m_GraphLowLevelPtr, m_iFirstLvlClusterWidth, m_iFirstLvlClusterHeight);
		nextLevel.BuildClustersNextLevel(&m_AllAbstrLevels[m_AllAbstrLevels.size() - 1]);
		nextLevel.BuildEntrancesNextLevel(&m_AllAbstrLevels[m_AllAbstrLevels.size() - 1]);
		nextLevel.BuildIntraEdges(m_AllAbstrLevels[m_AllAbstrLevels.size() - 1].GetGraph());

		m_AllAbstrLevels.push_back(nextLevel);
	}

	/*CAstar AStar;
	AStar.SetMapManagerPtr(m_GraphLowLevelPtr);

	m_iStart = AStar.GenerateRandomAccessibleNodeId();
	m_iGoal = AStar.GenerateRandomAccessibleNodeId();

	FindShortestPath(m_iStart, m_iGoal);*/
}

void CHPAStar::FindShortestPath(int iStartId, int iGoalId)
{
	std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();
	AddStartAndGoalNodesToGraph(iStartId, iGoalId);

	if (m_iMaxLvlStartGoalNodes == 0)
	{
		m_AStar.FindShortestPath(iStartId, iGoalId);
		m_shortestPath = m_AStar.GetShortestPath();
	}
	else
	{
		m_AStar.FindShortestPathAbstract(iStartId, iGoalId, m_AllAbstrLevels[m_iMaxLvlStartGoalNodes - 1].GetGraph());
		m_shortestPath = m_AStar.GetShortestPath();
		std::reverse(m_shortestPath.begin(), m_shortestPath.end());
		std::vector<int> resultPath;
		for (int iLvl = m_iMaxLvlStartGoalNodes - 1; iLvl >= 0; --iLvl)
		{
			RefinePath(m_AStar, m_shortestPath, iLvl, resultPath);
			m_shortestPath = resultPath;
		}
	}

	RemoveStartAndGoalNodesFromGraph(iStartId, iStartId);

	std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
	double timeElapsedMS = std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count();
	m_Statistics.SetSearchTimeChrono(timeElapsedMS);
}

void CHPAStar::RefinePath(CAstar &AStar, std::vector<int> &path, int iLvl, std::vector<int> &resultPath)
{
	resultPath.clear();
	resultPath.push_back(path[0]); //push iStart

	for (int iIter = 0; iIter < path.size() - 1; ++iIter)
	{
		if (m_AllAbstrLevels[iLvl].CalcClusterId(path[iIter]) == m_AllAbstrLevels[iLvl].CalcClusterId(path[iIter + 1])) // if same cluster on current level
		{
			if (iLvl == 0) // final refinement to low lvl
				AStar.FindShortestPath(path[iIter], path[iIter + 1]);
			else
				AStar.FindShortestPathAbstract(path[iIter], path[iIter + 1], m_AllAbstrLevels[iLvl - 1].GetGraph()); // find path on lower lvl
			if (!AStar.GetPathFound())
				int a = 5;
			auto &tmpResult = AStar.GetShortestPath();
			for (int iLowerIter = tmpResult.size() - 1; iLowerIter >= 0 ; --iLowerIter) // shortestPath is reversed
			{
				if (tmpResult[iLowerIter] != path[0] && tmpResult[iLowerIter] != path[path.size() - 1]) // nor start nor goal id
					resultPath.push_back(tmpResult[iLowerIter]);
			}
		}
	}

	resultPath.push_back(path[path.size() - 1]);
}

void CHPAStar::AddStartAndGoalNodesToGraph(int iStartId, int iGoalId)
{
	bool bAddedToLevel;
	m_iMaxLvlStartGoalNodes = 0;
	for (int iLvl = 1; iLvl <= m_iMaxLevel; ++iLvl)
	{
		auto &graphLowerLevel = iLvl - 2 < 0 ? m_AllAbstrLevels[0].GetGraph() : m_AllAbstrLevels[iLvl - 2].GetGraph(); //jezeli lower lvl nie istnieje to wyslij smieci
		bAddedToLevel = m_AllAbstrLevels[iLvl - 1].AddStartAndGoalNodesToGraph(m_AStar, iStartId, iGoalId, graphLowerLevel);
		if (bAddedToLevel)
			m_iMaxLvlStartGoalNodes = iLvl;
	}
}

void CHPAStar::RemoveStartAndGoalNodesFromGraph(int iStartId, int iGoalId)
{
	for (int iLvl = m_iMaxLvlStartGoalNodes; iLvl >= 1; --iLvl)
	{
		m_AllAbstrLevels[iLvl - 1].DeleteStartAndGoalNodesFromGraph(iStartId, iGoalId);
	}
}