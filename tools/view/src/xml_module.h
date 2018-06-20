#pragma once

#include <vector>
#include <stdint.h>
#include"elmo.h"
#include"moons.h"
#include"angle_encoder.h"
#include"drive_unit.h"
#include"wheel.h"
#include"navigation.h"
#include"vehicle.h"
//#include"posix.h"
#include"dio.h"

struct can_bus{
	int id_;
	char name_[128];
	int						canbus_id_;				// port
	int						canbus_type_;
	int						baud_rate_;
};

struct elmo{
	int                     id_;
	char                    name_[128];
	var__elmo_t             var_elmo_info;
};

struct driveunit{
	int id_;
	char name_[128];
	var__drive_unit_t  var__driveunit_info;
	std::vector<int>wheel_;
};

struct copley{
	int id_;
	char name_[128];
	int can_;
	int port_;
	int node_;
};


struct swheel{
	int 					id_;
	char 					name_[128];
	var__swheel_t           var__swheel_info;

};

struct dwheel {
	int                     id_;
	char                    name_[128];
	var__dwheel_t           var__dwheel_info;

};

struct sddex{
	int                     id_;
	char                    name_[128];
	var__sdd_extra_t        var__sdd_extra_info;
};

struct navigation {
	int             id_;
	char            name_[128];
	var__navigation_t     var__navigation_info;

};

struct vehicle{
	int             id_;
	char            name_[128];
	var__vehicle_t  var__vehicle_info;

};
struct mnt{
	int from;
	int foff;
	int to;
	int toff;
	int len;

	struct mnt &operator=(const mnt &lref){
		if (&lref == this)return *this;
		from = lref.from;
		foff = lref.foff;
		to = lref.to;
		toff = lref.toff;
		len = lref.len;
		return *this;
	}
};

struct moos{
	int                     id_;
	char                    name_[128];
	var__moos_t             var__moos_info;
};


struct angle_encoder{
	int                     id_;
	char                    name_[128];
	var__angle_encoder_t    var__angle_encoder_info;
};

typedef struct{
	int                     id_;
	char                    name_[128];
	var__dio_t              var__dio_info;
	int						ais_block_num_;
	int						aos_block_num_;
}dio;

int __stdcall add_canbus_element(can_bus& canbus_ele);
int __stdcall add_elmo_element(elmo& elmo_ele);
int __stdcall add_copley_element(copley& copley_ele);
int __stdcall add_driveunit_element(driveunit& driveunit_ele);
int __stdcall add_dwheel_element(dwheel& dwheel_ele);
int __stdcall add_swheel_element(swheel& swheel_ele);
int __stdcall add_sddex_element(sddex& sddex_ele);
int __stdcall add_navigation_element(navigation& navigation_ele);
int __stdcall add_vehicle_element(vehicle& vehicle_ele);
int __stdcall add_mnt_element(mnt& mnt_ele);
int __stdcall add_moos_element(moos& moos_ele);
int __stdcall add_angle_encoder_element(angle_encoder& angle_encoder_ele);
int __stdcall add_dio_element(dio&var__dio_ele);

int __stdcall get_canbus_element(std::vector<can_bus>&vct_can_bus);
int __stdcall get_elmo_element(std::vector<elmo>& elmo_ele);
int __stdcall get_driveunit_element(std::vector<driveunit>& driveunit_ele);
int __stdcall get_dwheel_element(std::vector<dwheel>& dwheel_ele);
int __stdcall get_swheel_element(std::vector<swheel>& swheel_ele);
int __stdcall get_sddex_element(std::vector<sddex>& sddex_ele);
int __stdcall get_navigation_element(std::vector<navigation>& navigation_ele);
int __stdcall get_vehicle_element(std::vector<vehicle>& vehicle_ele);
int __stdcall get_mnt_element(std::vector<mnt>& mnt_ele);
int __stdcall get_moos_element(std::vector<moos>& moos_ele);
int __stdcall get_angle_encoder_element(std::vector<angle_encoder>& angle_encoder_ele);
int __stdcall get_dio_element(std::vector<dio>&var__dio_element);

int __stdcall update_driveunit_element(driveunit& driveunit_ele);
int __stdcall update_canbus_element(can_bus& canbus_ele);
int __stdcall update_elmo_element(elmo& elmo_ele);
int __stdcall update_dwheel_element(dwheel& dwheel_ele);
int __stdcall update_swheel_element(swheel& swheel_ele);
int __stdcall update_sddex_element(sddex& sddex_ele);
int __stdcall update_moos_element(moos& moos_ele);
int __stdcall update_angle_encoder_element(angle_encoder& angle_encoder_ele);
int __stdcall update_dio_element(dio&var__dio_element);

int __stdcall delete_canbus_element(int canbus_id);
int __stdcall delete_elmo_element(int elmo_id);
int __stdcall delete_driveunit_element(int driveunit_ele);
int __stdcall delete_dwheel_element(int dwheel_id);
int __stdcall delete_swheel_element(int swheel_id);
int __stdcall delete_sddex_element(int sddex_id);
int __stdcall delete_mnt_element(mnt& mnt_ele);
int __stdcall delete_moos_element(int moos_id);
int __stdcall delete_angle_encoder_element(int angle_encoder_id);
int __stdcall delete_dio_element(int dio_id);