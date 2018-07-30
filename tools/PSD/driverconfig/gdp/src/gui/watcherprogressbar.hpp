#ifndef WATCHERPROGRESSBAR_H
#define WATCHERPROGRESSBAR_H

#include "progressbar.h"
#include "guitoolkit.h"
#include <QProgressBar>
#include <QtConcurrent/QtConcurrent>
#include <functional>

namespace gdp
{
	namespace gui
	{
		namespace
		{
			static const int PROGRESSBARV1_WIDTH = 200;
			static const int PROGRESSBARV1_HEIGHT = 30;
			static const int PROGRESSBARV2_WIDTH = 200;
			static const int PROGRESSBARV2_HEIGHT = 200;
		}

		class WatcherProgressBarV1
		{
			Q_DISABLE_COPY(WatcherProgressBarV1);
			friend nsp::toolkit::singleton<WatcherProgressBarV1>;
		public:
			template <typename Class, typename Ret>
			void run(Class *object, Ret(Class::*fn)(), const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				QProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV1_WIDTH, PROGRESSBARV1_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
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
			void run(Class *object, void(Class::*fn)(), QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				QProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV1_WIDTH, PROGRESSBARV1_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1>
			void run(Class *object, Ret(Class::*fn)(Param1), const Arg1 &arg1, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				QProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV1_WIDTH, PROGRESSBARV1_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
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
			void run(Class *object, void(Class::*fn)(Param1), const Arg1 &arg1, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				QProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV1_WIDTH, PROGRESSBARV1_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				QProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV1_WIDTH, PROGRESSBARV1_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
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
			void run(Class *object, void(Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				QProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV1_WIDTH, PROGRESSBARV1_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				QProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV1_WIDTH, PROGRESSBARV1_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
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
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				QProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV1_WIDTH, PROGRESSBARV1_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				QProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV1_WIDTH, PROGRESSBARV1_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
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
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				QProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV1_WIDTH, PROGRESSBARV1_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				QProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV1_WIDTH, PROGRESSBARV1_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
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
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				QProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV1_WIDTH, PROGRESSBARV1_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4, arg5);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}
		private:
			WatcherProgressBarV1(){}
		};

		class WatcherProgressBarV2
		{
			Q_DISABLE_COPY(WatcherProgressBarV2);
			friend nsp::toolkit::singleton<WatcherProgressBarV2>;
		public:
			template <typename Class, typename Ret>
			void run(Class *object, Ret(Class::*fn)(), const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				ProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV2_WIDTH, PROGRESSBARV2_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
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
			void run(Class *object, void(Class::*fn)(), QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				ProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV2_WIDTH, PROGRESSBARV2_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1>
			void run(Class *object, Ret(Class::*fn)(Param1), const Arg1 &arg1, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				ProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV2_WIDTH, PROGRESSBARV2_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
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
			void run(Class *object, void(Class::*fn)(Param1), const Arg1 &arg1, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				ProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV2_WIDTH, PROGRESSBARV2_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				ProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV2_WIDTH, PROGRESSBARV2_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
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
			void run(Class *object, void(Class::*fn)(Param1, Param2), const Arg1 &arg1, const Arg2 &arg2, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				ProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV2_WIDTH, PROGRESSBARV2_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				ProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV2_WIDTH, PROGRESSBARV2_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
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
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				ProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV2_WIDTH, PROGRESSBARV2_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				ProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV2_WIDTH, PROGRESSBARV2_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
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
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3, Param4), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				ProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV2_WIDTH, PROGRESSBARV2_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}

			template <typename Class, typename Ret, typename Param1, typename Arg1, typename Param2, typename Arg2, typename Param3, typename Arg3, typename Param4, typename Arg4, typename Param5, typename Arg5>
			void run(Class *object, Ret(Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, const std::function<void(Ret)> &cb = nullptr, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				ProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV2_WIDTH, PROGRESSBARV2_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
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
			void run(Class *object, void(Class::*fn)(Param1, Param2, Param3, Param4, Param5), const Arg1 &arg1, const Arg2 &arg2, const Arg3 &arg3, const Arg4 &arg4, const Arg5 &arg5, QWidget *parent = nullptr)
			{
				if (nullptr == object)
				{
					return;
				}

				ProgressBar progressBar;
				progressBar.setFixedSize(PROGRESSBARV2_WIDTH, PROGRESSBARV2_HEIGHT);

				if (nullptr != parent)
				{
					progressBar.setParent(parent);
				}
				else
				{
					progressBar.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowStaysOnTopHint);
					parent = nsp::toolkit::singleton<GuiToolkit>::instance()->getTopWidget();
				}

				if (nullptr != parent)
				{
					const QRect &rect = parent->geometry();
					int x = (rect.width() - progressBar.width()) / 2 + rect.x();
					int y = (rect.height() - progressBar.height()) / 2 + rect.y();
					progressBar.setGeometry(x, y, progressBar.width(), progressBar.height());
				}

				progressBar.show();

				const std::function<void(int, int)> &setProgressRangeCb = [&](int min, int max)->void{
					QMetaObject::invokeMethod(&progressBar, "setRange", Qt::QueuedConnection,
						Q_ARG(int, min),
						Q_ARG(int, max));
				};

				const std::function<void(int)> &setProgressValueCb = [&](int value)->void{
					QMetaObject::invokeMethod(&progressBar, "setValue", Qt::QueuedConnection,
						Q_ARG(int, value));
				};

				object->setProgressRangeCallback(setProgressRangeCb);
				object->setProgressValueCallback(setProgressValueCb);
				QEventLoop eventLoop;
				QFutureWatcher<Ret> futureWatcher;
				QObject::connect(&futureWatcher, &QFutureWatcher<Ret>::finished, &eventLoop, &QEventLoop::quit);
				QFuture<Ret> future = QtConcurrent::run(object, fn, arg1, arg2, arg3, arg4, arg5);
				futureWatcher.setFuture(future);
				eventLoop.exec();
			}
		private:
			WatcherProgressBarV2(){}
		};
	}
}

#endif // WATCHERPROGRESSBAR_H
