#include "PathSearch.h"
#include "Floyd.h"
#include "Dijkstra.h"
#include "DataInfo.h"
#include "log.h"
#define MapNumber 16

unsigned int m_nSearchType = PathSearch_FLOYD;
int m_iUserId = -1;
void *SearchFun[MapNumber];
int iSearchType[MapNumber];

EXPS_FUNCTION(int) PathSearch_GetUserId()
{
	m_iUserId++;
	return m_iUserId;
}

EXPS_FUNCTION( void ) PathSearch_SetType( const int iUserId, const PathSearch_Type type )
{
	if( type == PathSearch_FLOYD ) {
		//m_nSearchType = PathSearch_FLOYD;
		SearchFun[iUserId] = new Floyd();
		Floyd* floyd = (Floyd*)SearchFun[iUserId];
		if (floyd){
			floyd->SetFloydId(iUserId);
			floyd->SetSearchType(type);
			iSearchType[iUserId] = PathSearch_FLOYD;
		}

		
	} else {
		//m_nSearchType = PathSearch_DIJKSTRA;
		SearchFun[iUserId]=new Dijkstra();
		Dijkstra *dijkstra = (Dijkstra*)SearchFun[iUserId];
		if (dijkstra){
			dijkstra->SetDijkstraId(iUserId);
			dijkstra->SetSearchType(type);
			iSearchType[iUserId] = PathSearch_DIJKSTRA;
		}

	}
}

EXPS_FUNCTION(int)PathSearch_OpenMap(const char* pFilePath, PathSearch_LayoutData* pLayoutData)
{
	if( !pFilePath || !pLayoutData) {
		loerror("path_search") << "PathSearch_OpenMap: pFilePath is NULL. or pLayoutData is NULL.";
		return -1;
	}
	DataInfo datainfo;
	if (datainfo.OpenMap(pFilePath, pLayoutData) < 0){
		return -1;
	}
	/*if( nsp::toolkit::singleton<DataInfo>::instance()->OpenMap( pFilePath, pLayoutData ) < 0 ) {
		return -1;
	}*/
	return 0;
}

EXPS_FUNCTION(int) PathSearch_LoadData(const int iUserId, const PathSearch_LayoutData* pLayoutData)
{
	if( !pLayoutData ) {
		loerror("path_search") << "PathSearch_LoadData: pLayoutData is NULL.";
		return -1;
	}
	if (!SearchFun[iUserId])
	{
		return -1;
	}

	if (iSearchType[iUserId] == PathSearch_FLOYD) {
		Floyd* floyd = (Floyd*)SearchFun[iUserId];
		if (floyd){
			if (floyd->LoadData(pLayoutData) < 0)
			{
				return -1;
			}
		}

	}
	else {
		Dijkstra*dijkstra = (Dijkstra*)SearchFun[iUserId];
		if (dijkstra){
			if (dijkstra->LoadData(pLayoutData) < 0)
			{
				return -1;
			}
		}
	}
	return 0;
}
 
EXPS_FUNCTION(int) PathSearch_SetWops(const int iUserId, const PathSearch_Wop* pWops, const unsigned int iLength)
{
	if (iSearchType[iUserId] == PathSearch_FLOYD){
		Floyd*floyd = (Floyd*)SearchFun[iUserId];
		if (floyd){
			if (floyd->SetWops(pWops, iLength) < 0){
				return -1;
			}
		}

	}
	else{
		Dijkstra*dijkstra = (Dijkstra*)SearchFun[iUserId];
		if (dijkstra){
			if (dijkstra->SetWops(pWops, iLength) < 0){
				return -1;
			}
		}

	}
	return 0;	
}

EXPS_FUNCTION(int) PathSearch_SetLinks(const int iUserId, const PathSearch_Link* pLinks, const unsigned int iLength)
{
	if (iSearchType[iUserId] == PathSearch_FLOYD){
		Floyd*floyd = (Floyd*)SearchFun[iUserId];
		if (floyd){
			if (floyd->SetLinks(pLinks, iLength) < 0){
				return -1;
			}
		}
		
	}
	else{
		Dijkstra*dijkstra = (Dijkstra*)SearchFun[iUserId];
		if (dijkstra){
			if (dijkstra->SetLinks(pLinks, iLength) < 0){
					return -1;
			}
		}
		
	}
	return 0;
}

EXPS_FUNCTION(int) PathSearch_SetEdges(const int iUserId, const PathSearch_EdgeInfo* pEdges, const unsigned int iLength)
{
	if (iSearchType[iUserId] == PathSearch_FLOYD){
		Floyd*floyd = (Floyd*)SearchFun[iUserId];
		if (floyd){
			if (floyd->SetEdges(pEdges, iLength) < 0){
				return -1;
			}
		}

	}
	else{
		Dijkstra*dijkstra = (Dijkstra*)SearchFun[iUserId];
		if (dijkstra){
			if (dijkstra->SetEdges(pEdges, iLength) < 0){
				return -1;
			}
		}

	}
	return 0;
}

EXPS_FUNCTION(int)PathSearch_StartFolyd(const int iUserId, const char* pFloydPath, int(*FinishLevelCB)(const unsigned int iPercent, const void *pUsr), void* pUsr)
{
	if( m_nSearchType == PathSearch_FLOYD ) {
		if( !FinishLevelCB ) {
			return -1;
		}
		Floyd*floyd = (Floyd*)SearchFun[iUserId];
		if (floyd){
			if (floyd->StartFolyd( pFloydPath,FinishLevelCB, pUsr) < 0){
				return -1;
			}
		}

	} 
	return 0;	 
}
std::recursive_mutex _lock_path;
EXPS_FUNCTION(int) PathSearch_GetPath(const int iUserId, const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance)
{
	std::lock_guard<std::recursive_mutex> guard_lock(_lock_path);
	if (iSearchType[iUserId] == PathSearch_FLOYD){
		Floyd*floyd = (Floyd*)SearchFun[iUserId];
		if (floyd){
			if (floyd->GetPathByUpl(stStartUpl, stEndUpl, path, iLength, fDistance) < 0){
				return -1;
			}
		}

	}
	else{
		Dijkstra*dijkstra = (Dijkstra*)SearchFun[iUserId];
		if (dijkstra){
			if (dijkstra->GetPathByUpl(stStartUpl, stEndUpl, path, iLength, fDistance) < 0){
				return -1;
			}
		}

	}

	
	return 0;
}

EXPS_FUNCTION(int) PathSearch_GetPathByPos(const int iUserId, const PathSearch_Position stStartPos, const PathSearch_Position stEndPos, PathSearch_Wop** path, int* iLength, float* fDistance)
{
	if (iSearchType[iUserId] == PathSearch_FLOYD){
		Floyd*floyd = (Floyd*)SearchFun[iUserId];
		if (floyd){
			if (floyd->GetPathByPos(stStartPos, stEndPos, path, iLength, fDistance) < 0){
				return -1;
			}
		}

	}
	else{
		Dijkstra*dijkstra = (Dijkstra*)SearchFun[iUserId];
		if (dijkstra){
			if (dijkstra->GetPathByPos(stStartPos, stEndPos, path, iLength, fDistance) < 0){
				return -1;
			}
		}

	}

	return 0;
}

EXPS_FUNCTION(int) PathSearch_GetPathBySPosAndEUpl(const int iUserId, const PathSearch_Position stStartPos, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance)
{
	if (iSearchType[iUserId] == PathSearch_FLOYD){
		Floyd*floyd = (Floyd*)SearchFun[iUserId];
		if (floyd){
			if (floyd->GetPathBySPosAndEUpl(stStartPos, stEndUpl, path, iLength, fDistance) < 0){
				return -1;
			}
		}

	}
	else{
		Dijkstra*dijkstra = (Dijkstra*)SearchFun[iUserId];
		if (dijkstra){
			if (dijkstra->GetPathBySPosAndEUpl(stStartPos, stEndUpl, path, iLength, fDistance) < 0){
				return -1;
			}
		}

	}

	return 0;

}

EXPS_FUNCTION(int) PathSearch_GetPathByWop(const int iUserId, const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance)
{
	if (iSearchType[iUserId] == PathSearch_FLOYD){
		Floyd*floyd = (Floyd*)SearchFun[iUserId];
		if (floyd){
			if (floyd->GetPath(stStartUpl, stEndUpl, path, iLength, fDistance) < 0){
				return -1;
			}
		}

	}
	else{
		Dijkstra*dijkstra = (Dijkstra*)SearchFun[iUserId];
		if (dijkstra){
			if (dijkstra->GetPath(stStartUpl, stEndUpl, path, iLength, fDistance) < 0){
				return -1;
			}
		}

	}
	

	return 0;
}

EXPS_FUNCTION(void) PathSearch_ReleasePath(const int iUserId, PathSearch_Wop * path)
{
	if (iSearchType[iUserId] == PathSearch_FLOYD){
		Floyd*floyd = (Floyd*)SearchFun[iUserId];
		if (floyd){
			floyd->Releasepath(path);
		}

	}
	else{
		Dijkstra*dijkstra = (Dijkstra*)SearchFun[iUserId];
		if (dijkstra){
			dijkstra->Releasepath(path);
		}

		}
}

EXPS_FUNCTION(int) PathSearch_GetPathAtWeight(const int iUserId, const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance, const int* pWeightEdgesId, const unsigned int iWeightEdgesNum)
{

	if (iSearchType[iUserId] == PathSearch_FLOYD){
			return -1;
	}
	else{
		Dijkstra*dijkstra = (Dijkstra*)SearchFun[iUserId];
		if (dijkstra){
			if (dijkstra->GetPathAtWeight(stStartUpl, stEndUpl, path, iLength, fDistance, pWeightEdgesId, iWeightEdgesNum) < 0){
				return -1;
		}

		}
	}

	return 0;
}

EXPS_FUNCTION(void) PathSearch_Release()
{
	for (int i = 0; i < MapNumber; i++)
	{
		if (SearchFun[i]){
			delete (Floyd*)SearchFun[i];
			SearchFun[i] = nullptr;
			iSearchType[i] = -1;
		}
	}
	m_iUserId = -1;
}





