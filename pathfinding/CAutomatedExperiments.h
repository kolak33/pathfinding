#pragma once

#include "CExperimentsHandler.h"
#include "CStatisticsSummary.h"


class CAutomatedExperiments
{
public:
	CAutomatedExperiments();
	~CAutomatedExperiments();


	void DoExperiments();

private:
	CStatisticsSummary m_ExperimentResults;
};

