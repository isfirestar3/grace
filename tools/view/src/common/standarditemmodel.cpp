#include "standarditemmodel.h"

StandardItemModel::StandardItemModel(QObject *parent)
	: QStandardItemModel(parent)
{
	connect(this, &QStandardItemModel::itemChanged, this, &StandardItemModel::CheckStateChanged);
}

StandardItemModel::~StandardItemModel()
{

}

void StandardItemModel::CheckStateChanged(QStandardItem *pItem)
{
	if (nullptr == pItem)
	{
		return;
	}

	if (!pItem->isCheckable())
	{
		return;
	}

	disconnect(this, &QStandardItemModel::itemChanged, this, &StandardItemModel::CheckStateChanged);
	ChangeChildCheckState(pItem);
	ChangeParentCheckState(pItem);
	ChangeHeaderViewCheckState(pItem);
	connect(this, &QStandardItemModel::itemChanged, this, &StandardItemModel::CheckStateChanged);
}

void StandardItemModel::ChangeChildCheckState(QStandardItem *pItem)
{
	if (nullptr == pItem)
	{
		return;
	}

	QStandardItem *pChildItem = nullptr;

	for (int i = 0; i < pItem->rowCount(); ++i)
	{
		pChildItem = pItem->child(i);

		if (nullptr == pChildItem)
		{
			continue;
		}

		Qt::CheckState checkState = pItem->checkState();
		pChildItem->setCheckState(checkState);
		ChangeChildCheckState(pChildItem);
	}
}

void StandardItemModel::ChangeParentCheckState(QStandardItem *pItem)
{
	if (nullptr == pItem)
	{
		return;
	}

	int uncheckedCount = 0, partiallyCheckedCount = 0, checkedCount = 0;
	QStandardItem *pParentItem = pItem->parent();

	if (nullptr == pParentItem)
	{
		return;
	}

	int childCount = pParentItem->rowCount();
	QStandardItem *pChildItem = nullptr;

	for (int i = 0; i < childCount; ++i)
	{
		pChildItem = pParentItem->child(i);

		if (nullptr == pChildItem)
		{
			continue;
		}

		switch (pChildItem->checkState())
		{
			case Qt::Unchecked:
			{
				++uncheckedCount;
			}
				break;
			case Qt::PartiallyChecked:
			{
				++partiallyCheckedCount;
			}
				break;
			case Qt::Checked:
			{
				++checkedCount;
			}
				break;
			default:
			{
				Q_ASSERT(false);
			}
				break;
		}
	}

	Qt::CheckState checkState;

	Q_ASSERT((uncheckedCount + partiallyCheckedCount + checkedCount) == childCount);

	if ((uncheckedCount + partiallyCheckedCount + checkedCount) != childCount)
	{
		return;
	}

	if (uncheckedCount == childCount)
	{
		checkState = Qt::Unchecked;
	}
	else if (checkedCount == childCount)
	{
		checkState = Qt::Checked;
	}
	else
	{
		checkState = Qt::PartiallyChecked;
	}

	pParentItem->setCheckState(checkState);
	ChangeParentCheckState(pParentItem);
}

void StandardItemModel::ChangeHeaderViewCheckState(QStandardItem *pItem)
{
	if (nullptr == pItem)
	{
		return;
	}

	QStandardItem *pParent = nullptr;

	do 
	{
		pParent = pItem->parent();

		if (nullptr == pParent)
		{
			break;
		}

		pItem = pParent;
	} while (true);

	if (nullptr == pItem)
	{
		return;
	}

	QStandardItem *p_item = nullptr;
	Qt::CheckState checkState = Qt::Unchecked;
	int unChecked = 0, partiallyChecked = 0, checked = 0;
	int column = pItem->column();
	int rc = rowCount();

	for (int i = 0; i < rc; ++i)
	{
		p_item = item(i, column);

		if (nullptr == p_item)
		{
			continue;
		}

		checkState = p_item->checkState();

		switch (checkState)
		{
		case Qt::Unchecked:
		{
			++unChecked;
		}
			break;
		case Qt::PartiallyChecked:
		{
			++partiallyChecked;
		}
			break;
		case Qt::Checked:
		{
			++checked;
		}
			break;
		default:
		{
			Q_ASSERT(false);
		}
			break;
		}
	}

	if (unChecked == rc)
	{
		emit UpdateHeaderViewCheckState(column, Qt::Unchecked);
	}
	else if (checked == rc)
	{
		emit UpdateHeaderViewCheckState(column, Qt::Checked);
	}
	else
	{
		emit UpdateHeaderViewCheckState(column, Qt::PartiallyChecked);
	}
}

void StandardItemModel::ChangeTopCheckState(int column, Qt::CheckState checkState)
{
	QStandardItem *pItem = nullptr;
	int rc = rowCount();

	for (int i = 0; i < rc; ++i)
	{
		pItem = item(i, column);

		if (nullptr == pItem)
		{
			continue;
		}

		pItem->setCheckState(checkState);
	}
}
