#include "custom_control_delegate.h"
#include <QtWidgets\qapplication.h>
#include <QtGui\qpainter.h>
#include "proto_udp_typedef.h"

custom_control_delegate::custom_control_delegate(QObject *parent) :
QItemDelegate(parent), parent_(parent)
{
}

void custom_control_delegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
	if (index.column() == 4) {
		//设置进度条代理
		int value = index.model()->data(index).toInt();
		QStyleOptionProgressBarV2 progressBarOption;
		progressBarOption.rect = option.rect.adjusted(4, 4, -4, -4);
		progressBarOption.minimum = 0;
		progressBarOption.maximum = 100;
		progressBarOption.textAlignment = Qt::AlignRight;
		progressBarOption.textVisible = true;
		progressBarOption.progress = value;
		progressBarOption.text = tr("%1%").arg(progressBarOption.progress);

		painter->save();
		if (option.state & QStyle::State_Selected) {
			painter->fillRect(option.rect, option.palette.highlight());
			painter->setBrush(option.palette.highlightedText());
		}
		QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressBarOption, painter);

		painter->restore();

	}
	else if (index.column() == 6){
		//设置按钮代理
		if (index.model()->data(index).isNull())return;
		QStyleOptionButton* button = m_btns.value(index);
		if (!button) {
			button = new QStyleOptionButton();
			button->rect = option.rect.adjusted(2, 2, -2, -2);
			button->text = index.model()->data(index).toString();
			button->state |= QStyle::State_Enabled;
			(const_cast<custom_control_delegate *>(this))->m_btns.insert(index, button);
		}
		painter->save();

		if (option.state & QStyle::State_Selected) {
			painter->fillRect(option.rect, option.palette.highlight());

		}
		painter->restore();
		QApplication::style()->drawControl(QStyle::CE_PushButton, button, painter);
	}
	else {
		//否则调用默认委托
		return QItemDelegate::paint(painter, option, index);
	}
}

bool custom_control_delegate::editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index)
{
	if (event->type() == QEvent::MouseButtonPress) {
		QMouseEvent* e = (QMouseEvent*)event;
		if (option.rect.adjusted(2, 2, -2, -2).contains(e->x(), e->y()) && m_btns.contains(index)) {
			m_btns.value(index)->state |= QStyle::State_Sunken;
		}
	}
	if (event->type() == QEvent::MouseButtonRelease) {
		QMouseEvent* e = (QMouseEvent*)event;

		if (option.rect.adjusted(2, 2, -2, -2).contains(e->x(), e->y()) && m_btns.contains(index)) {
			m_btns.value(index)->state &= (~QStyle::State_Sunken);
			QVector<int> operate;
			QString btn_str = model->data(index).toString();
			if (btn_str == QStringLiteral("重新获取"))
			{
				operate.push_back(TABLEVIEW_BTN::Btn_ReGet);
			}
			else if (btn_str == QStringLiteral("重试"))
			{
				operate.push_back(TABLEVIEW_BTN::Btn_Retry);
			}
			else if (btn_str == QStringLiteral("降级"))
			{
				operate.push_back(TABLEVIEW_BTN::Btn_Degrade);
			}
			emit model->dataChanged(index, index, operate);
		}
	}
	return true;
}