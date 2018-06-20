#pragma once

#include <mutex>
#include <map>
#include <vector>
#include <string>
#include "singleton.hpp"

class receiver_write_manage{
	receiver_write_manage();
	~receiver_write_manage();
	friend class nsp::toolkit::singleton<receiver_write_manage>;

public:
	bool search_write_file(const std::string& file_path, uint32_t& link);
	void add_write_file(const uint32_t link, const std::string& file_path);
	void remove_write_file(const uint32_t link);

private:
	std::recursive_mutex re_mutex_;
	std::map<uint32_t,std::string> map_write_files_;
};