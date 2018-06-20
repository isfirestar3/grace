#pragma once

#include <QtWidgets\QItemDelegate>
#include <QtGui\qevent.h>
#include <QtCore\qobject.h>

class custom_control_delegate : public QItemDelegate{
	Q_OBJECT
public:
	explicit custom_control_delegate(QObject *parent = 0);
	void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const;
	bool editorEvent(QEvent *event, QAbstractItemModel *model, const QStyleOptionViewItem &option, const QModelIndex &index);

signals:
	void btn_signal_clicked(int index,QString str);

private:
	QMap<QModelIndex, QStyleOptionButton*> m_btns;
	QObject *parent_;

};