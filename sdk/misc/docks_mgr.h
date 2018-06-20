//////////////////////////////////////////////////////////////////////////
/*!
 * file docks_mgr.h
 * date 2017/07/28 13:52
 *
 * author chenkai
 *
 * brief  docks.xml reader
 *
 * note
*/
#ifndef __DOCKS_MGR_H_CK__
#define __DOCKS_MGR_H_CK__
#include "optpar.h"
#include <map>

class docks_mgr
{
public:
    struct op_info
    {
        int id;
        int type;
        int64_t parameters_[20];
    };
    struct DocksInfo
    {
        int id;
        upl_t upl;
        position_t pos;
        std::map<int, op_info> opt;
    };
public:
    docks_mgr();
    ~docks_mgr();

    int load_docks(std::string file);
    int get_op_info_by_id(int dock_id, int opt_id, op_info& op);


private:

    std::map<int, DocksInfo> __map_docks;
};

#endif


