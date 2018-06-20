#ifndef Floyd_20161228_H
#define Floyd_20161228_H

#include <map>
#include <vector>
#include <iostream>
#include"PathSearch.h"
#include"singleton.hpp"
#define INVALID_DISTANCE 1000000
#define INVALID_PATH_INDEX -1


typedef  int( *FinishLevelCB )( const unsigned int iPercent, const void *pUsr );

class Floyd
{
public:
	Floyd();
	~Floyd();
	//friend class nsp::toolkit::singleton<Floyd>;

private:
	std::vector<PathSearch_Wop> m_vctWops;
	std::map<unsigned int/*link_id*/, PathSearch_Link> m_mapLinks;

	//std::map<unsigned int/* from_wop_index*/, std::map<unsigned int/*to_wop_index*/, float/*distance*/>> m_mapWopDistances;
	//std::map<unsigned int/* from_wop_index*/, std::map<unsigned int/*to_wop_index*/, int/* path_wop_index*/>> m_mapWopPaths;

	//optimize
	float** m_WopDistances;
	int m_WopDistancesLength = 0;
	int** m_WopPaths;
	int m_WopPathsLength = 0;


	PathSearch_Wop stInvalidWop;

	std::map<unsigned int/*edge_id*/, PathSearch_EdgeInfo> m_mapEdges;
	std::map<unsigned int/*wop_id*/, PathSearch_Wop> m_mapWopType;
	int m_iFloydId;
	unsigned int m_iSearchType;

	struct FloydDate{
		int iStartIndex;
		int iEndIndex;
		int iPathIndex;
		float fDistance;
	};

private:
	int InsertPath( std::vector<unsigned int>&  vctWopPath );
	int GetStartAndEndWop( const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl,
		                   int& startWopIndex, int& endWopIndex,
		                   float& fDistanceStart, float& fDistanceEnd );
	int IsSameWop( const PathSearch_Wop stWop1, const PathSearch_Wop stWop2 );
	int SameWopPathSearch( const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl,
		                   const unsigned int iSameWopIndex,
						   const float fDistanceStart, const float fDistanceEnd,
		                   std::vector<unsigned int>& vctWopPath, float& fDistance );
	int SameWopInsideSearch( const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl,
		                     const unsigned int iSameWopIndex,
		                     std::vector<unsigned int>&  vctWopPath, float& fDistance );
	int SameWopOutsideSearch( const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl,
		                      const unsigned int iSameWopIndex,
		                      const float fDistanceStart,  const float fDistanceEnd,
		                      std::vector<unsigned int>&  vctWopPath, float& fDistance );
	
	
public:
	int GetFloydId();
	int SetFloydId(int iFloydId);
	int SetSearchType(unsigned int iSearchType);

public:
	int LoadData( const PathSearch_LayoutData* pLayoutData );
	int SetWops( const PathSearch_Wop* pWops, const unsigned int iLength );
	int SetLinks( const  PathSearch_Link* pLinks, const unsigned int iLength );
	int SetEdges( const PathSearch_EdgeInfo* pEdges, const unsigned int iLength );

	int StartFolyd(const char* pFloydPath, FinishLevelCB FinishLevelCB, void* pUsr);
	int StartFloydPath(FinishLevelCB FinishLevelCB, void* pUsr);
	int GetPath( const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance );	
	int GetPathByPos( const PathSearch_Position stStartPos, const PathSearch_Position stEndPos, PathSearch_Wop** path, int* iLength, float* fDistance );
	int GetPathByUpl( const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance );
	int GetPathBySPosAndEUpl( const PathSearch_Position stStartPos, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance );
	int WriteFloydData(float**& MatrixWopDistances, int**& MatrixWopPaths, FloydDate*date);
	int ReadFLoydData(FloydDate*date, int iCount, float**& MatrixWopDistances, int**& MatrixWopPaths);
	void Releasepath( PathSearch_Wop * path );	
};

#endif

