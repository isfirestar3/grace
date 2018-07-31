#include "progressbar.h"
#include "progressbar_p.h"
#include "gdp_def.h"
#include <QPainter>

namespace gdp
{
	namespace gui
	{
		namespace
		{
			static const int SCALE_COUNT = 100;
			static const int SCALE_LEN = 20;
		}

		ProgressBar::ProgressBar(QWidget *parent)
			: QWidget(parent),
			d_ptr{ QSharedPointer<ProgressBarPrivate>::create(this) }
		{
			Q_D(ProgressBar);

			if (nullptr == d)
			{
				return;
			}

			d->init();
		}

		ProgressBar::~ProgressBar()
		{

		}

		void ProgressBar::setMin(int minValue)
		{
			Q_D(ProgressBar);

			if (nullptr == d)
			{
				return;
			}

			d->m_minValue = minValue;
		}

		void ProgressBar::setMax(int maxValue)
		{
			Q_D(ProgressBar);

			if (nullptr == d)
			{
				return;
			}

			d->m_maxValue = maxValue;
		}

		Q_INVOKABLE void ProgressBar::setRange(int minValue, int maxValue)
		{
			Q_D(ProgressBar);

			if (nullptr == d)
			{
				return;
			}

			d->m_minValue = minValue;
			d->m_maxValue = maxValue;
		}

		Q_INVOKABLE void ProgressBar::setValue(int value)
		{
			Q_D(ProgressBar);

			if (nullptr == d)
			{
				return;
			}

			if (value < d->m_minValue || value > d->m_maxValue)
			{
				return;
			}

			float percentage = ((float)(value - d->m_minValue) / (d->m_maxValue - d->m_minValue)) * 100;
			setPercentage(percentage);
		}

		void ProgressBar::setSpanAngle(float spanAngle)
		{
			Q_D(ProgressBar);

			if (nullptr == d)
			{
				return;
			}

			d->setSpanAngle(spanAngle);
		}

		void ProgressBar::resizeEvent(QResizeEvent *)
		{
			Q_D(ProgressBar);

			if (nullptr == d)
			{
				return;
			}

			setFixedSize(200, 200);
			int sz = qMin(width(), height());
			QRegion region(0, 0, sz, sz, QRegion::Ellipse);
			setMask(region);
			const QRect &rect = region.boundingRect();
			d->m_rect.setRect(rect.x(), rect.y(), rect.width(), rect.height());
			d->m_clockDialPixmap = d->m_clockDialPixmap.scaled(rect.width(), rect.height());
		}

		void ProgressBar::paintEvent(QPaintEvent *)
		{
			Q_D(ProgressBar);

			if (nullptr == d)
			{
				return;
			}

			d->m_clockDialPixmap.fill(QColor(255, 255, 255, 200));
			QPainter clockDialPainter(&d->m_clockDialPixmap);
			clockDialPainter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
			clockDialPainter.setPen(d->m_clockDialPen);
			paintClockDial(clockDialPainter);
			QPainter painter(this);
			painter.setRenderHints(QPainter::Antialiasing | QPainter::TextAntialiasing);
			painter.drawPixmap(d->m_rect, d->m_clockDialPixmap);
		}

		void ProgressBar::paintClockDial(QPainter &painter)
		{
			Q_D(ProgressBar);

			if (nullptr == d)
			{
				return;
			}

			paintCircleScale(painter, d->m_startAngle, d->m_endAngle, d->m_perSpanAngle);
			paintCirclePercentage(painter);
		}

		void ProgressBar::paintCircleScale(QPainter &painter, float startAngle, float endAngle, float perSpanAngle)
		{
			Q_D(ProgressBar);

			if (nullptr == d)
			{
				return;
			}

			for (float angle_ = startAngle; angle_ >= endAngle; angle_ -= perSpanAngle)
			{
				QRect rect = d->m_rect;
				painter.save();
				painter.translate(rect.center());
				painter.scale(gdp::core::SX, gdp::core::SY);
				painter.rotate(angle_);
				painter.drawLine(QLine(rect.width() / 2, 0, rect.width() / 2 - SCALE_LEN, 0));
				painter.restore();
			}
		}

		void ProgressBar::paintCirclePercentage(QPainter &painter)
		{
			Q_D(ProgressBar);

			if (nullptr == d)
			{
				return;
			}

			if (d->m_percentageText.isEmpty())
			{
				return;
			}

			QRect rect = d->m_rect;

			for (float angle_ = d->m_startAngle; angle_ >= d->m_percentageAngle; angle_ -= d->m_perSpanAngle)
			{				
				painter.save();
				painter.setPen(d->m_percentagePen);
				painter.translate(rect.center());
				painter.scale(gdp::core::SX, gdp::core::SY);
				painter.rotate(angle_);
				painter.drawLine(QLine(rect.width() / 2, 0, rect.width() / 2 - SCALE_LEN, 0));
				painter.restore();
			}

			painter.save();
			painter.setPen(d->m_percentagePen);
			painter.setFont(d->m_percentageTextFont);
			painter.drawText(rect, Qt::AlignCenter, d->m_percentageText);
			painter.restore();
		}

		void ProgressBar::setPercentage(quint8 percentage)
		{
			Q_D(ProgressBar);

			if (nullptr == d)
			{
				return;
			}

			d->m_percentageAngle = d->m_startAngle - d->m_perSpanAngle * percentage;
			d->m_percentageText = QString::number(percentage) + gdp::core::PERCENTAGE_SYMBOL;
			update();
		}
	}
}
