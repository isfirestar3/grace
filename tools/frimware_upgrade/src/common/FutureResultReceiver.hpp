#ifndef FUTURERESULTRECEIVER_H
#define FUTURERESULTRECEIVER_H
#include <QtCore/QFutureWatcher>
#include <QtConcurrent/QtConcurrent>
#include <functional>

namespace gdp
{
	namespace gui
	{
		template<typename Ret>
		class FutureResultReceiver : public QObject
		{
		public:
			FutureResultReceiver();
			~FutureResultReceiver();

			QFutureWatcher<Ret> &getFutureWatcher()
			{
				return m_futureWatcher;
			}

			void setResultCallback(const std::function<void(Ret)> &retCallback)
			{
				m_retCallback = retCallback;
			}

			void setExceptionCallback(const std::function<void()> &exceptionCallback)
			{
				m_exceptionCallback = exceptionCallback;
			}

			void resultAt(int index);
		private:
			QFutureWatcher<Ret> m_futureWatcher;
			std::function<void(Ret)> m_retCallback;
			std::function<void()> m_exceptionCallback;
		};

		template<typename Ret>
		FutureResultReceiver<Ret>::FutureResultReceiver()
		{

		}

		template<typename Ret>
		FutureResultReceiver<Ret>::~FutureResultReceiver()
		{

		}

		template<typename Ret>
		void FutureResultReceiver<Ret>::resultAt(int index)
		{
			if (m_retCallback)
			{
				try
				{
					Ret ret = m_futureWatcher.resultAt(index);
					m_retCallback(ret);
				}
				catch (...)
				{
					if (m_exceptionCallback)
					{
						m_exceptionCallback();
					}
				}
			}
		}
	}
}

#endif // !FUTURERESULTRECEIVER_H
