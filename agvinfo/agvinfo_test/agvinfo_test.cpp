#include "libagvinfo.h"

#include <unistd.h>

int main(int argc, char **atgv) {
	connect_agvinfo_server("127.0.0.1", 9100);
	while(1) sleep(1);
	return 0;
}
