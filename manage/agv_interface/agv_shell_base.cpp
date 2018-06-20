#include "agv_shell_base.h"
#include "net_manager.h"
#include "agv_shell_session.h"
#include "agv_shell_alive.h"

static
agv_shell_alive alive_th_;

agv_shell_base::agv_shell_base()
{
}

agv_shell_base::~agv_shell_base()
{
	disconnect_agv_shell();
}

int agv_shell_base::login_agv_shell(const std::string& ipv4, const uint16_t port)
{
	nsp::tcpip::endpoint ep;
	char epstr[32];
	sprintf(epstr, "%s:%u", ipv4.c_str(), port);
	if (nsp::tcpip::endpoint::build(epstr, ep) < 0) {
		return -EINVAL;
	}
	if (client_session_)
	{
		loerror("agv_shell_interface") << "the client session is already exist,so can not make client session.";
		return -1;
	}
	std::shared_ptr<agv_shell_session> client;
	try
	{
		client = std::make_shared<agv_shell_session>();
		if (client->try_connect(ep) < 0) {
			loerror("agv_shell_interface") << "failed to connect to target:" << client->get_target_endpoint().to_string() << " the link is " << client->get_link();
			return -1;
		}
		//child class convert to base class
		client_session_ = std::dynamic_pointer_cast<nsp::tcpip::tcp_application_client<nsp::proto::nspdef::protocol>>(client);
		alive_th_.add_session(client_session_);
	}
	catch (...)
	{
		loerror("agv_shell_interface") << "failed to make share ptr of agv_shell session";
		return -1;
	}

	return 0;
}

void agv_shell_base::disconnect_agv_shell()
{
	if(client_session_)client_session_->close();
	alive_th_.remove_session(client_session_);
}

int agv_shell_base::get_agv_shell_process_list(std::vector<process_info>& vct_process)
{
	if (!client_session_)
	{
		loerror("agv_shell_interface") << "the client session is empty.";
		return -1;
	}
	motion::asio_data a_data;
	nsp::os::waitable_handle waiter(0);
	process_list_info list_info;

	{
		std::lock_guard<decltype(lock_mutex_)> lock(lock_mutex_);
		list_process_.clear();
	}
	
	//base class convert to base class
	std::shared_ptr<agv_shell_session> client;
	try
	{
		client = std::static_pointer_cast<agv_shell_session>(client_session_);
	}
	catch (...)
	{
		loerror("agv_shell_interface") << "failed to convert bacs class to child class.";
		return -1;
	}
	if (client->is_network_conntected() < 0)
	{
		loerror("agv_shell_interface") << "failed to get agv_shell process list,the target " << client->get_target_endpoint().to_string() 
			<< " and link is: " << client->get_link() << " network status isn't connected.";
		return -2;
	}
	int res = client->post_get_process_list(std::make_shared<motion::asio_block>([&](const void*data){
		if (!data){
			waiter.sig();
			return;
		}
		a_data = *(motion::asio_data*)data;
		if (a_data.get_err() < 0){
			waiter.sig();
			return;
		}
		list_info = *(process_list_info*)data;
		waiter.sig();
	}));
	if (res < 0){
		loerror("agv_shell_interface") << "failed to post get process list package to target:" << client->get_target_endpoint().to_string();
		client->close();
		//ÒÆ³ýclient_session
		
		return -1;
	}
	waiter.wait();
	if (a_data.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		vct_process = list_info.vct_process_;

		std::lock_guard<decltype(lock_mutex_)> lock(lock_mutex_);
		list_process_ = list_info.vct_process_;
		return 0;
	}
	else if (a_data.get_err() == nsp::proto::errorno_t::kRequestTimeout)
	{
		loerror("agv_shell_interface") << "get a timeout result while post get agv_shell process list,the target endpoint is:" 
			<< client->get_target_endpoint().to_string();
	}
	return -1;
}

int agv_shell_base::post_agv_shell_process_cmd(const std::vector<process_command>& vct_p_command,
	const command_agv_shell cmd)
{
	if (!client_session_)
	{
		loerror("agv_shell_interface") << "the client session is empty.";
		return -1;
	}
	//base class convert to child class
	std::shared_ptr<agv_shell_session> client;
	try
	{
		client = std::static_pointer_cast<agv_shell_session>(client_session_);
	}
	catch (...)
	{
		loerror("agv_shell_interface") << "failed to convert bacs class to child class.";
		return -1;
	}
	if (client->is_network_conntected() < 0)
	{
		loerror("agv_shell_interface") << "failed to post agv_shell process cmd,the target "<< client->get_target_endpoint().to_string() 
			<< " and link is: " << client->get_link() << " network status isn't connected.";
		return -2;
	}
	int cmd_process = 0;
	std::vector<std::string> vct_p_cmd;

	{
		std::lock_guard<decltype(lock_mutex_)> lock(lock_mutex_);
		if (list_process_.size() == 0){
			lowarn("agv_shell_interface") << "the target " << client->get_target_endpoint().to_string() << " and link is: " << client->get_link()
				<< " local process list is empty.";
		}
		for (const auto& p_cmd : list_process_)
		{
#if _WIN32
			auto orig_iter = std::find_if(vct_p_command.begin(), vct_p_command.end(), [&](const process_command& p_info)->bool {
				return p_info.p_id_ == p_cmd.process_id ? true : false;
			});
			if (orig_iter != vct_p_command.end())
			{
				cmd_process = cmd_process | (1 << orig_iter->p_id_);
				vct_p_cmd.push_back(orig_iter->p_command_);
			}
			else
			{
				vct_p_cmd.push_back("");
			}
#else
			auto iter = vct_p_command.begin();
			for (; iter != vct_p_command.end(); ++iter) {
				if (p_cmd.process_id == iter->p_id_)
				{
					cmd_process = cmd_process | (1 << iter->p_id_);
					vct_p_cmd.push_back(iter->p_command_);
					break;
				}
			}
#endif
		}
	}

	int result = client->post_agv_shell_process_cmd(cmd_process, vct_p_cmd, cmd);
	if (result < 0)
	{
		lowarn("agv_shell_interface") << "failed post agv shell process command to client: " << client->get_target_endpoint().to_string() << 
			",the result:" << result << " ,the close the session.";
		client->close();
		//ÒÆ³ýclient_session
		
	}
	return result;
}

int agv_shell_base::get_agv_shell_process_status(std::vector<process_status>& vct_p)
{
	if (!client_session_)
	{
		loerror("agv_shell_interface") << "the client session is empty.";
		return -1;
	}
	std::shared_ptr<agv_shell_session> client;
	try
	{
		client = std::static_pointer_cast<agv_shell_session>(client_session_);
	}
	catch (...)
	{
		loerror("agv_shell_interface") << "failed to convert bacs class to child class.";
		return -1;
	}
	if (client->is_network_conntected() < 0)
	{
		loerror("agv_shell_interface") << "failed to get agv_shell process status,the target " << client->get_target_endpoint().to_string()
			<< " and link is: " << client->get_link() << " network status isn't connected.";
		return -2;
	}
	client->get_agv_shell_process_status(vct_p);
	return 0;
}
