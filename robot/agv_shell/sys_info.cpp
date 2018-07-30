#include "sys_info.h"
#include "agv_shell_common.h"  //global_parameter
#include "agv_shell_define.h"
#include "agv_shell_server.h"  //agv_shell_server
#include "const.h"
#include "log.h"
#include "os_util.hpp"  //gettick
#include "proto_agv_sysinfo.h" //proto_sysinfo_fixed_t proto_sysinfo_changed_t
#include "public.h"    //split_symbol_string
#include "swnet.h"  //swnet

#ifdef _WIN32
#include<atlbase.h>
#include<atlconv.h>
#include<IPHlpApi.h>
#define NET_CARD_KEY "SYSTEM\\CurrentControlSet\\Control\\Class\\{4d36e972-e325-11ce-bfc1-08002be10318}"
#else
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <errno.h>
#endif

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

extern 
int get_process_run_info(nsp::proto::proto_vector_t<agv::proto::proto_process_run_info_t>&);
extern
int get_process_config_info(nsp::proto::proto_vector_t<agv::proto::proto_process_info_t>&);


sys_info::sys_info() : mac_addr_("")
					, cpu_used_percentage_(0)
					, uname_info_("")
					, network_name_("eth0") {
	;
}

sys_info::~sys_info()
{

}

int sys_info::init() {
	net_iostat_ = (net_iostat*)malloc( sizeof(net_iostat) );
	memset(net_iostat_, 0, sizeof(net_iostat));
	get_mac();  //必须先与 get_iostat 调用 
	get_devmem();
	get_cpu_info();
	get_cpuoccupy(last_cpu_stat_);
	get_iostat();
	posix__getsysmem(&sys_mem_);
	get_version();
	
	char host_name[64] = { 0 };
	nsp::toolkit::singleton<nsp::tcpip::swnet>::instance()->nis_lgethost(host_name, 64);
	host_name_ = host_name;
	if (host_name_.length() == 0){
		host_name_ = "N/A";
	}
	
	print_statinfo(); //for test
	
	return 0;
}

void sys_info::get_mac()
{
#ifdef _WIN32
	PIP_ADAPTER_INFO apter_info;
	ULONG output = sizeof(_IP_ADAPTER_INFO);
	apter_info = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (apter_info == NULL) {
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
	}
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(apter_info, &output) == ERROR_BUFFER_OVERFLOW)
	{
		FREE(apter_info);
		apter_info = (IP_ADAPTER_INFO *)MALLOC(output);
		if (apter_info == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
		}
	}
	PIP_ADAPTER_INFO pAdapter = NULL;
	if (GetAdaptersInfo(apter_info, &output) == NO_ERROR) {
		pAdapter = apter_info;
		std::string ipv4;
		while (pAdapter) {
			char mac_chr[64] = { 0 };
			for (UINT i = 0; i < pAdapter->AddressLength; i++) {
				if (i == (pAdapter->AddressLength - 1))
				{
					printf("%.2X\n", (int)pAdapter->Address[i]);
					sprintf(mac_chr, "%s%.2X", mac_chr, (int)pAdapter->Address[i]);
				}
				else
				{
					printf("%.2X:", (int)pAdapter->Address[i]);
					sprintf(mac_chr, "%s%.2X:", mac_chr, (int)pAdapter->Address[i]);
				}
			}
			mac_addr_ = mac_chr;
			switch (pAdapter->Type) {
			case MIB_IF_TYPE_OTHER:
				printf("Other\n");
				break;
			case MIB_IF_TYPE_ETHERNET:
				printf("Ethernet\n");
				break;
			case MIB_IF_TYPE_TOKENRING:
				printf("Token Ring\n");
				break;
			case MIB_IF_TYPE_FDDI:
				printf("FDDI\n");
				break;
			case MIB_IF_TYPE_PPP:
				printf("PPP\n");
				break;
			case MIB_IF_TYPE_LOOPBACK:
				printf("Lookback\n");
				break;
			case MIB_IF_TYPE_SLIP:
				printf("Slip\n");
				break;
			default:
				printf("Unknown type %ld\n", pAdapter->Type);
				break;
			}
			ipv4 = pAdapter->IpAddressList.IpAddress.String;
			printf("\tIP Address: \t%s\n",
				pAdapter->IpAddressList.IpAddress.String);
			printf("\tIP Mask: \t%s\n", pAdapter->IpAddressList.IpMask.String);

			printf("\tGateway: \t%s\n", pAdapter->GatewayList.IpAddress.String);
			printf("\t***\n");
			struct tm newtime;
			char buffer[32];
			errno_t error;
			if (pAdapter->DhcpEnabled) {
				printf("\tDHCP Enabled: Yes\n");
				printf("\t  DHCP Server: \t%s\n",
					pAdapter->DhcpServer.IpAddress.String);

				printf("\t  Lease Obtained: ");
				/* Display local time */
				error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseObtained);
				if (error)
					printf("Invalid Argument to _localtime32_s\n");
				else {
					// Convert to an ASCII representation 
					error = asctime_s(buffer, 32, &newtime);
					if (error)
						printf("Invalid Argument to asctime_s\n");
					else
						/* asctime_s returns the string terminated by \n\0 */
						printf("%s", buffer);
				}

				printf("\t  Lease Expires:  ");
				error = _localtime32_s(&newtime, (__time32_t*)&pAdapter->LeaseExpires);
				if (error)
					printf("Invalid Argument to _localtime32_s\n");
				else {
					// Convert to an ASCII representation 
					error = asctime_s(buffer, 32, &newtime);
					if (error)
						printf("Invalid Argument to asctime_s\n");
					else
						/* asctime_s returns the string terminated by \n\0 */
						printf("%s", buffer);
				}
				NET_ADAPTER_TYPE type;
				is_virtual_adapter(pAdapter->AdapterName, type);
				printf("\tNetword Type:%d\n", type);
			}
			else
				printf("\tDHCP Enabled: No\n");
			
			if (nsp::toolkit::singleton<global_parameter>::instance()->get_local_ipv4() == ipv4) {
				break;;
			}
			pAdapter = pAdapter->Next;
		}
	}
	if (apter_info)
		FREE(apter_info);
	
#else
	int fd;
	struct ifreq buf[16];
	struct ifconf ifc;
	int get_flag = 0;

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		loerror("agv_shell") << __FILE__ <<":" << __LINE__ << " socket error:" << strerror(errno);
		close(fd);
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;
	if (ioctl(fd, SIOCGIFCONF, (char *)&ifc)) {
		loerror("agv_shell") << __FILE__ <<":" << __LINE__ << " ioctl get all net error:" << strerror(errno);
		close(fd);
	}
	
	int interfaceNum = ifc.ifc_len / sizeof(struct ifreq);
	char ip[32] = {0};
	std::string ipv4;
	
	while (interfaceNum-- > 0) {
		//ignore lo interface(loopback's mac is 0 )
		if (strcmp(buf[interfaceNum].ifr_name, "lo") == 0) {
			continue;
		}
		loinfo("agv_shell") << "ndevice name:" << buf[interfaceNum].ifr_name;

		//ignore the interface that not up or not runing  
		if (ioctl(fd, SIOCGIFFLAGS, (char *)&buf[interfaceNum])) {
			loerror("agv_shell") << __FILE__ <<":" << __LINE__ << " ioctl error:" << strerror(errno);
			continue;
		}
		
		//get the IP of this interface  
		if (ioctl(fd, SIOCGIFADDR, (char *)&buf[interfaceNum])) {
			loerror("agv_shell") << __FILE__ <<":" << __LINE__ << " ioctl get ip error:" << strerror(errno);
			continue;
		}
		
		memset(ip, 0, sizeof(ip));
		snprintf(ip, sizeof(ip), "%s", (char *)inet_ntoa(((struct sockaddr_in *)&(buf[interfaceNum].ifr_addr))->sin_addr));
		ipv4 = ip;
		if (nsp::toolkit::singleton<global_parameter>::instance()->get_local_ipv4() == ipv4) {
			get_flag = 1;
		}
		loinfo("agv_shell") << "device ip:" << ip;
		loinfo("agv_shell") << "local ip:" << nsp::toolkit::singleton<global_parameter>::instance()->get_local_ipv4();

		//get the mac of this interface  
		if (ioctl(fd, SIOCGIFHWADDR, (char *)(&buf[interfaceNum]))) {
			loerror("agv_shell") << __FILE__ <<":" << __LINE__ << " ioctl get mac addr error:" << strerror(errno);
			continue;
		}

		char mac[18] = {0};
		snprintf(mac, sizeof(mac), "%02x:%02x:%02x:%02x:%02x:%02x",
			(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[0],
			(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[1],
			(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[2],

			(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[3],
			(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[4],
			(unsigned char)buf[interfaceNum].ifr_hwaddr.sa_data[5]);
		mac_addr_ = mac;
		network_name_ = buf[interfaceNum].ifr_name;
		loinfo("agv_shell") << "device mac:" << mac_addr_;
		if (get_flag == 1) {
			break;
		}
	}
	close(fd);
	if (get_flag == 0) {
		loinfo("agv_shell") << "can't find local ip:" << nsp::toolkit::singleton<global_parameter>::instance()->get_local_ipv4()
			<< ", set local ip:" << ip;
		nsp::toolkit::singleton<global_parameter>::instance()->set_local_ipv4(ip);
	}
#endif

}

// get hard disk meeeage 
void sys_info::get_devmem() {
#ifndef _WIN32
	FILE * fp;
	char a[80],e[80],f[256];
	char* buf = (char*)malloc(256);
	uint64_t b,c,d;
	fp = popen("df","r");
	buf = fgets(buf, 256, fp); // skip title
	disk_info_.total_size = 0;
	disk_info_.used_size = 0;
	while( 6 == fscanf(fp,"%s %lu %lu %lu %s %s",a,&b,&c,&d,e,f)) {
		disk_info_.total_size += b;
		disk_info_.used_size += c;
	}
	loinfo("agv_shell") << "disk, total:" << disk_info_.total_size << ", used size:" << disk_info_.used_size;
	pclose(fp);
#endif
}

//  /proc/cpuinfo
void sys_info::get_cpu_info() {
#ifndef _WIN32
	FILE *fd;
	char* buff = (char*)malloc(32);
	std::string ori_str, path;
	std::vector<std::string> v_str;
	cpu_info cpu;
	uint32_t cpu_num = 0;
	//loinfo("agv_shell") << "begin get cpu info";
  
    fd = fopen ("/sys/devices/system/cpu/present", "r");
	buff = fgets(buff, 32, fd);
	if(buff) {
		ori_str = buff;
		v_str.clear();
		split_symbol_string(ori_str, '-', v_str);
		if(v_str.size() > 1 && !v_str[1].empty()) {
			cpu_num = atoi(v_str[1].c_str());
			++cpu_num;
		} else {
			cpu_num = 1;
		}
	}
	fclose(fd);
	
	for(uint32_t i = 0; i < cpu_num; ++i) {
		cpu.name = std::to_string(i);
		cpu.hz = 0;
		path = "/sys/devices/system/cpu/cpu" + cpu.name + "/cpufreq/cpuinfo_cur_freq";
		fd = fopen (path.c_str(), "r");
		if(fd && (buff = fgets(buff, 32, fd))) {
			cpu.hz = atol(buff);
			cpu.hz = cpu.hz >> 10;
			fclose(fd);
		}
		cpu_info_.push_back(cpu);
	}
	
#else
	
#endif
}

void sys_info::get_cpuoccupy (cpu_stat &cpust)
{     
#ifndef _WIN32
    FILE *fd;
    char* buff = (char*)malloc(256);
    cpu_stat *cpu_occupy;
    cpu_occupy=&cpust;
  
    fd = fopen ("/proc/stat", "r");
    buff = fgets (buff, 256, fd);
    //printf("%s\r\n",buff);
    sscanf (buff, "%s %u %u %u %u %u %u %u", cpu_occupy->name, &cpu_occupy->user, &cpu_occupy->nice,&cpu_occupy->system, &cpu_occupy->idle,&cpu_occupy->lowait,&cpu_occupy->irq,&cpu_occupy->softirq);  
    fclose(fd);
#else
	
#endif
}


//#方法1：cpu usage=(idle2-idle1)/(cpu2-cpu1)*100
//#方法2: cpu usage=[(user_2 +sys_2+nice_2) - (user_1 + sys_1+nice_1)]/(total_2 - total_1)*100
void sys_info::get_cpu_usage() {
#ifndef _WIN32
	cpu_stat n;
	get_cpuoccupy(n);
	
	unsigned long od, nd;
	double cpu_use = 0;

	od = (unsigned long) (last_cpu_stat_.user + last_cpu_stat_.nice + last_cpu_stat_.system +last_cpu_stat_.idle + \
			last_cpu_stat_.lowait + last_cpu_stat_.irq + last_cpu_stat_.softirq);
	nd = (unsigned long) (n.user + n.nice + n.system +n.idle + n.lowait + n.irq + n.softirq);

	double sum = nd - od;  
	double idle = n.idle - last_cpu_stat_.idle;  
	/*if(0 != sum) {
		cpu_use = idle/sum;
	} else {
		cpu_use = 0;
	}
	*/

	idle = n.user + n.system + n.nice - last_cpu_stat_.user - last_cpu_stat_.system - last_cpu_stat_.nice;
	if(0 != sum) {
		cpu_use = idle/sum;
	} else {
		cpu_use = 0;
	}

	//printf("%f\r\n",cpu_use);
	cpu_used_percentage_ = (uint32_t)(cpu_use * 100);
	
	last_cpu_stat_ = n;
#else
	
#endif
}


//网络流量
void sys_info::get_iostat() {
#ifndef _WIN32
	FILE *fd;
	char trim_buff[256];
	char* buff = (char*)malloc(256);
	char* pos;
	//loinfo("agv_shell") << "begin get iostat data";
  
    fd = fopen ("/proc/net/dev", "r");
    while( (buff = fgets(buff, 256, fd)) ) {
		pos = strstr(buff, network_name_.c_str());
		if(pos) {
			if( !nsp::toolkit::trim_space(pos, trim_buff, sizeof(trim_buff)) ) break;
			std::string ori_str = trim_buff;
			std::vector<std::string> v_str;
			split_symbol_string(ori_str, ' ', v_str);
			if(v_str.size() > 14) {
				if( !v_str[0].empty() ) {
					memcpy(net_iostat_->net_name, v_str[0].c_str(), v_str[0].size());
					net_iostat_->net_name[v_str[0].size()-1] = '0';
				}
				net_iostat_->rcv_bytes = atol(v_str[1].c_str());
				net_iostat_->rcv_packets = atol(v_str[2].c_str());
				net_iostat_->rcv_errs = atoi(v_str[3].c_str());
				net_iostat_->rcv_drop = atoi(v_str[4].c_str());
				net_iostat_->rcv_fifo = atoi(v_str[5].c_str());
				net_iostat_->send_bytes = atol(v_str[9].c_str());
				net_iostat_->send_packets = atol(v_str[10].c_str());
				net_iostat_->send_errs = atoi(v_str[11].c_str());
				net_iostat_->send_drop = atoi(v_str[12].c_str());
				net_iostat_->send_fifo = atoi(v_str[13].c_str());
			}
			break;
		}
	}
    fclose(fd);
#else
	
#endif
}

// /proc/version
void sys_info::get_version() {
#ifndef _WIN32
	FILE *fd;
	char *pchar = nullptr;
	char* buff = (char*)malloc(256);
	fd = fopen ("/proc/version", "r");
	buff = fgets (buff, 256, fd);
	if((pchar = strchr(buff,'\n'))!= nullptr){
		*pchar = '\0';
	}
	uname_info_ = buff;
	fclose(fd);
#else
	
#endif
}

//判断网卡是否为虚拟网卡
int sys_info::is_virtual_adapter(const char *pAdapterName, NET_ADAPTER_TYPE& ty)
{
	
#ifdef WIN32
	int ret_value = -1;
	char szDataBuf[MAX_PATH];
	DWORD dwDataLen = MAX_PATH;
	DWORD dwSz = REG_SZ;
	DWORD dwDword = REG_DWORD;
	HKEY hNetKey = NULL;
	HKEY hLocalNet = NULL;

	ty = NET_ADAPTER_TYPE::NCF_Unkown;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, NET_CARD_KEY, 0, KEY_READ, &hNetKey))return -1;
	DWORD NameCnt, NameMaxLen, KeySize, KeyCnt, KeyMaxLen, MaxDateLen;
	if (ERROR_SUCCESS != RegQueryInfoKey(hNetKey, NULL, NULL, NULL, &KeyCnt, &KeyMaxLen, NULL, &NameCnt, &NameMaxLen, &MaxDateLen, NULL, NULL))
	{
		RegCloseKey(hNetKey);
		return -1;
	}
	//枚举子键
	for (DWORD dwIndex = 0; dwIndex<KeyCnt; dwIndex++)
	{
		//因为RegQueryInfoKey得到的长度不包括0结束字符，所以应加1
		KeySize = KeyMaxLen + 1;         
		char* szKeyName = new char[KeySize];
		//参数定义请参照获取子键/键值信息部分
		if (ERROR_SUCCESS != RegEnumKeyEx(hNetKey, dwIndex, szKeyName, &KeySize, NULL, NULL, NULL, NULL))
		{
			RegCloseKey(hNetKey);
			return -1;
		};
		sprintf_s(szDataBuf, MAX_PATH,"\\%s", szKeyName);
		if (ERROR_SUCCESS != RegOpenKeyEx(hNetKey, szKeyName, 0, KEY_READ, &hLocalNet))
		{
			RegCloseKey(hNetKey);
			return -1;
		}
		delete[]szKeyName;
		memset(szDataBuf, 0, MAX_PATH);
		if (ERROR_SUCCESS != RegQueryValueEx(hLocalNet, "NetCfgInstanceId", 0, &dwSz, (BYTE *)szDataBuf, &dwDataLen))
		{
			goto ret;
		}
		if (strcmp(pAdapterName, szDataBuf) == 0)
		{
			memset(szDataBuf, 0, MAX_PATH);
			if (ERROR_SUCCESS != RegQueryValueEx(hLocalNet, "Characteristics", 0, &dwDword, (BYTE *)szDataBuf, &dwDataLen))
			{
				goto ret;
			}
			DWORD Characteristics;
			memcpy_s(&Characteristics, sizeof(Characteristics), szDataBuf, sizeof(Characteristics));
			if ((Characteristics & NCF_VIRTUAL) == NCF_VIRTUAL)
			{
				ty = NET_ADAPTER_TYPE::NCF_VIRTUAL;
			}
			if ((Characteristics & NCF_PHYSICAL) == NCF_PHYSICAL)
			{
				ty = NET_ADAPTER_TYPE::NCF_PHYSICAL;
			}
			break;
		}
	}
	ret_value = 0;
ret:
	RegCloseKey(hLocalNet);
	RegCloseKey(hNetKey);
	return ret_value;
#else
	return 0;
#endif

}


int sys_info::post_sysinfo_fixed(const uint32_t link, const unsigned char *buffer, int cb) {
	if( !buffer ) return -1;
	int ret = -1;

	agv::proto::proto_sysinfo_fixed_t ack_fixed_sysinfo(PKTTYPE_AGV_SHELL_GET_FIXED_SYSINFO_ACK);
	ack_fixed_sysinfo.head.id_ = *(uint32_t *)buffer;
	ack_fixed_sysinfo.head.type_ = PKTTYPE_AGV_SHELL_GET_FIXED_SYSINFO_ACK;
	ack_fixed_sysinfo.head.err_ = 0;

	ack_fixed_sysinfo.total_mem = sys_mem_.totalram;
	ack_fixed_sysinfo.cpu_num = cpu_info_.size();
	ack_fixed_sysinfo.disk_total_size = disk_info_.total_size;
	ack_fixed_sysinfo.uname = uname_info_;
	ack_fixed_sysinfo.soft_version = nsp::toolkit::singleton<global_parameter>::instance()->get_shell_version();
	ack_fixed_sysinfo.config_version = nsp::toolkit::singleton<global_parameter>::instance()->get_config_version();
	ack_fixed_sysinfo.status = nsp::toolkit::singleton<global_parameter>::instance()->query_file_lock();
	ack_fixed_sysinfo.ntp_server = nsp::toolkit::singleton<global_parameter>::instance()->get_ntp_server();
	
	agv::proto::proto_cpu_info_t proto_cpu;
	for(unsigned int i = 0; i < cpu_info_.size(); ++i) {
		proto_cpu.name = cpu_info_[i].name;
		proto_cpu.hz = cpu_info_[i].hz;
		ack_fixed_sysinfo.cpu_list.push_back(proto_cpu);
	}
	
	get_process_config_info(ack_fixed_sysinfo.pocess_info);

	ack_fixed_sysinfo.head.size_ = ack_fixed_sysinfo.length();
	
	ret = nsp::toolkit::singleton<agv_shell_server>::instance()->post_pkgTsession_bylink(link, &ack_fixed_sysinfo);
	return ret;
}

int sys_info::post_sysinfo_changed(const uint32_t link, const unsigned char *buffer, int cb) {
	if( !buffer ) return -1;
	int ret = -1;
	
	get_cpu_usage();
	get_devmem();
	get_iostat();
	posix__getsysmem(&sys_mem_);
	
	print_statinfo(); //for test
	
	agv::proto::proto_keepalive_ack_t ack_keepalive(PKTTYPE_AGV_SHELL_KEEPALIVE_ACK);
	ack_keepalive.head.id_ = *(uint32_t *)buffer;
    ack_keepalive.head.err_ = 0;
	ack_keepalive.sysinfo.cpu_percentage = cpu_used_percentage_;
	ack_keepalive.sysinfo.free_mem = sys_mem_.freeram;
	ack_keepalive.sysinfo.total_swap = sys_mem_.totalswap;
	ack_keepalive.sysinfo.free_swap = sys_mem_.freeswap;
	ack_keepalive.sysinfo.disk_used_size = disk_info_.used_size;
	ack_keepalive.sysinfo.uptime = nsp::os::gettick();
	ack_keepalive.sysinfo.host_time = time(NULL);
	if(net_iostat_) {
		ack_keepalive.sysinfo.net_io_rec = net_iostat_->rcv_bytes;
		ack_keepalive.sysinfo.net_io_tra = net_iostat_->send_bytes;
	}
	
	//进程状态信息 
	get_process_run_info(ack_keepalive.process_list);
	
	ack_keepalive.reserve.resize(KEEPALIVE_RESERVE_SIZE);
	
	ack_keepalive.head.size_ = ack_keepalive.length();
    ret = nsp::toolkit::singleton<agv_shell_server>::instance()->post_pkgTsession_bylink(link, &ack_keepalive);
	return ret;
}

void sys_info::print_statinfo() {
	//cpu info
	loinfo("agv_shell")<< "cpu count:" << cpu_info_.size();
	for(unsigned int i = 0; i < cpu_info_.size(); ++i) {
		loinfo("agv_shell")<< "cpu " << i << " model name:" << cpu_info_[i].name << ", hz:" << cpu_info_[i].hz;
	}
	
	//cpu stat info
	loinfo("agv_shell")<< "cpu stat info, user:" << last_cpu_stat_.user << ", nice:" << last_cpu_stat_.nice << ", system:" << last_cpu_stat_.system;
	loinfo("agv_shell")<< "\t idle:" << last_cpu_stat_.idle << ", lowait:" << last_cpu_stat_.lowait << ", irq:" <<last_cpu_stat_.irq <<", softirq:" <<last_cpu_stat_.softirq; 
	loinfo("agv_shell")<< "cpu_used_percentage_:" << cpu_used_percentage_;
	
	//memory stat info
	loinfo("agv_shell")<< "memory stat info, totalram:" <<sys_mem_.totalram <<", freeram:" <<sys_mem_.freeram <<", totalswap:" <<sys_mem_.totalswap<<", freeswap:" << sys_mem_.freeswap;
	
	//net io stat info
	/*loinfo("agv_shell")<< "net_iostat net_name:" << net_iostat_->net_name << ", receive bytes:"<< net_iostat_->rcv_bytes <<", packets:" << net_iostat_->rcv_packets;
	loinfo("agv_shell")<< "\t errs:"<< net_iostat_->rcv_errs << ", drop:"<< net_iostat_->rcv_drop <<", fifo:"<< net_iostat_->rcv_fifo;
	loinfo("agv_shell")<< "send bytes:"<< net_iostat_->send_bytes <<", packets:" << net_iostat_->send_packets << ", errs:"<< net_iostat_->send_errs;
	loinfo("agv_shell")<< "\t drop:"<< net_iostat_->send_drop <<", fifo:"<< net_iostat_->send_fifo;

	//disk stat info
	loinfo("agv_shell") << "disk, total:" << disk_info_.total_size << ", used size:" << disk_info_.used_size;
	
	loinfo("agv_shell") << "uname:" << uname_info_;
	loinfo("agv_shell") << "mac addr:" << mac_addr_;*/
}


