#ifndef PROGRESSBAR_P_H
#define PROGRESSBAR_P_H

#include "progressbar.h"
#include <QPen>

namespace gdp
{
	namespace gui
	{
		class ProgressBarPrivate
		{
			Q_DECLARE_PUBLIC(ProgressBar);
		public:
			ProgressBarPrivate(ProgressBar *pProgressBar);
			~ProgressBarPrivate();

			void init();
			void setSpanAngle(float spanAngle);
		private:
			ProgressBar *q_ptr{ nullptr };
			float m_perSpanAngle;
			float m_startAngle;
			float m_endAngle;
			float m_percentageAngle{ 0.0 };
			int m_minValue;
			int m_maxValue;
			int m_value;
			QString m_percentageText;
			QFont m_percentageTextFont;
			QPen m_clockDialPen;
			QPen m_percentagePen;
			QRect m_rect;
			QPixmap m_clockDialPixmap;
		};
	}
}

#endif // PROGRESSBAR_P_H
