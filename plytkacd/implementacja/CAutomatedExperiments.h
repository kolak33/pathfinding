#pragma once

#include "CExperimentsHandler.h"
#include "CStatisticsSummary.h"


class CAutomatedExperiments
{
public:
	CAutomatedExperiments();
	~CAutomatedExperiments();


	void DoExperiments();
	void DoSingleExperiment();
	void SaveSingleExpToFile(CExperimentsHandler exp, std::string &fileName);
	void FindFileNamesInDirectory();
	void FindPathForMapDraw();
	void DrawMap();

private:
	CStatisticsSummary m_ExperimentResults;

	std::vector<std::string> m_strFileNames;

	CExperimentsHandler m_singleMapExperiment;
	
	//CExperimentsHandler m_mapHandler;
};

