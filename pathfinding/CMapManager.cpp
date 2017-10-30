#include "stdafx.h"
#include "CMapManager.h"
#include "Config.h"
#include <fstream>
#include <string>
#include <gl\freeglut.h>

CMapManager::CMapManager(): m_iMapHeight(-1), m_iMapWidth(-1)
{
	m_TerrainMapInfo['.'] = std::make_pair(CellTypePassableTerrain, GridTypePassable);
	m_TerrainMapInfo['G'] = std::make_pair(CellTypePassableTerrain, GridTypePassable);
	m_TerrainMapInfo['S'] = std::make_pair(CellTypeShallowWater, GridTypePassable);

	m_TerrainMapInfo['@'] = std::make_pair(CellTypeOutOfBounds, GridTypeImpassable);
	m_TerrainMapInfo['O'] = std::make_pair(CellTypeOutOfBounds, GridTypeImpassable);
	m_TerrainMapInfo['T'] = std::make_pair(CellTypeTrees, GridTypeImpassable);
	m_TerrainMapInfo['W'] = std::make_pair(CellTypeWater, GridTypeImpassable);
}

CMapManager::~CMapManager()
{
}

GridTypeEnum CMapManager::GetGridTypeByChar(char chCell)
{
	auto it = m_TerrainMapInfo.find(chCell);
	return (it == m_TerrainMapInfo.end() ? GridTypeNone : (it->second.second));
}

void CMapManager::DrawMap()
{
	glClear(GL_COLOR_BUFFER_BIT);
	//double offset[3];
	//offset[0] = 0.1; offset[1] = 0.3; offset[2] = 0.5;
	//glutSolidSierpinskiSponge(5, offset, 0.5);
	//glutSolidTeapot(.6);

	int quadCount = max(m_iMapHeight, m_iMapWidth);
	float quadSize = 2.0f / static_cast<float>(1.1f * quadCount);

	glBegin(GL_QUADS);
	//glColor3f(0.5f, 0.2f, 0.8f);

	for (int x = 0; x < m_iMapHeight; ++x)
	{
		float yPos = 0.9f - x * quadSize;

		for (int y = 0; y < m_iMapWidth; ++y)
		{
			float xPos = -0.9f + y * quadSize;
			if(m_GridMap[x][y].GetGridTypeEnum() == GridTypePassable)
				glColor3f(0.5f, 0.2f, 0.8f); //PURPLE
			else
				glColor3f(1.0f, 0.0f, 0.0f); //RED

			/*glVertex2f(xPos, yPos);
			glVertex2f(xPos + quadSize, yPos);
			glVertex2f(xPos + quadSize, yPos + quadSize);
			glVertex2f(xPos, yPos + quadSize);*/

			CCoords &coords = m_GridMap[x][y].GetCoords();

			glVertex2f(coords.x, coords.y);
			glVertex2f(coords.x + quadSize, coords.y);
			glVertex2f(coords.x + quadSize, coords.y + quadSize);
			glVertex2f(coords.x, coords.y + quadSize);
		}
	}


	glEnd();
	//glFlush();
}

void CMapManager::DrawShortestPath(std::vector<int> &vec)
{
	glBegin(GL_QUADS);
	int quadCount = max(m_iMapHeight, m_iMapWidth);
	float quadSize = 2.0f / static_cast<float>(1.1f * quadCount);
	
	for (int i = 0; i < vec.size(); ++i)
	{
		if(i==0 || i == vec.size() - 1)
			glColor3f(1.0f, 1.0f, 1.0f); //BLACK
		else
			glColor3f(0.0f, 1.0f, 0.0f); //GREEN

		int iRow, iCol;
		iRow = floor(vec[i] / m_iMapWidth);
		iCol = vec[i] - iRow * m_iMapWidth;

		CCoords &coords = m_GridMap[iRow][iCol].GetCoords();

		glVertex2f(coords.x, coords.y);
		glVertex2f(coords.x + quadSize, coords.y);
		glVertex2f(coords.x + quadSize, coords.y + quadSize);
		glVertex2f(coords.x, coords.y + quadSize);
	}

	glEnd();
	glFlush();
}

void CMapManager::InitMapFromFile()
{
	//to do assert octile type
	CString strMapName = "\\dao\\den001d.map";
	CString strMapLocationPath = strLoadingFilePrefix + strMapName;
	std::ifstream mapFile;
	mapFile.open(strMapLocationPath);

	ATLASSERT(mapFile.is_open() == true);
	if (mapFile.is_open())
	{
		std::string strMapType, strTemp;
		//int iMapHeight, iMapWidth;

		mapFile >> strTemp >> strMapType; // type octile
		mapFile >> strTemp >> m_iMapHeight;
		mapFile >> strTemp >> m_iMapWidth;
		mapFile >> strTemp;
		ATLASSERT(strMapType == "octile");
		
		m_iMapNodesCount = m_iMapHeight * m_iMapWidth;
		m_GridMap.resize(m_iMapHeight);
		for (int i = 0; i < m_iMapHeight; ++i)
		{
			m_GridMap[i].reserve(m_iMapWidth);
		}

		//TODO w jednym miejscu quadSize obliczac, moze zapisac jako zmienna statyczna
		int quadCount = max(m_iMapHeight, m_iMapWidth);
		float quadSize = 2.0f / static_cast<float>(1.1f * quadCount);

		for (int i = 0, id = 0; i < m_iMapHeight; ++i)
		{
			float yPos = 0.9f - i * quadSize;
			for (int j = 0; j < m_iMapWidth; ++j, ++id)
			{
				float xPos = -0.9f + j * quadSize;
				char chCell;
				CGridTile gridTile;
				gridTile.SetGridId(id);
				CCoords coords(xPos, yPos);
				gridTile.SetCoords(coords);

	
				mapFile >> chCell;
				GridTypeEnum gridType = GetGridTypeByChar(chCell);
				ATLASSERT(gridType != GridTypeNone);
				gridTile.SetGridTypeEnum(gridType);

				m_GridMap[i].push_back(gridTile);
			}
		}
	}
}