#include "stdafx.h"
#include "CAutomatedExperiments.h"
#include <boost\filesystem.hpp>
#include <boost\regex.hpp>
#include <regex>
#include "CExperimentsHandler.h"
#include "CStatistics.h"
#include <iostream>

CAutomatedExperiments::CAutomatedExperiments()
	: m_ExperimentResults(true)
	, m_singleMapExperiment(std::string(singleExpMapPath), singleExpTestPath, 1.7)
{
	//DoExperiments();
	DoSingleExperiment();
}


CAutomatedExperiments::~CAutomatedExperiments()
{
}

void CAutomatedExperiments::DrawMap()
{
	m_singleMapExperiment.GetHPAStar().DrawMap();
}

void CAutomatedExperiments::DoSingleExperiment()
{
	m_singleMapExperiment.DoTestSuiteExperiments();
}

void CAutomatedExperiments::FindFileNamesInDirectory()
{
	std::string strTestMapSuite = strTestsLocationPrefix + strMapName;
	strTestMapSuite = std::regex_replace(strTestMapSuite, std::regex(".map"), ".txt");

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
				m_strFileNames.push_back(file.path().string());
		}
	}
	catch (std::exception& e) {
		std::cout << e.what() << '\n';
	}
}

void CAutomatedExperiments::SaveSingleExpToFile(CExperimentsHandler exp, std::string &fileName)
{
	std::ofstream resultFile(strResultsPathOdchylenie.c_str(), std::ofstream::app);

	if (resultFile.is_open())
	{
		//resultFile << fileName << "\n";
		//m_ExperimentResults.WriteStatsToFile(resultFile);
		auto avgStats = exp.GetStatsAStar().GetAvgStats();//m_ExperimentResults.GetAvgStats();
		for (auto iter : avgStats)
			resultFile << iter.first << " " << iter.second << "\n";

		avgStats = exp.GetStatsHPAStar().GetAvgStats();//m_ExperimentResults.GetAvgStats();
		for (auto iter : avgStats)
			resultFile << iter.first << " " << iter.second << "\n";

		avgStats = exp.GetStatsJPSearch().GetAvgStats();//m_ExperimentResults.GetAvgStats();
		for (auto iter : avgStats)
			resultFile << iter.first << " " << iter.second << "\n";

		avgStats = exp.GetStatsFringe().GetAvgStats();//m_ExperimentResults.GetAvgStats();
		for (auto iter : avgStats)
			resultFile << iter.first << " " << iter.second << "\n";

		resultFile.close();
	}
	else
	{
		std::cout << "nie mozna zapisac wynikow" << std::endl;
	}
}

void CAutomatedExperiments::DoExperiments()
{
	FindFileNamesInDirectory();

	//empty tmp file
	std::ofstream resultFileTmp(strResultsPathOdchylenie.c_str());
	resultFileTmp.close();

	//int iMaxTest = 20;
	double heurFactor = 1.7;
	//for (int iTest = 0; iTest < iMaxTest; ++iTest)
	//{
		int i = 0;
		long lAllMemoryUsage = 0;
		long lBasicMapMemUsage = 0;
		bool bOnce = true;
		for (auto strMapPath : m_strFileNames)
		{
			if (strMapPath.find(".map") != std::string::npos) //it's a map test file
			{
				const std::string singleslash("\\\\");
				const std::string doubleslash("\\\\\\\\");
				std::string mapLocEscaped = boost::regex_replace(strMapLocationPrefix, boost::regex(singleslash), doubleslash);
				std::string strTestPath = boost::regex_replace(strMapPath, boost::regex(mapLocEscaped), strTestsLocationPrefix);

				CExperimentsHandler singleMapExperiment(strMapPath, strTestPath, heurFactor);
				singleMapExperiment.DoTestSuiteExperiments();
				SaveSingleExpToFile(singleMapExperiment, strMapPath);

				m_ExperimentResults.AddAvgStats(singleMapExperiment.GetStatsAStar().GetAvgStats());
				m_ExperimentResults.AddAvgStats(singleMapExperiment.GetStatsFringe().GetAvgStats());
				m_ExperimentResults.AddAvgStats(singleMapExperiment.GetStatsJPSearch().GetAvgStats());
				m_ExperimentResults.AddAvgStats(singleMapExperiment.GetStatsHPAStar().GetAvgStats());
				m_ExperimentResults.IncreaseAvgStatsCount();

				lAllMemoryUsage += singleMapExperiment.GetHPAStar().GetMemoryUsage();

				if (bOnce)
				{
					auto &graph = singleMapExperiment.GetMapManager().GetGraph();
					for (auto edge : graph)
					{
						auto &neighbours = edge.GetNeighbours();
						lBasicMapMemUsage += neighbours.size() * 12; // neighbours upper size

						lBasicMapMemUsage += 28; // edge size
					}


					bOnce = false;
				}

				i++;
				std::cout << "done: " << i << std::endl;
				//if (i == 4)
				//	break;
			}
		}
		std::cout << "\n\n" << std::endl;
		m_ExperimentResults.PrintAvgStats();

		

		std::ofstream resultFile(strResultsPath.c_str(), std::ofstream::app);

		if (resultFile.is_open())
		{
			//m_ExperimentResults.WriteStatsToFile(resultFile);
			auto avgStats = m_ExperimentResults.GetAvgStats();
			resultFile << heurFactor << "\n";
			for (auto iter : avgStats)
			{
				if (iter.first == "HPAStar openSetMaxSize")
					resultFile << iter.first << " " << iter.second / m_ExperimentResults.GetStatsCount() << "\n";
				else
					resultFile << iter.first << " " << iter.second << "\n";
			}
			resultFile << "HPA avg mem usage" << lAllMemoryUsage / (m_strFileNames.size() / 2) << "\n";
			resultFile << "Map basic graph mem usage" << lBasicMapMemUsage << "\n";
			resultFile << "\n";
			resultFile.close();
		}
		else
		{
			std::cout << "nie mozna zapisac wynikow" << std::endl;
		}

	//	m_ExperimentResults.ClearStats();
	//	heurFactor += 0.05;
	//}
}


void CAutomatedExperiments::FindPathForMapDraw()
{


}