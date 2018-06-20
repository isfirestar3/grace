#ifndef MOTOR_FRAME_H
#define MOTOR_FRAME_H

#include "ui_motor_frame.h"
#include "task_type_define.h"
#include <QWidget>
#include <cstdint>

class motor_frame : public QWidget
{
	Q_OBJECT

public:
	motor_frame(QWidget *parent = 0);
	~motor_frame();

	void init_rw_labels();
	void read_motor_specified(task_type_t task_type, uint16_t sub_index, funcalltask_t callback = nullptr);
	void write_motor_r(task_type_t task_type, uint16_t sub_index, float motor_r_, funcalltask_t callback = nullptr);
	void write_motor_l(task_type_t task_type, uint16_t sub_index, float motor_l_, funcalltask_t callback = nullptr);
	void write_polepairs(task_type_t task_type, uint16_t sub_index, uint8_t polepairs_, funcalltask_t callback = nullptr);
	void write_encoderlines(task_type_t task_type, uint16_t sub_index, uint16_t encoderlines_, funcalltask_t callback = nullptr);
	void write_break_t(task_type_t task_type, uint16_t sub_index, bool break_t_, funcalltask_t callback = nullptr);
	void write_con_current(task_type_t task_type, uint16_t sub_index, uint8_t con_current_, funcalltask_t callback = nullptr);
	void write_max_current(task_type_t task_type, uint16_t sub_index, uint8_t max_current_, funcalltask_t callback = nullptr);	
	void on_refresh();			//Ë¢ÐÂ½çÃæ
public slots:
	void on_motor_r_saveBtn_clicked();
	void on_motor_l_saveBtn_clicked();
	void on_polepairs_saveBtn_clicked();
	void on_encoderline_saveBtn_clicked();
	void on_break_saveBtn_clicked();
	void on_con_current_saveBtn_clicked();
	void on_max_current_saveBtn_clicked();
	void on_refreshBtn_clicked();
protected:
	void showEvent(QShowEvent *);
private:
	Ui::motor_frame ui;
};

#endif // MOTOR_FRAME_H
