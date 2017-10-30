#pragma once

#include "atlstr.h"

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
	CellTypeWall = 3,
	CellTypeWater = 4,
	CellTypeShallowWater = 5,
}MapCellType;

 const CString strLoadingFilePrefix = "..\\mapki";

 const float DIAGONAL_WEIGHT = 1.4142f;
 const float STRAIGHT_WEIGHT = 1.0f;