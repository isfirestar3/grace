#include "sys_info.h"
#include <vector>
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

#include "agv_shell_common.h"
#include "log.h"

#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))

sys_info::sys_info()
{

}

sys_info::~sys_info()
{

}

std::string sys_info::get_mac_addr()
{
	std::string mac_addr("");
#ifdef _WIN32
	PIP_ADAPTER_INFO apter_info;
	ULONG output = sizeof(_IP_ADAPTER_INFO);
	apter_info = (IP_ADAPTER_INFO *)MALLOC(sizeof(IP_ADAPTER_INFO));
	if (apter_info == NULL) {
		printf("Error allocating memory needed to call GetAdaptersinfo\n");
		return mac_addr;
	}
	// Make an initial call to GetAdaptersInfo to get
	// the necessary size into the ulOutBufLen variable
	if (GetAdaptersInfo(apter_info, &output) == ERROR_BUFFER_OVERFLOW)
	{
		FREE(apter_info);
		apter_info = (IP_ADAPTER_INFO *)MALLOC(output);
		if (apter_info == NULL) {
			printf("Error allocating memory needed to call GetAdaptersinfo\n");
			return mac_addr;
		}
	}
	PIP_ADAPTER_INFO pAdapter = NULL;
	if (GetAdaptersInfo(apter_info, &output) == NO_ERROR) {
		pAdapter = apter_info;
		std::string ipv4;
		while (pAdapter) {
			char mac_chr[64] = { 0 };
			for (int i = 0; i < pAdapter->AddressLength; i++) {
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
			mac_addr = mac_chr;
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

	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		loerror("agv_shell") << __FILE__ <<":" << __LINE__ << " socket error:" << strerror(errno);
		close(fd);
		return mac_addr;
	}

	ifc.ifc_len = sizeof(buf);
	ifc.ifc_buf = (caddr_t)buf;
	if (ioctl(fd, SIOCGIFCONF, (char *)&ifc)) {
		loerror("agv_shell") << __FILE__ <<":" << __LINE__ << " ioctl get all net error:" << strerror(errno);
		close(fd);
		return mac_addr;
	}
	
	int interfaceNum = ifc.ifc_len / sizeof(struct ifreq);
	loinfo("agv_shell") << "interface num:" << interfaceNum;
	char ip[32] = {0};
	std::string ipv4;
	
	while (interfaceNum-- > 0) {
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
		loinfo("agv_shell") << "device ip:" << ip;
		if (nsp::toolkit::singleton<global_parameter>::instance()->get_local_ipv4() != ipv4) {
			loinfo("agv_shell") << "device ip:" << ip;
			loinfo("agv_shell") << "local ip:" << nsp::toolkit::singleton<global_parameter>::instance()->get_local_ipv4();
			continue;
		}

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
		mac_addr = mac;
		loinfo("agv_shell") << "device mac:" << mac_addr;
		break;
	}

	close(fd);
#endif

	return mac_addr;
}

//判断网卡是否为虚拟网卡
bool sys_info::is_virtual_adapter(const char *pAdapterName, NET_ADAPTER_TYPE& ty)
{
	
#ifdef WIN32
	BOOL ret_value = FALSE;
	char szDataBuf[MAX_PATH];
	DWORD dwDataLen = MAX_PATH;
	DWORD dwSz = REG_SZ;
	DWORD dwDword = REG_DWORD;
	HKEY hNetKey = NULL;
	HKEY hLocalNet = NULL;

	ty = NET_ADAPTER_TYPE::NCF_Unkown;
	if (ERROR_SUCCESS != RegOpenKeyEx(HKEY_LOCAL_MACHINE, NET_CARD_KEY, 0, KEY_READ, &hNetKey))return false;
	DWORD NameCnt, NameMaxLen, KeySize, KeyCnt, KeyMaxLen, DateSize, MaxDateLen;
	if (ERROR_SUCCESS != RegQueryInfoKey(hNetKey, NULL, NULL, NULL, &KeyCnt, &KeyMaxLen, NULL, &NameCnt, &NameMaxLen, &MaxDateLen, NULL, NULL))
	{
		RegCloseKey(hNetKey);
		return false;
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
			return false;
		};
		sprintf_s(szDataBuf, MAX_PATH,"\\%s", szKeyName);
		if (ERROR_SUCCESS != RegOpenKeyEx(hNetKey, szKeyName, 0, KEY_READ, &hLocalNet))
		{
			RegCloseKey(hNetKey);
			return false;
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
	ret_value = TRUE;
ret:
	RegCloseKey(hLocalNet);
	RegCloseKey(hNetKey);
	return ret_value != 0;
#else
	return true;
#endif

}
