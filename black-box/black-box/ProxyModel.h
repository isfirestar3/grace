#pragma once
#include <QSortFilterProxyModel>
class ProxyModel :public QSortFilterProxyModel
{
	Q_OBJECT
public:
	ProxyModel(QObject* parent = 0);
	~ProxyModel();

	void setText(const QString & text);
protected:
	bool filterAcceptsRow(int sourceRow,
		const QModelIndex &sourceParent) const;

private:
	QString m_str;
};

