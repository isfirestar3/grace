#include "agv_atom_taskdata_opt.h"


agv_atom_taskdata_newopt::agv_atom_taskdata_newopt()
:agv_atom_taskdata_base(AgvAtomTaskData_NewOpt)
{
    agv_atom_taskdata_base::set_task_phase(AgvTaskPhase_Send);
}


agv_atom_taskdata_newopt::~agv_atom_taskdata_newopt()
{
}

agv_atom_opt_taskdata_header::agv_atom_opt_taskdata_header()
:agv_atom_taskdata_header(AgvAtomTaskType_Operation)
{

}

agv_atom_opt_taskdata_header::~agv_atom_opt_taskdata_header()
{

}

agv_atom_taskdata_modifyopt::agv_atom_taskdata_modifyopt()
:agv_atom_taskdata_base(AgvAtomTaskData_ModifyOpt)
{
    agv_atom_taskdata_base::set_task_phase(AgvTaskPhase_Send);
}

agv_atom_taskdata_modifyopt::~agv_atom_taskdata_modifyopt()
{

}

agv_atom_taskdata_pauseopt::agv_atom_taskdata_pauseopt()
:agv_atom_taskdata_base(AgvAtomTaskData_PauseOpt)
{
    agv_atom_taskdata_base::set_task_phase(AgvTaskPhase_Send);
}

agv_atom_taskdata_pauseopt::~agv_atom_taskdata_pauseopt()
{

}

agv_atom_taskdata_resumeopt::agv_atom_taskdata_resumeopt()
:agv_atom_taskdata_base(AgvAtomTaskData_ResumeOpt)
{
    agv_atom_taskdata_base::set_task_phase(AgvTaskPhase_Send);
}

agv_atom_taskdata_resumeopt::~agv_atom_taskdata_resumeopt()
{

}

agv_atom_taskdata_cancelopt::agv_atom_taskdata_cancelopt()
:agv_atom_taskdata_base(AgvAtomTaskData_CancelOpt)
{
    agv_atom_taskdata_base::set_task_phase(AgvTaskPhase_Send);
}

agv_atom_taskdata_cancelopt::~agv_atom_taskdata_cancelopt()
{

}

agv_atom_taskdata_checkopt::agv_atom_taskdata_checkopt(const var__operation_t& opt)
:agv_atom_taskdata_base(AgvAtomTaskData_CheckOpt), __opt2check(opt)
{

}

agv_atom_taskdata_checkopt::~agv_atom_taskdata_checkopt()
{

}
