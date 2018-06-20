#pragma once

#include <QItemDelegate>
#include <qlineedit.h>
#include <QSpinBox>

class item_delegate :
	public QItemDelegate
{
public:
	item_delegate(QObject * parent = 0);

	QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const;

	void setEditorData(QWidget *editor, const QModelIndex &index) const;

	void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const;

	void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const;
};

