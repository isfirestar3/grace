#ifndef Dijkstra_20161228_H
#define Dijkstra_20161228_H
#include<map>
#include<vector>
#include<mutex>
#include"PathSearch.h"
#include"singleton.hpp"

#define INVALID_DISTANCE 1000000


class Dijkstra
{
public:
	Dijkstra();
	~Dijkstra();
	//friend class nsp::toolkit::singleton<Dijkstra>;

private:
	std::vector<PathSearch_Wop> m_vctWops;
	std::map<unsigned int/*link_id*/, PathSearch_Link> m_Links;
	//std::map<unsigned int/* from_wop_index*/, std::map<unsigned int/*to_wop_index*/, float/*distance*/>> m_mapWopDistances;
	float** m_WopDistances;
	int m_WopDistancesLength = 0;

	std::map<unsigned int/*edge_id*/, PathSearch_EdgeInfo> m_mapEdges;
	std::map<unsigned int/*wop_id*/, PathSearch_Wop> m_mapWopType;

private:
	int GetStartAndEndWop( const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl,
		int& startWopIndex, int& endWopIndex,
		float& fDistanceStart, float& fDistanceEnd );
	int GetStartAndEndWeightWop(const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl,
		int& startWopIndex, int& endWopIndex,
		float& fDistanceStart, float& fDistanceEnd, std::vector<PathSearch_Wop>& m_vctWops_tmp);
	int IsSameWop( const PathSearch_Wop stWop1, const PathSearch_Wop stWop2 );
	int SameWopPathSearch( const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl,
		const unsigned int iSameWopIndex,
		const float fDistanceStart, const float fDistanceEnd,
		std::vector<unsigned int>& vctWopPath, float& fDistance, std::vector<PathSearch_Wop>& m_vctWops_tmp);
	int SameWopInsideSearch( const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl,
		const unsigned int iSameWopIndex,
		std::vector<unsigned int>&  vctWopPath, float& fDistance);
	int SameWopOutsideSearch( const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl,
		const unsigned int iSameWopIndex,
		const float fDistanceStart, const float fDistanceEnd,
		std::vector<unsigned int>&  vctWopPath, float& fDistance);
	int SameWopInsideWeightSearch(const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl,
		const unsigned int iSameWopIndex,
		std::vector<unsigned int>&  vctWopPath, float& fDistance, std::vector<PathSearch_Wop>& m_vctWops_tmp);
	int SameWopOutsideWeightSearch(const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl,
		const unsigned int iSameWopIndex,
		const float fDistanceStart,
		const float fDistanceEnd,
		std::vector<unsigned int>&  vctWopPath, float& fDistance, std::vector<PathSearch_Wop>& m_vctWops_tmp);
	int StartWeightDijkstra(unsigned int startWopIndex, unsigned int endWopIndex, std::vector<unsigned int>& vctWopPath, float& fDistance, std::vector<PathSearch_Wop>& m_vctWops_tmp);
	int GetWeightPath(const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance, std::vector<PathSearch_Wop>& m_vctWops_tmp);
	int m_iDijkstraId;
	unsigned int m_iSearchType;
	std::recursive_mutex		_lock_data;
	struct Path_Info{
		int ireturn;
		PathSearch_Upl stStartUpl;
		PathSearch_Upl stEndUpl;
		PathSearch_Wop* path;
		int iLength;
		float fDistance;		
	};
	std::vector<Path_Info> path_info;
#if _WIN32
	void Dijktra_thread(Path_Info &path_iata);
#else
	void Dijktra_thread(void* param);
#endif

public:
	int GetDijkstraId();
	int SetDijkstraId(unsigned int iDijkstraId);
	int SetSearchType(unsigned int iSearchType);
public:
	int LoadData( const PathSearch_LayoutData* pLayoutData );
	int SetWops( const PathSearch_Wop* pWops, const unsigned int iLength );
	int SetLinks( const  PathSearch_Link* pLinks, const unsigned int iLength );
	int SetEdges( const PathSearch_EdgeInfo* pEdges, const unsigned int iLength );
	int StartDijkstra(unsigned int startWopIndex, unsigned int endWopIndex, std::vector<unsigned int>& vctWopPath, float& fDistance);
	int GetPath(const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance);
	int GetPathByUpl( const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance );
	int GetPathByPos( const PathSearch_Position stStartPos, const PathSearch_Position stEndPos, PathSearch_Wop** path, int* iLength, float* fDistance );
	int GetPathBySPosAndEUpl( const PathSearch_Position stStartPos, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance );
	void Releasepath( PathSearch_Wop * path );
	int GetPathAtWeight(const PathSearch_Upl stStartUpl, const PathSearch_Upl stEndUpl, PathSearch_Wop** path, int* iLength, float* fDistance, const int* pWeightEdgesId, const unsigned int iWeightEdgesNum);
	int ChangeWopValue(const int* pWeightEdgesId, const unsigned int iWeightEdgesNum, std::vector<PathSearch_Wop>& m_vctWops_tmp);
	int RestoreWopValue(const int* pWeightEdgesId, const unsigned int iWeightEdgesNum);
};

#endif

