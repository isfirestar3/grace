#ifndef ONEKEYADJUSTDIALOG_H
#define ONEKEYADJUSTDIALOG_H

#include <QtWidgets/QDialog>
#include <vector>

namespace Ui {
class OneKeyAdjustDialog;
}

class OneKeyAdjustDialog : public QDialog
{
public:
	enum class AdjustType
	{
		AdjustTwo,
		AdjustOne_1,
		AdjustOne_2,
		AdjustOne_3,
		AdjustOne_4
	};
public:
    explicit OneKeyAdjustDialog(QWidget *parent = 0);
    ~OneKeyAdjustDialog();

	const std::vector<int> &GetWopIdVector() const
	{
		return m_vecWopId;
	}

	void RetranslateUi();
	void InitUiData();
	void InitUiControl();
	void InitSlots();
	void AdjustAllEdgeToTwo();
	void AdjustAllEdgeToOne();
public slots:
	void SlotTwoWayRadioBtnClicked();
	void SlotOneWayRadioBtnClicked();
	void SlotUpdateEdgeProperty();
private:
	Ui::OneKeyAdjustDialog *ui;
	bool m_en{ true };
	std::vector<int> m_vecWopId;
};

#endif // ONEKEYADJUSTDIALOG_H
