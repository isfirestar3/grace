#include "application.h"
#include "log.h"
#include "net_task.h"
#include "common/watcherwaiter.hpp"
#include <iostream>

namespace
{
	static const int ARGS_COUNT = 5;
	static const char*Args[] = {"-ip", "-port"};
}

void Application::ShowHelp() const
{
	std::cerr << "Parameter critical!Usage:\"RealtimeViewer -ip ipaddr -port port\".\n.ie.\"RealtimeViewer -ip 127.0.0.1 -port 4409\"." << std::endl;
}

bool Application::Parse()
{
	if (ARGS_COUNT != m_argc)
	{
		ShowHelp();

		return false;
	}

	QString ip = m_argv[1];
	QString ipArgs = Args[0];

	if (ip != ipArgs)
	{
		ShowHelp();

		return false;
	}

	QString portStr = m_argv[3];
	QString portStrArgs = Args[1];

	if (portStr != portStrArgs)
	{
		ShowHelp();

		return false;
	}

	QString ipAddr = m_argv[2];
	bool ok = false;
	int port = QString(m_argv[4]).toInt(&ok);

	if (!ok)
	{
		return false;
	}

	emit ConnectToHost(ipAddr, port);

	return true;
}
