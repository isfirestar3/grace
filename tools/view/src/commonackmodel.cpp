#include "commonackmodel.h"
#include "common_impls.hpp"

static const int COLUMN_COUNT = 2;

CommonAckModel::CommonAckModel(QObject *parent)
	: QAbstractTableModel(parent)
{

}

CommonAckModel::~CommonAckModel()
{

}

void CommonAckModel::SetCommonAckList(QList<QStringList> &commonAckList)
{
	beginResetModel();
	qListFree<QStringList>(m_commonAckList);
	m_commonAckList.swap(commonAckList);
	endResetModel();
}

void CommonAckModel::SetHorizontalHeader(QStringList &hHeader)
{
	qListFree<QString>(m_hHeader);
	m_hHeader.swap(hHeader);
}

void CommonAckModel::Clear()
{
	SetCommonAckList(QList<QStringList>());
}

int CommonAckModel::rowCount(const QModelIndex &/*parent*/ /*= QModelIndex()*/) const
{
	return m_commonAckList.size();
}

int CommonAckModel::columnCount(const QModelIndex &/*parent*/ /*= QModelIndex()*/) const
{
	return COLUMN_COUNT;
}

QVariant CommonAckModel::data(const QModelIndex & index, int role /*= Qt::DisplayRole*/) const
{
	if (!index.isValid() || Qt::DisplayRole != role)
	{
		return QVariant();
	}

	int row = index.row();

	if (row >= m_commonAckList.size())
	{
		return QVariant();
	}

	int column = index.column();

	if (column >= COLUMN_COUNT)
	{
		return QVariant();
	}

	QStringList columnData = m_commonAckList[row];
	Q_ASSERT(COLUMN_COUNT == columnData.size());

	if (COLUMN_COUNT != columnData.size())
	{
		return QVariant();
	}

	return columnData[column];
}

QVariant CommonAckModel::headerData(int section, Qt::Orientation orientation, int role /*= Qt::DisplayRole*/) const
{
	if (section >= COLUMN_COUNT || Qt::Vertical == orientation || Qt::DisplayRole != role)
	{
		return QVariant();
	}

	Q_ASSERT(COLUMN_COUNT == m_hHeader.size());

	if (COLUMN_COUNT != m_hHeader.size())
	{
		return QVariant();
	}

	return m_hHeader[section];
}