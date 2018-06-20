#ifndef STANDARDITEMMODEL_H
#define STANDARDITEMMODEL_H

#include <QtGui/QStandardItemModel>

class StandardItemModel : public QStandardItemModel
{
	Q_OBJECT

public:
	StandardItemModel(QObject *parent = nullptr);
	~StandardItemModel();
	
	void CheckStateChanged(QStandardItem *pItem);
	void ChangeChildCheckState(QStandardItem *pItem);
	void ChangeParentCheckState(QStandardItem *pItem);
	void ChangeHeaderViewCheckState(QStandardItem *pItem);
	void ChangeTopCheckState(int column, Qt::CheckState checkState);
signals:
	void UpdateHeaderViewCheckState(int, Qt::CheckState);
};

#endif // STANDARDITEMMODEL_H
