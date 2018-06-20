#include "docks_mgr.h"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include "log.h"
#include "os_util.hpp"
docks_mgr::docks_mgr()
{
}


docks_mgr::~docks_mgr()
{
}

int docks_mgr::load_docks(std::string dock_path)
{
    //std::string file_path = nsp::os::get_module_directory<char>();
    //std::string dock_path = file_path + "/docks.xml";
    rapidxml::xml_document<> doc;
    rapidxml::file<char> *file = nullptr;
    try {
        file = new rapidxml::file<char>(dock_path.c_str());
        doc.parse<0>(file->data());
        rapidxml::xml_node<>* root = doc.first_node("docks");
        if (root)
        {
            for (rapidxml::xml_node<>* dock = root->first_node(); dock; dock = dock->next_sibling())
            {
                DocksInfo anchorInfo;
                rapidxml::xml_node<>* id = dock->first_node("id");
                if (id)
                {
                    anchorInfo.id = atoi(id->value());
                }
                rapidxml::xml_node<>* pos = dock->first_node("pos");
                if (pos)
                {
                    rapidxml::xml_attribute<char>* attr = pos->first_attribute("x");
                    if (attr)
                    {
                        anchorInfo.pos.x_ = atof(attr->value());
                    }
                    attr = pos->first_attribute("y");
                    if (attr)
                    {
                        anchorInfo.pos.y_ = atof(attr->value());
                    }
                    attr = pos->first_attribute("angle");
                    if (attr)
                    {
                        anchorInfo.pos.angle_ = atof(attr->value());
                    }
                }
                rapidxml::xml_node<>* upl = dock->first_node("upl");
                if (upl)
                {
                    rapidxml::xml_attribute<char>* attr = upl->first_attribute("edge_id");
                    if (attr)
                    {
                        anchorInfo.upl.edge_id_ = atoi(attr->value());
                    }
                    attr = upl->first_attribute("percentage");
                    if (attr)
                    {
                        anchorInfo.upl.percentage_ = atof(attr->value());
                    }
                    attr = upl->first_attribute("aoa");
                    if (attr)
                    {
                        //anchorInfo.upl.angle_ = atof(attr->value()) * PI / 180.0;
                        anchorInfo.upl.angle_ = atof(attr->value());
                    }
                }

                rapidxml::xml_node<>* opts = dock->first_node("opts");
                if (opts)
                {
                    for (rapidxml::xml_node<>* opt = opts->first_node(); opt; opt = opt->next_sibling())
                    {
                        if (!opt)
                        {
                            continue;
                        }
                        op_info optInfo;
                        rapidxml::xml_node<>* id = opt->first_node("id");
                        if (id)
                        {
                            optInfo.id = atoi(id->value());
                        }
                        rapidxml::xml_node<>* type = opt->first_node("type");
                        if (type)
                        {
                            optInfo.type = atoi(type->value());
                        }
                        int i = 0;
                        for (rapidxml::xml_node<>* param = opt->first_node("p0"); param; param = param->next_sibling())
                        {
                            if (!param) continue;
                            int64_t val = atoll(param->value());
                            optInfo.parameters_[i++] = val;
                        }

                        anchorInfo.opt[optInfo.id] = optInfo;
                    }
                }

                __map_docks[anchorInfo.id] = anchorInfo;
            }

        }
        delete file;
    }
    catch (...) {
        if (file)
        {
            delete file;
        }
        
        loerror("docksmgr") << "Load Dock" << dock_path<<" Failed...";
        return -1;
    }

    return 0;
}

int docks_mgr::get_op_info_by_id(int dock_id, int opt_id, op_info& op)
{
    std::map<int, DocksInfo>::iterator itr_dock = __map_docks.find(dock_id);
    if (itr_dock == __map_docks.end())
    {
        return -1;
    }

    std::map<int, op_info>::iterator itr_opt = itr_dock->second.opt.find(opt_id);
    if (itr_opt == itr_dock->second.opt.end())
    {
        return -1;
    }

    op = itr_opt->second;
    return 0;
}
