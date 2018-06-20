#if !defined AGV_INFO_H
#define AGV_INFO_H

#include <stdint.h>
#include "nisdef.h"
#pragma pack(push,1)

struct agv_attribute {
	char name[64];
	char describe[128];
};

struct agv_detail {
	uint32_t vhid;
	int count_of_attrs;
	struct agv_attribute *attrs;
};

enum load_agvinfo_method {
	LAM_Local,		// load agv info from local xml file£¬ @status field in agv_info must be AS_IGNORE
	LAM_Server,		// load agv info from xml file configure which storage on agvinfo-server
	LAM_Real,		// load agv info from agvinfo-server, include xml file configure and realtime report
};

struct agv_info {
	uint32_t vhid;
	uint32_t vhtype;		// type of this vehicle
	char inet[16];			// real/configure ipv4 address of this vehicle, like '192.168.1.101'
	uint16_t mtport;		// port of motion template --fixed(4409)
	uint16_t shport;		// port of agv shell --default(4410)
	uint16_t ftsport;		// port of agv shell fts --default(4411)
	char hwaddr[18];		// mac address of this vehicle, like '00:16:3e:06:6c:6c'
	uint16_t status;		// status of this vehicle
	int count_of_attrs;
	struct agv_attribute *attrs;
	struct agv_info *next;  // link all agvinfo node like a vector with nullptr ending
};


#pragma pack(pop)

#define AS_IGNORE		(0x7FFF)	// load agvinfo only from xml file
#define AS_ONLINE		(1)			// this vehicle is storage in xml file and it's online
#define AS_OFFLINE		(2)			// this vehicle is storage in xml file but it's offline
#define AS_UNKNOWN		(3)			// we can receive UDP report from client. but no @hwaddr can be match in xml file

/*  connect agvinfo server endpoint by inet:port
	call this method means calling process will be a agvinfo-client
*/
interface_format(int) connect_agvinfo_server(const char *inet, uint16_t port);

interface_format(void) disconnect_from_agvinfo();

/* load/cover base information of all agv 
   $cover_agvinfo, option LAM_Real can not be use.
   $cover_agvinfo, if @agvs is a nullptr, then agv list targeted by @method will be remove, all data in xml file will be erase
*/
interface_format(int) load_agvinfo(struct agv_info **agvs, enum load_agvinfo_method method);
interface_format(int) cover_agvinfo(const struct agv_info *agvs, enum load_agvinfo_method method);

/* get/set detail of specify agv
	$set_agvdetail, option LAM_Real can not be use
 */
interface_format(int) get_agvdetail(uint32_t vhid, struct agv_detail **detail, enum load_agvinfo_method method);
interface_format(int) set_agvdetail(uint32_t vhid, const struct agv_detail *detail, enum load_agvinfo_method method);

/* release agv info list @agvs which return by $load_agvinfo */
interface_format(void) release_agvinfo(struct agv_info *agvs);
/*release internal data pointer of @detail which return by $get_agvdetail*/
interface_format(void) release_agvdetail(struct agv_detail *detail);

/*if the current identity is a client, and connection to agvinfo-server is established, @notify will call when the server's data changed.*/
interface_format(void) about_something_changed(void(*notify)());


#endif



