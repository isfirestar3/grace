#include "ProxyModel.h"


ProxyModel::ProxyModel(QObject *parent)
	:QSortFilterProxyModel(parent) 
{
}


ProxyModel::~ProxyModel()
{
}

void ProxyModel::setText(const QString &text) {

	if (m_str != text) {

		m_str = text;
		invalidateFilter();
	}
}

bool ProxyModel::filterAcceptsRow(int sourceRow,
	const QModelIndex &sourceParent) const {
	if (!m_str.isEmpty()) {

		QModelIndex index = sourceModel()->index(sourceRow, 1,
			sourceParent);

		if (!sourceModel()->data(index).toString().contains(m_str))
			return false;
	}
	return true;
}
