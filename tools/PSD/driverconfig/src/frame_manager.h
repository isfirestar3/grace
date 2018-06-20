#pragma once
#include "singleton.hpp"
#include <QWidget>

class frame_manager
{
private:
	frame_manager();
public:
	
	friend class  nsp::toolkit::singleton<frame_manager>;

	~frame_manager();

	int init_frame( QWidget * pmainframe = nullptr);

	QWidget * getspeedframe() { return pSpeed_widget; };

	QWidget * getcurrentframe() { return pCurrent_widget; };

	QWidget * getdiframe() { return pDi_widget; };

	QWidget * getmotorframe() { return pMotor_widget; };

	QWidget * getpositionframe() { return pPosition_widget; };
private:
	QWidget * pSpeed_widget = nullptr;
	QWidget * pCurrent_widget = nullptr;
	QWidget * pDi_widget = nullptr;
	QWidget * pMain_widget = nullptr;
	QWidget * pMotor_widget = nullptr;
	QWidget * pPosition_widget = nullptr;
};

