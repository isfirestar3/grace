#include "push_button.h"

namespace
{
	static const int PB_WIDTH = 24;
	static const int PB_HEIGHT = 24;
}

PushButton::PushButton(QWidget *parent)
	:QPushButton(parent)
{
	m_state = PB_State::PB_Invalid;
	setFixedSize(QSize(PB_WIDTH, PB_HEIGHT));
}

void PushButton::SetPixmap(const QString &pixmap)
{
	m_pixmapEnabled = pixmap;
	int index = m_pixmapEnabled.lastIndexOf(".");
	int rightLen = m_pixmapEnabled.length() - index - 1;
	m_pixmapHover = m_pixmapEnabled.right(rightLen) + "_hover";
	m_pixmapDisabled = m_pixmapEnabled.right(rightLen) + "_disabled";
	m_state = PB_State::PB_Enabled;
}

void PushButton::changeEvent(QEvent *e)
{
	if (nullptr == e)
	{
		return;
	}

	QEvent::Type type = e->type();

	if (QEvent::EnabledChange == type)
	{
		if (isEnabled())
		{
			m_state = PB_State::PB_Enabled;
		}
		else
		{
			m_state = PB_State::PB_Disabled;
		}
	}

	if (QEvent::EnabledChange == type || QEvent::Enter == type)
	{
		update();
	}

	QPushButton::changeEvent(e);
}

void PushButton::enterEvent(QEvent *e)
{
	if (nullptr == e)
	{
		return;
	}

	if (isEnabled())
	{
		m_state = PB_State::PB_Hover;
	}
	else
	{
		m_state = PB_State::PB_Disabled;
	}

	update();
	QPushButton::enterEvent(e);
}

void PushButton::leaveEvent(QEvent *e)
{
	if (nullptr == e)
	{
		return;
	}

	if (isEnabled())
	{
		m_state = PB_State::PB_Enabled;
	}
	else
	{
		m_state = PB_State::PB_Disabled;
	}

	update();
	QPushButton::leaveEvent(e);
}

void PushButton::paintEvent(QPaintEvent *)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);

	switch (m_state)
	{
	case PushButton::PB_State::PB_Invalid:
		break;
	case PushButton::PB_State::PB_Enabled:
	{
		painter.drawPixmap(rect(), m_pixmapEnabled);
	}
		break;
	case PushButton::PB_State::PB_Hover:
	{
		painter.drawPixmap(rect(), m_pixmapHover);
	}
		break;
	case PushButton::PB_State::PB_Disabled:
	{
		painter.drawPixmap(rect(), m_pixmapDisabled);
	}
		break;
	default:
	{
		Q_ASSERT(false);
	}
		break;
	}
}
