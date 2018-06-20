#include "frame_manager.h"
#include "current_frame.h"
#include "di_frame.h"
#include "motor_frame.h"
#include "position_frame.h"
#include "speed_frame.h"

frame_manager::frame_manager()
{

}


frame_manager::~frame_manager()
{

}

int frame_manager::init_frame( QWidget * pmainframe )
{
	try
	{
		pSpeed_widget = new speed_frame();
		pCurrent_widget = new current_frame();
		pDi_widget = new di_frame;
		pMotor_widget = new motor_frame();
		pPosition_widget = new position_frame;
	}
	catch (...)
	{
		return	-1;
	}
	return 0;
}
