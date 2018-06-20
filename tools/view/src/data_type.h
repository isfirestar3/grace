#pragma once

#include <vector>
#include <map>
#include <string>
#include "xml_module.h"
#include "net_impls.h"
#include <Windows.h>
#include "vartypes.h"
//#include "posix.h"

//[100, 120)段为canbus id号
#define CANBUS_START_ID			100
#define CANBUS_END_ID			CANBUS_START_ID + 20

//121 为navigation id号
#define NAVIGATION_ID          121

//122 为vehicle id号
#define VEHICLE_ID             122

//[130,170)段为elmo id号
#define ELMO_START_ID			130
#define ELMO_END_ID				ELMO_START_ID + 40

//[180,200)段为moos id号
#define MOOS_START_ID		180
#define MOOS_END_ID			MOOS_START_ID + 20

//[200,220)段为angle_encoder id号
#define ANGLE_ENCODER_START_ID   200
#define ANGLE_ENCODER_END_ID     ANGLE_ENCODER_START_ID + 20

//[30,50)段为driverunit id号
#define DRIVERUNIT_START_ID		30
#define DRIVERUNIT_END_ID		DRIVERUNIT_START_ID + 20

//[220,250)段为dio id号
#define DIO_START_ID         220
#define DIO_END_ID           DIO_START_ID + 30

//[250,300)段为sddex id号
#define SDDEX_START_ID		250
#define SDDEX_END_ID		SDDEX_START_ID + 50

//[300,400)段为dwheel id号
#define DWHEEL_START_ID		300
#define DWHEEL_END_ID		DWHEEL_START_ID + 100

//[400,500)段为swheel id号
#define SWHEEL_START_ID		400
#define SWHEEL_END_ID		SWHEEL_START_ID + 100

//树上child的类型
#define PHY_TYPE				0
#define CANBUS_TYPE				1
#define ELMO_TYPE				2
#define ELMO_ELEMENT			3
#define MOOS_TYPE			    4
#define MOOS_ELEMENT		    5
#define ANGLE_ENCODER_TYPE      6
#define ANGLE_ENCODER_ELEMENT   7
#define DIO_TYPE                8
#define DIO_ELEMENT             9
#define LOGIC_TYPE				10
#define DRIVERUNIT				11
#define DRIVERUNIT_ELEMENT		12
#define DRIVERUNIT_DWHEER		13
#define DRIVERUNIT_SWHEER		14
#define DRIVERUNIT_SDDEX        15
#define NAVIGATION				20
#define NAVIGATION_ELEMENT		21
#define VEHICLE					30
#define VEHICLE_ELEMENT			31

#define	CanDrvType_Unknown		0
#define CanDrvType_Kvaser		1
#define	CanDrvType_CanCard		2
#define	CanDrvType_IxxatV3		3

//#if !defined offsetof
#define offsetof_data(__type, __field)      (( unsigned int )(&((__type*)0)->__field))
//#endif

//#if !defined msizeof
#define msizeof(__type, __field)      (sizeof(((__type*)0)->__field))
////#endif
#pragma pack (push, 1)

struct canbus_collection{
	int id_;
	std::string canbus_desc_;
	int can_id_;
	int canbus_type_;
	int canbus_buad_;
	std::vector<copley> vct_copley_collection_;
	std::vector<elmo> vct_elmo_collection_;
	std::vector<moos> vct_moos_collection_;
	std::vector<angle_encoder> vct_angle_encoder_collection_;
	std::vector<dio>vct_dio_collection_;
};

struct driveuint_collection{
	int driveuint_id_;
	std::string driveuint_desc_;
	int driveuint_type_;
	double driveuint_install_x_;
	double driveuint_install_y_;
	double driveuint_install_theta_;
	double driveuint_calibrated_x_;
	double driveuint_calibrated_y_;
	double driveuint_calibrated_theta_;
	std::vector<int> driveuint_wheel_;
	std::vector<dwheel> driveuint_vct_dwheel_;
	std::vector<swheel> driveuint_vct_swheel_;
	std::vector<sddex>driveuint_vct_sddex_;
};

struct all_xml_collection{
	std::vector<canbus_collection> vct_canbus_collection_;
	std::vector<elmo> vct_elmo_collection_;
	std::vector<moos> vct_moos_collection_;
	std::vector<angle_encoder> vct_angle_encoder_collection_;
	std::vector<driveuint_collection> vct_driveuint_collection_;
	std::vector<dwheel> vct_dwheel_collection_;
	std::vector<swheel> vct_swheel_collection_;
	std::vector<sddex> vct_sddex_collection_;
	std::vector<navigation> vct_navi_collection_;
	std::vector<vehicle> vct_vehicle_collection;
	std::vector<dio>vct_dio_collection_;
};

struct offsetinfo{
	int len;
	int	offset;
};

enum btn_state{
	START_MONITOR = 0,
	PAUSE_MONITOR,
	STOP_MONITOR,
	SEND_COMMAND
};