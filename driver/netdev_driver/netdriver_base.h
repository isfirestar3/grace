#ifndef __NETDRIVER_BASE_H__
#define __NETDRIVER_BASE_H__

#include "var.h"

class netdriver_base
{
public:
    netdriver_base();
    virtual ~netdriver_base();
    
    virtual int net_create(var__functional_object_t *object) = 0;
    virtual int net_close() = 0;

private:

};

#endif


