#include "agv_atom_taskdata_nav.h"


agv_atom_taskdata_newnav::agv_atom_taskdata_newnav()
:agv_atom_taskdata_base(AgvAtomTaskData_NewNav)
{
}


agv_atom_taskdata_newnav::~agv_atom_taskdata_newnav()
{
}


agv_atom_taskdata_addnav::agv_atom_taskdata_addnav()
:agv_atom_taskdata_base(AgvAtomTaskData_AddNav)
{
}


agv_atom_taskdata_addnav::~agv_atom_taskdata_addnav()
{
}



agv_atom_nav_taskdata_header::agv_atom_nav_taskdata_header()
:agv_atom_taskdata_header(AgvAtomTaskType_Nav)
{

}

agv_atom_nav_taskdata_header::~agv_atom_nav_taskdata_header()
{

}

agv_atom_taskdata_pathsearch::agv_atom_taskdata_pathsearch()
:agv_atom_taskdata_base(AgvAtomTaskData_PathSearch)
{

}

agv_atom_taskdata_pathsearch::~agv_atom_taskdata_pathsearch()
{

}

agv_atom_taskdata_pausenav::agv_atom_taskdata_pausenav()
:agv_atom_taskdata_base(AgvAtomTaskData_PauseNav)
{
    agv_atom_taskdata_base::set_task_phase(AgvTaskPhase_Send);
}

agv_atom_taskdata_pausenav::~agv_atom_taskdata_pausenav()
{

}

agv_atom_taskdata_resumenav::agv_atom_taskdata_resumenav()
:agv_atom_taskdata_base(AgvAtomTaskData_ResumeNav)
{
    agv_atom_taskdata_base::set_task_phase(AgvTaskPhase_Send);
}

agv_atom_taskdata_resumenav::~agv_atom_taskdata_resumenav()
{

}

agv_atom_taskdata_cancelnav::agv_atom_taskdata_cancelnav()
:agv_atom_taskdata_base(AgvAtomTaskData_CancelNav)
{
    agv_atom_taskdata_base::set_task_phase(AgvTaskPhase_Send);
}

agv_atom_taskdata_cancelnav::~agv_atom_taskdata_cancelnav()
{

}
