#pragma once
#include "CMapManager.h"
#include "CDijkstra.h"

class CExperimentsHandler
{
public:
	CExperimentsHandler()
	{
		m_Dijkstra.SetMapManagerPtr(&m_MapManager);
		m_Dijkstra.SearchPathTest();
	}
	~CExperimentsHandler()
	{
	}

	auto& GetMapManager() { return m_MapManager; }
	auto& GetDijkstra() { return m_Dijkstra; }

private:
	CMapManager m_MapManager;
	CDijkstra	m_Dijkstra;

};

