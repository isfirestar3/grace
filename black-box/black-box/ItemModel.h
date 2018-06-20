#pragma once
#include "qstandarditemmodel.h"
class ItemModel :public QStandardItemModel
{
	Q_OBJECT
public:
	ItemModel(QObject* parent = 0);
	~ItemModel();
	void setData(const QList<QPair<bool, QString>> & datas);

private:
	void initialize();
};

