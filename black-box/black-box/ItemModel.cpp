#include "ItemModel.h"
#include <QtCore>

ItemModel::ItemModel(QObject *parent)
	:QStandardItemModel(parent)
{
	initialize();
}


ItemModel::~ItemModel()
{
}

void ItemModel::setData(const QList<QPair<bool, QString> > &datas) {
	initialize();
	QListIterator<QPair<bool, QString> > i(datas);
	while (i.hasNext()) {

		QList<QStandardItem* > items;
		QPair<bool, QString> const& data = i.next();
		QStandardItem* item = new QStandardItem;
		item->setCheckable(true);
		item->setEditable(false);
		item->setCheckState(data.first ? Qt::Checked : Qt::Unchecked);
		items << item;
		item = new QStandardItem;
		item->setData(data.second, Qt::DisplayRole);
		items << item;
		appendRow(items);
	}

}

void ItemModel::initialize() {
	//clear();
	//setColumnCount(2);
	//setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit(""));
	//setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("日志类型"));
	clear();
	std::string name = QString::fromLocal8Bit("log types").toStdString();
	setHorizontalHeaderLabels(QStringList() << tr("") << tr(name.c_str()));
}