#include "impl_net_dev.h"
#include <memory>
#include <map>
#include "object_switcher.hpp"
static std::map<int, std::shared_ptr<netdriver_base>> __map_udp_provider;

EXP(int) netdev_create(var__functional_object_t *object)
{
    std::map<int, std::shared_ptr<netdriver_base>>::iterator it = __map_udp_provider.find(object->object_id_);
    if (it != __map_udp_provider.end())
    {
        return 0;
    }
    std::shared_ptr<netdriver_base> net_object = ::get_netdriver_ptr(object);
    if (nullptr == net_object) return -1;
    if (net_object->net_create(object) < 0)
    {
        return -1;
    }
    __map_udp_provider[object->object_id_] = net_object;

    return 0;
}


EXP(int) netdev_uinit()
{
    for (std::map<int, std::shared_ptr<netdriver_base>>::iterator it = __map_udp_provider.begin();
        it != __map_udp_provider.end();++it)
    {
        it->second->net_close();
    }
    __map_udp_provider.clear();
	return 0;
}



