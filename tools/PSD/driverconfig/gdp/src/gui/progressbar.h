#ifndef PROGRESSBAR_H
#define PROGRESSBAR_H

#include "gdp_global.h"
#include <QWidget>
#include <QSharedPointer>

namespace gdp
{
	namespace gui
	{
		class ProgressBarPrivate;

		class GDP_EXPORT ProgressBar : public QWidget
		{
			Q_OBJECT
			Q_DECLARE_PRIVATE(ProgressBar);
			Q_DISABLE_COPY(ProgressBar);
		public:
			ProgressBar(QWidget *parent = nullptr);
			~ProgressBar();

			void setMin(int minValue);
			void setMax(int maxValue);
			Q_INVOKABLE void setRange(int minValue, int maxValue);
			Q_INVOKABLE void setValue(int value);
			void setSpanAngle(float spanAngle);
		protected:
			void resizeEvent(QResizeEvent *);
			void paintEvent(QPaintEvent *);
		private:
			void paintClockDial(QPainter &painter);
			void paintCircleScale(QPainter &painter, float startAngle, float endAngle, float perSpanAngle);
			void paintCirclePercentage(QPainter &painter);
			void setPercentage(quint8 percentage);

			QSharedPointer<ProgressBarPrivate> d_ptr;
		};
	}
}

#endif // PROGRESSBAR_H
