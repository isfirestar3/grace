#ifndef WATCHERDIALOG_H
#define WATCHERDIALOG_H
#include "waitdialog.h"
#include "FutureResultReceiver.hpp"
#include "gdp_singleton.hpp"
#include <QtConcurrent/QtConcurrent>
#include <functional>

namespace gdp
{
	namespace gui
	{
		class WatcherDialog
		{
			Q_DISABLE_COPY(WatcherDialog);
			friend gdp::core::GdpSingleton<WatcherDialog>;
		public:
			template <typename Class, typename Ret>
			void run(Class *object, Ret(Class::*fn)(), QWidget *parent = nullptr, const std::function<void(Ret)> &retCallback = nullptr, const std::function<void()> &exceptionCallback = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				WaitDialog dlg(parent);
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				QFuture<Ret> future = QtConcurrent::run(object, fn);
				futureWatcher.setFuture(future);
				dlg.exec();

				if (nullptr != retCallback)
				{
					try
					{
						Ret ret = futureWatcher.result();
						retCallback(ret);
					}
					catch (...)
					{
						if (nullptr != exceptionCallback)
						{
							exceptionCallback();
						}
					}
				}
			}

			template <typename Class>
			void run(Class *object, void(Class::*fn)(), QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				QFuture<void> future = QtConcurrent::run(object, fn);
				futureWatcher.setFuture(future);
				dlg.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1>
			void run(Class *object, Ret(Class::*fn)(Param1), const Arg1 &arg1, QWidget *parent = nullptr, const std::function<void(Ret)> &retCallback = nullptr, const std::function<void()> &exceptionCallback = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				WaitDialog dlg(parent);
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1);
				futureWatcher.setFuture(future);
				dlg.exec();

				if (nullptr != retCallback)
				{
					try
					{
						Ret ret = futureWatcher.result();
						retCallback(ret);
					}
					catch (...)
					{
						if (nullptr != exceptionCallback)
						{
							exceptionCallback();
						}
					}
				}
			}

			template <typename Class, typename Param1, typename Arg1>
			void run(Class *object, void(Class::*fn)(Param1), const Arg1 &arg1, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1);
				futureWatcher.setFuture(future);
				dlg.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2, QWidget *parent = nullptr, const std::function<void(Ret)> &retCallback = nullptr, const std::function<void()> &exceptionCallback = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				WaitDialog dlg(parent);
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2);
				futureWatcher.setFuture(future);
				dlg.exec();

				if (nullptr != retCallback)
				{
					try
					{
						Ret ret = futureWatcher.result();
						retCallback(ret);
					}
					catch (...)
					{
						if (nullptr != exceptionCallback)
						{
							exceptionCallback();
						}
					}
				}
			}

			template <typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2>
			void run(Class *object, void(Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1, arg2);
				futureWatcher.setFuture(future);
				dlg.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, QWidget *parent = nullptr, const std::function<void(Ret)> &retCallback = nullptr, const std::function<void()> &exceptionCallback = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				WaitDialog dlg(parent);
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3);
				futureWatcher.setFuture(future);
				dlg.exec();

				if (nullptr != retCallback)
				{
					try
					{
						Ret ret = futureWatcher.result();
						retCallback(ret);
					}
					catch (...)
					{
						if (nullptr != exceptionCallback)
						{
							exceptionCallback();
						}
					}
				}
			}

			template <typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1, arg2, arg3);
				futureWatcher.setFuture(future);
				dlg.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, QWidget *parent = nullptr, const std::function<void(Ret)> &retCallback = nullptr, const std::function<void()> &exceptionCallback = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				WaitDialog dlg(parent);
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4);
				futureWatcher.setFuture(future);
				dlg.exec();

				if (nullptr != retCallback)
				{
					try
					{
						Ret ret = futureWatcher.result();
						retCallback(ret);
					}
					catch (...)
					{
						if (nullptr != exceptionCallback)
						{
							exceptionCallback();
						}
					}
				}
			}

			template <typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4);
				futureWatcher.setFuture(future);
				dlg.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, QWidget *parent = nullptr, const std::function<void(Ret)> &retCallback = nullptr, const std::function<void()> &exceptionCallback = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				WaitDialog dlg(parent);
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4, arg5);
				futureWatcher.setFuture(future);
				dlg.exec();

				if (nullptr != retCallback)
				{
					try
					{
						Ret ret = futureWatcher.result();
						retCallback(ret);
					}
					catch (...)
					{
						if (nullptr != exceptionCallback)
						{
							exceptionCallback();
						}
					}
				}
			}

			template <typename Class, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				QFuture<void> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4, arg5);
				futureWatcher.setFuture(future);
				dlg.exec();
			}

			// map() on sequences
			template <typename Sequence, typename MapFunctor>
			void map(Sequence &sequence, MapFunctor map, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				futureWatcher.setFuture(QtConcurrent::map(sequence, map));
				dlg.exec();
			}

			// map() on iterators
			template <typename Iterator, typename MapFunctor>
			void map(Iterator begin, Iterator end, MapFunctor map, QWidget *parent = nullptr)
			{
				WaitDialog dlg(parent);
				QFutureWatcher<void> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<void>::finished, &dlg, &QDialog::accept);
				futureWatcher.setFuture(QtConcurrent::map(begin, end, map));
				dlg.exec();
			}

			// mapped() for sequences
			template <typename Ret, typename Sequence, typename MapFunctor>
			void mapped(const Sequence &sequence, MapFunctor map, QWidget *parent = nullptr, const std::function<void(Ret)> &retCallback = nullptr, const std::function<void()> &exceptionCallback = nullptr)
			{
				WaitDialog dlg(parent);
				FutureResultReceiver<Ret> futureResultReceiver;
				futureResultReceiver.setResultCallback(retCallback);
				futureResultReceiver.setExceptionCallback(exceptionCallback);
				QFutureWatcher<Ret> &futureWatcher = futureResultReceiver.getFutureWatcher();
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::resultReadyAt, &futureResultReceiver, &FutureResultReceiver<Ret>::resultAt);
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				futureWatcher.setFuture(QtConcurrent::mapped(sequence, map));
				dlg.exec();
			}

			// mapped() for iterator ranges.
			template <typename Ret, typename Iterator, typename MapFunctor>
			void mapped(Iterator begin, Iterator end, MapFunctor map, QWidget *parent = nullptr, const std::function<void(Ret)> &retCallback = nullptr, const std::function<void()> &exceptionCallback = nullptr)
			{
				WaitDialog dlg(parent);
				FutureResultReceiver<Ret> futureResultReceiver;
				futureResultReceiver.setResultCallback(retCallback);
				futureResultReceiver.setExceptionCallback(exceptionCallback);
				QFutureWatcher<Ret> &futureWatcher = futureResultReceiver.getFutureWatcher();
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::resultReadyAt, &futureResultReceiver, &FutureResultReceiver<Ret>::resultAt);
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &dlg, &QDialog::accept);
				futureWatcher.setFuture(QtConcurrent::mapped(begin, end, map));
				dlg.exec();
			}
		private:
			WatcherDialog(){}
		};
	}
}

#endif // WATCHERDIALOG_H
