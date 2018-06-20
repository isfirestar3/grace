/*	load layout.xml and dump to file by binary format. */
#include "map_test.h"
#include "var.h"
#include "object.h"
#include "posix_ifos.h"

int main()
{
	objinit();
	
	if (var__load_layout() < 0) {
		printf("load xml failure!!\n");
		return -1;
	}
	printf("var__load_layout load xml suceess!!\n");
	
	if ( var__dump2file() < 0) {
		printf("load xml failure!!\n");
		return -1;
	}
	
	printf("var__dump2file suceess!!\n");
	
	
	if ( var__load_layout_ffile() < 0) {
		printf("load xml failure!!\n");
		return -1;
	}
	
	printf("var__load_layout_ffile suceess!!\n");
	
	var__free_layout();
	
	return 0;
}
