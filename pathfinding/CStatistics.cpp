#include "stdafx.h"
#include "CStatistics.h"


std::ostream& operator<<(std::ostream& os, CStatistics& stats)
{
	std::for_each(stats.GetStats().begin(), stats.GetStats().end(), [](std::map <std::string, double>::value_type &iter) {
		std::cout << iter.first << " " << iter.second << std::endl;
	});
	return os;
}