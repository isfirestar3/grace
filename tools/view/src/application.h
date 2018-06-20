#ifndef APPLICATION_H
#define APPLICATION_H

#include "singleton.hpp"
#include <QtWidgets/QApplication>
#include <cstdint>
#include <functional>

class Application : public QObject
{
	Q_OBJECT
	friend nsp::toolkit::singleton<Application>;
public:
	int GetArgc() const
	{
		return m_argc;
	}

	void SetArgc(int argc)
	{
		m_argc = argc;
	}

	void SetArgv(char **argv)
	{
		m_argv = argv;
	}

	void ShowHelp() const;
	bool Parse();
signals:
	void ConnectToHost(const QString &, int);
private:
	int m_argc;
	char **m_argv;

	Application(){};
	Application(const Application &) = delete;
	Application &operator =(const Application &) = delete;
};

#endif // APPLICATION_H
