#pragma once
#include <atldef.h>
#include <string>

typedef enum GridTypeEnum {
	GridTypeNone = 0,
	GridTypePassable = 1,
	GridTypeImpassable = 2,
}GridTypeEnum;

typedef enum MapCellType {
	CellTypeNone = 1,
	CellTypePassableTerrain = 2,
	CellTypeOutOfBounds = 3,
	CellTypeTrees = 4,
	CellTypeWall = 5,
	CellTypeWater = 6,
	CellTypeShallowWater = 7,
}MapCellType;

 const std::string strMapLocationPrefix = "..\\mapki\\dao\\";
 const std::string strTestsLocationPrefix = "..\\\\TESTS\\\\";

 const std::string strMapName = "orz103d.map"; //"den400d.map"; //
 const int iGenerateTestCount = 250;

 const float DIAG_HEUR_VAL = 0.4142f;
 const float DIAGONAL_WEIGHT = 1.4142f;
 const float STRAIGHT_WEIGHT = 1.0f;
 const float INTER_EDGE_WEIGHT = 1.0f;
 const int GRID_BRANCHING_FACTOR = 8;

 const double NANO_TO_MILI_FACTOR = 1000000.0;