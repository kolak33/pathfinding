#include "stdafx.h"
#include "CExperimentsHandler.h"


//CExperimentsHandler::CExperimentsHandler()
//{
//}
//
//
//CExperimentsHandler::~CExperimentsHandler()
//{
//}


void CExperimentsHandler::DoDijkstraExperiments(int iNumExperiments)
{
	for (int i = 0; i < iNumExperiments; ++i)
	{
		m_Dijkstra.SearchPathTest();
		m_StatisticCollection.push_back(m_Dijkstra.GetStatistics());
	}
}