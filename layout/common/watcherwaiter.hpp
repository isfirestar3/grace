#ifndef WATCHERDIALOG_H
#define WATCHERDIALOG_H

#include "waitdialog.h"
#include "singleton.hpp"
#include "common_impls.hpp"
#include <QtConcurrent/QtConcurrent>
#include <functional>

namespace shr
{
	namespace gui
	{
		class WatcherDialog
		{
			friend nsp::toolkit::singleton<WatcherDialog>;
		public:
			template <typename Class, typename Ret>
			void run(Class *object, Ret(Class::*fn)(), const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				QFuture<Ret> future = QtConcurrent::run(object, fn);
				futureWatcher.setFuture(future);
				dlg.exec();

				if (nullptr != cb)
				{
					Ret ret = futureWatcher.result();
					cb(ret);
				}
			}

			template <typename Class>
			void run(Class *object, void(Class::*fn)(), QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				QFuture<void> future = QtConcurrent::run(object, fn);
				futureWatcher.setFuture(future);
				dlg.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1>
			void run(Class *object, Ret(Class::*fn)(Param1), const Arg1 &arg1, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1);
				futureWatcher.setFuture(future);
				dlg.exec();

				if (nullptr != cb)
				{
					Ret ret = futureWatcher.result();
					cb(ret);
				}
			}

			template <typename Class, typename Param1, typename Arg1>
			void run(Class *object, void(Class::*fn)(Param1), const Arg1 &arg1, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1);
				futureWatcher.setFuture(future);
				dlg.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2);
				futureWatcher.setFuture(future);
				dlg.exec();

				if (nullptr != cb)
				{
					Ret ret = futureWatcher.result();
					cb(ret);
				}
			}

			template <typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
			void run(Class *object, void(Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1, arg2);
				futureWatcher.setFuture(future);
				dlg.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3);
				futureWatcher.setFuture(future);
				dlg.exec();

				if (nullptr != cb)
				{
					Ret ret = futureWatcher.result();
					cb(ret);
				}
			}

			template <typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1, arg2, arg3);
				futureWatcher.setFuture(future);
				dlg.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4);
				futureWatcher.setFuture(future);
				dlg.exec();

				if (nullptr != cb)
				{
					Ret ret = futureWatcher.result();
					cb(ret);
				}
			}

			template <typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4);
				futureWatcher.setFuture(future);
				dlg.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4, arg5);
				futureWatcher.setFuture(future);
				dlg.exec();

				if (nullptr != cb)
				{
					Ret ret = futureWatcher.result();
					cb(ret);
				}
			}

			template <typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4, arg5);
				futureWatcher.setFuture(future);
				dlg.exec();
			}
		private:
			WatcherDialog(){}
			Q_DISABLE_COPY(WatcherDialog);
		};

		class WatcherWaiter
		{
			friend nsp::toolkit::singleton<WatcherWaiter>;
		public:
			template <typename Class, typename Ret>
			void run(Class *object, Ret(Class::*fn)(), const std::function<void(Ret)> &cb = nullptr)
			{
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn);
				futureWatcher.setFuture(future);
				eventLoop.exec();

				if (nullptr != cb)
				{
					Ret ret = futureWatcher.result();
					cb(ret);
				}
			}

			template <typename Class>
			void run(Class *object, void(Class::*fn)())
			{
				QEventLoop eventLoop;
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<void> future = QtConcurrent::run(object, fn);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1>
			void run(Class *object, Ret(Class::*fn)(Param1), const Arg1 &arg1, const std::function<void(Ret)> &cb = nullptr)
			{
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1);
				futureWatcher.setFuture(future);
				eventLoop.exec();

				if (nullptr != cb)
				{
					Ret ret = futureWatcher.result();
					cb(ret);
				}
			}

			template <typename Class, typename Param1, typename Arg1>
			void run(Class *object, void(Class::*fn)(Param1), const Arg1 &arg1)
			{
				QEventLoop eventLoop;
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2, const std::function<void(Ret)> &cb = nullptr)
			{
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2);
				futureWatcher.setFuture(future);
				eventLoop.exec();

				if (nullptr != cb)
				{
					Ret ret = futureWatcher.result();
					cb(ret);
				}
			}

			template <typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
			void run(Class *object, void(Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2)
			{
				QEventLoop eventLoop;
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1, arg2);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const std::function<void(Ret)> &cb = nullptr)
			{
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3);
				futureWatcher.setFuture(future);
				eventLoop.exec();

				if (nullptr != cb)
				{
					Ret ret = futureWatcher.result();
					cb(ret);
				}
			}

			template <typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3)
			{
				QEventLoop eventLoop;
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1, arg2, arg3);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const std::function<void(Ret)> &cb = nullptr)
			{
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4);
				futureWatcher.setFuture(future);
				eventLoop.exec();

				if (nullptr != cb)
				{
					Ret ret = futureWatcher.result();
					cb(ret);
				}
			}

			template <typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4)
			{
				QEventLoop eventLoop;
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, const std::function<void(Ret)> &cb = nullptr)
			{
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4, arg5);
				futureWatcher.setFuture(future);
				eventLoop.exec();

				if (nullptr != cb)
				{
					Ret ret = futureWatcher.result();
					cb(ret);
				}
			}

			template <typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5)
			{
				QEventLoop eventLoop;
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4, arg5);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}
		private:
			WatcherWaiter(){}
			Q_DISABLE_COPY(WatcherWaiter);
		};
	}
}

#endif // WATCHERDIALOG_H
