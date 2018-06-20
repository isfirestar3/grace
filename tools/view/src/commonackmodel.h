#ifndef COMMONACKMODEL_H
#define COMMONACKMODEL_H

#include <QtCore/QAbstractTableModel>

class CommonAckModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	CommonAckModel(QObject *parent = nullptr);
	~CommonAckModel();

	void SetCommonAckList(QList<QStringList> &commonAckList);
	void SetHorizontalHeader(QStringList &hHeader);
	void Clear();
	int	rowCount(const QModelIndex &parent = QModelIndex()) const;
	int	columnCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;
private:
	QList<QStringList> m_commonAckList;
	QStringList m_hHeader;
};

#endif // COMMONACKMODEL_H
