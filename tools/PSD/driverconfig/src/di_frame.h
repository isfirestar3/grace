#ifndef DI_FRAME_H
#define DI_FRAME_H

#include "ui_di_frame.h"
#include "task_type_define.h"
#include <QWidget>
#include <cstdint>

class di_frame : public QWidget
{
	Q_OBJECT
public:
	enum DIConfigType
	{
		CT_LowEnable,
		CT_LowEnable_with_Reset,
		CT_HighEnable,
		CT_HighEnable_with_Reset,
		CT_Not_Configured,
		CT_Pos_Limit_Switch,
		CT_Neg_Limit_Switch,
		CT_Halt_Low,
		CT_Halt_High
	};
public:
	di_frame(QWidget *parent = 0);
	~di_frame();

	void initUi();
	void initCmb();
	void initCmb(int startIndex, int endIndex, QComboBox *pCmb);
	void init_rw_labels();
	void read_di_config(task_type_t task_type, uint16_t sub_index, funcalltask_t callback = nullptr);
	void write_di_config(task_type_t task_type, uint16_t sub_index, uint8_t di_config_, funcalltask_t callback = nullptr);
	void write_di_filter_time(task_type_t task_type, uint16_t sub_index, uint16_t di_filter_time_, funcalltask_t callback = nullptr);
	void write_di_state(task_type_t task_type, uint16_t sub_index, uint16_t di_state_, funcalltask_t callback = nullptr);	
	void write_save(task_type_t task_type, uint16_t sub_index, uint32_t save_, funcalltask_t callback = nullptr);
	void on_refresh();			//Ë¢ÐÂ½çÃæ
	QString DIConfigTypeToString(DIConfigType ct);
public slots:
	void on_di0_saveBtn_clicked();
	void on_di1_saveBtn_clicked();
	void on_di2_saveBtn_clicked();
	void on_di3_saveBtn_clicked();
	void on_di4_saveBtn_clicked();
	void on_di5_saveBtn_clicked();
	void on_di_filter_time0_saveBtn_clicked();
	void on_di_filter_time1_saveBtn_clicked();
	void on_di_filter_time2_saveBtn_clicked();
	void on_di_filter_time3_saveBtn_clicked();
	void on_di_filter_time4_saveBtn_clicked();
	void on_di_filter_time5_saveBtn_clicked();
	void on_refreshBtn_clicked();
protected:
	void showEvent(QShowEvent *);
private:
	Ui::di_frame ui;
};

#endif // DI_FRAME_H
