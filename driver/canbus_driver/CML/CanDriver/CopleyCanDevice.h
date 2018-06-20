#ifndef _SURO_PACKAGE_ARIA_APP_BOBSHENHUI_9_12_H__
#define _SURO_PACKAGE_ARIA_APP_BOBSHENHUI_9_12_H__

#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <sstream>
#include <boost/shared_ptr.hpp>

#include "CML.h"

CML_NAMESPACE_USE();

enum CAN_CARD {
	CAN_CARD_COPLEY = 1, 
	CAN_CARD_PISO200,
	CAN_CARD_KVASER,
	CAN_CARD_IXXAT
};

class CopleyCanDevice
{
public:
	CopleyCanDevice();
	~CopleyCanDevice();

public:
	bool SetBaud(int baud_rate);
	bool Init(int id = 0, int baud_rate = 1000000);
	bool Close();

	int GetCanId();
	boost::shared_ptr<CanInterface> GetCanInterface();

private:
	bool InitCopley();
	bool InitPiso200();
	bool InitKvaser();
#ifdef _WIN32
	bool InitIXXAT();
#endif

private:
	CAN_CARD can_type_;

	int can_id_;
	int can_baud_rate_;

	boost::shared_ptr<CanInterface> copley_device_;
};

#endif
