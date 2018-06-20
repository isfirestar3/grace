#pragma once

#include <Qtwidgets/QItemDelegate>
class empty_delegate : public QItemDelegate
{
public:
	explicit empty_delegate(QObject *parent = 0) : QItemDelegate(parent) {}
	//explicit empty_delegate(int row_from, int row_to, QObject *parent = 0) : QItemDelegate(parent) { row_start_ = row_from; row_end_ = row_to; }
	QWidget *createEditor(QWidget *pWidget, const QStyleOptionViewItem &option, const QModelIndex &index) const
	{
		//if (index.data() >= row_start_ && index.data() <= row_end_)
			return NULL;
	}

//private:
//	int row_start_;
//	int row_end_;
};