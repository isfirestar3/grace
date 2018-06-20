#ifndef CONTROLDIALOG_H
#define CONTROLDIALOG_H

#include "ui_controldialog.h"
#include "task_type_define.h"
#include <QDialog>
#include <QSharedPointer>
#include <cstdint>

class ControlDialogPrivate;

class ControlDialog : public QDialog
{
	Q_OBJECT
	Q_DECLARE_PRIVATE(ControlDialog);
	Q_DISABLE_COPY(ControlDialog);
public:
	enum class ModeType
	{
		Mode_Diagnosis,
		Mode_Current,
		Mode_Velocity,
		Mode_Position
	};
public:
	ControlDialog(QWidget *parent = 0);
	~ControlDialog();

	void init();
	void initUi();
	void initCmb();
	void init_rw_labels();
	void read_speed_pid(task_type_t task_type, uint16_t sub_index, funcalltask_t callback = nullptr);
	void write_switch(task_type_t task_type, uint16_t sub_index, uint8_t switch_, funcalltask_t callback = nullptr);
	void write_clr_fault(task_type_t task_type, uint16_t sub_index, int32_t clr_fault_, funcalltask_t callback = nullptr);
	void write_mode(task_type_t task_type, uint16_t sub_index, uint8_t mode_, funcalltask_t callback = nullptr);
	void write_diagnosis_current(task_type_t task_type, uint16_t sub_index, uint32_t diagnosis_current_, funcalltask_t callback = nullptr);
	void write_reset_caliburation(task_type_t task_type, uint16_t sub_index, uint8_t reset_caliburation_, funcalltask_t callback = nullptr);
	void on_refresh();
	QString ModeTypeToString(ModeType modeType);
protected:
	void showEvent(QShowEvent *);
	void closeEvent(QCloseEvent *);
private slots:
	void on_switch_save_btn_clicked();
	void on_clr_fault_save_btn_clicked();
	void on_mode_save_btn_clicked();
	void on_diagnosis_current_save_btn_clicked();
	void on_reset_caliburation_save_btn_clicked();
	void on_refresh_btn_clicked();
private:
	Ui::ControlDialog ui;
	QSharedPointer<ControlDialogPrivate> d_ptr;
};

#endif // CONTROLDIALOG_H
