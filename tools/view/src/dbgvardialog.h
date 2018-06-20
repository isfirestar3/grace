#ifndef DBGVARDIALOG_H
#define DBGVARDIALOG_H

#include "ui_dbgvardialog.h"
#include "common/headerview.h"
#include "dbgvarmodel.h"
#include <QtWidgets/QDialog>
#include "mntypes.h"

class DbgVarDialog : public QDialog
{
	Q_OBJECT

public:
	enum DVD_Field
	{
		DVD_ID_Type
	};
public:
	DbgVarDialog(QWidget *parent = nullptr);
	~DbgVarDialog();

	void UpdateNetStatus(int &netStatus)
	{
		m_netStatus = netStatus;
	}

	QMultiMap<QPair<int, uint32_t>, QString> GetChoosedMap() const
	{
		if (m_dbgVarModel.isNull())
		{
			return QMultiMap<QPair<int, uint32_t>, QString>();
		}

		return m_dbgVarModel->GetChoosedMap();
	}

	void InitUi();
	void InitSlots();
	void PostDdgVarFinished(bool successed);
	void ChoosedFinished();
	void SetChoosedMap(const QMultiMap<QPair<int, uint32_t>, QString> &choosedMap);

protected:
	void showEvent(QShowEvent *);
private:
	Ui::DbgVarDialog ui;
	int m_robotId{ -1 };
	int m_netStatus{ mn::kNetworkStatus_Closed };
	std::vector<mn::var_item> m_items;
	QSharedPointer<DbgVarModel> m_dbgVarModel;
	QSharedPointer<HeaderView> m_headerView;
};

#endif // DBGVARDIALOG_H
