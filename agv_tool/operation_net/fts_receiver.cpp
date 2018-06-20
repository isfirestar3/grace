#include "fts_receiver.h"
#include "log.h"

fts_receiver::fts_receiver() {
	initlization();
}

fts_receiver::~fts_receiver() {
	uint();
}

void fts_receiver::initlization() {
	fts_parameter tmp;
	memset(tmp.file_search_root, 0, sizeof(tmp.file_search_root));
	tmp.timeout_cancel_transfer = 10000;
	fts_change_configure(&tmp);
	if (fts_create_receiver("0.0.0.0", 4412) < 0) {
		loerror("operation_net") << "failed to create fts server to receive frimware bin file.";
	}
}

void fts_receiver::uint() {
	fts_destory_receiver("0.0.0.0", 4412);
}