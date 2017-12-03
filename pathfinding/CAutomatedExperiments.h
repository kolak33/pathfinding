#pragma once

#include "CExperimentsHandler.h"
#include "CStatisticsSummary.h"


class CAutomatedExperiments
{
public:
	CAutomatedExperiments();
	~CAutomatedExperiments();


	void DoExperiments();
	void FindPathForMapDraw();

private:
	CStatisticsSummary m_ExperimentResults;

	//CExperimentsHandler m_mapHandler;
};

