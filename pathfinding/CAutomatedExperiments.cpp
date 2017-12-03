#include "stdafx.h"
#include "CAutomatedExperiments.h"
#include <boost\filesystem.hpp>
#include <boost\regex.hpp>
#include <regex>
#include <fstream>
#include "CExperimentsHandler.h"
#include "CStatistics.h"

CAutomatedExperiments::CAutomatedExperiments()
	: m_ExperimentResults(true)
{
	DoExperiments();
}


CAutomatedExperiments::~CAutomatedExperiments()
{
}

void CAutomatedExperiments::DoExperiments()
{
	std::string strTestMapSuite = strTestsLocationPrefix + strMapName;
	strTestMapSuite = std::regex_replace(strTestMapSuite, std::regex(".map"), ".txt");


	std::vector<std::string> strFileNames;
	namespace fs = boost::filesystem;
	try
	{
		fs::path p(strMapLocationPrefix);
		if (!exists(p) || !is_directory(p)) {
			std::cout << p << " is not a path\n";
		}
		else
		{
			fs::recursive_directory_iterator begin(p), end;
			std::vector<fs::directory_entry> files(begin, end);

			for (auto file : files)
				strFileNames.push_back(file.path().string());
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << '\n';
	}

	int i = 4;
	for (auto strMapPath : strFileNames)
	{
		if (strMapPath.find(".map") != std::string::npos) //it's a map test file
		{
			const std::string singleslash("\\\\");
			const std::string doubleslash("\\\\\\\\");
			std::string mapLocEscaped = boost::regex_replace(strMapLocationPrefix, boost::regex(singleslash), doubleslash);
			std::string strTestPath = boost::regex_replace(strMapPath, boost::regex(mapLocEscaped), strTestsLocationPrefix);

			CExperimentsHandler singleMapExperiment(strMapPath, strTestPath);

			
			m_ExperimentResults.AddAvgStats(singleMapExperiment.GetStatsAStar().GetAvgStats());
			m_ExperimentResults.AddAvgStats(singleMapExperiment.GetStatsFringe().GetAvgStats());
			m_ExperimentResults.AddAvgStats(singleMapExperiment.GetStatsJPSearch().GetAvgStats());
			m_ExperimentResults.AddAvgStats(singleMapExperiment.GetStatsHPAStar().GetAvgStats());
			m_ExperimentResults.IncreaseAvgStatsCount();

			i++;
			if (i == 10)
				break;
		}
	}

	m_ExperimentResults.PrintAvgStats();
}