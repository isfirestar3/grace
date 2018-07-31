#include "progressbar_p.h"
#include "gdp_def.h"

namespace gdp
{
	namespace gui
	{
		namespace
		{
			static const int SCALE_COUNT = 100;
			static const quint8 PERCENTAGE_TEXT_FONT_SIZE = 65;
		}

		ProgressBarPrivate::ProgressBarPrivate(ProgressBar *pProgressBar)
			:q_ptr{ pProgressBar }
		{

		}

		ProgressBarPrivate::~ProgressBarPrivate()
		{

		}

		void ProgressBarPrivate::init()
		{
			Q_Q(ProgressBar);

			if (nullptr == q)
			{
				return;
			}

			m_percentageTextFont = q->font();
			m_percentageTextFont.setPixelSize(PERCENTAGE_TEXT_FONT_SIZE);
			m_percentageTextFont.setBold(true);
			m_minValue = 0;
			m_maxValue = 100;
			m_value = 0;
			m_clockDialPen = QPen(QColor(136, 204, 225));
			m_percentagePen = QPen(QColor(187, 68, 106), 2);
			m_clockDialPixmap = QPixmap(q->width(), q->height());
			setSpanAngle(270);
			q->setAttribute(Qt::WA_StaticContents);
		}

		void ProgressBarPrivate::setSpanAngle(float spanAngle)
		{
			if (spanAngle > gdp::core::FULL_CIRCLE)
			{
				return;
			}

			m_perSpanAngle = spanAngle / (float)SCALE_COUNT;
			float angle = (spanAngle - gdp::core::HALF_CIRCLE) / 2.0;
			m_startAngle = gdp::core::HALF_CIRCLE + angle;
			m_endAngle = -angle;			
		}
	}
}
