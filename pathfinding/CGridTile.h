#pragma once
#include "Config.h"


class CCoords
{
public:
	CCoords() : x(0.0), y(0.0)
	{
	}

	CCoords(float x, float y) : x(x), y(y)
	{
	}

	float x, y;
};

class CGridTile
{
public:
	CGridTile();

	~CGridTile();

	void SetGridTypeEnum(GridTypeEnum gridTypeEnum) { m_gridTypeEnum = gridTypeEnum; }
	GridTypeEnum GetGridTypeEnum() { return m_gridTypeEnum; }

	void SetCoords(CCoords coords) { m_fCoords = coords; }
	CCoords& GetCoords() { return m_fCoords; }

	void SetGridId(int id) { m_iGridId = id; }
	int GetGridId() { return m_iGridId; }
	

private:
	GridTypeEnum m_gridTypeEnum;
	int			 m_iGridId;
	CCoords		 m_fCoords;


	//TODO tymczasowe
public:
	int m_iXPos;
	int m_iYPos;
};

