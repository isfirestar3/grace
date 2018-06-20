#ifndef SPEED_FRAME_H
#define SPEED_FRAME_H

#include "ui_speed_frame.h"
#include "task_type_define.h"
#include <QWidget>
#include <cstdint>

class speed_data;
class QDoubleSpinBox;

class speed_frame : public QWidget
{
	Q_OBJECT

public:
	speed_frame(QWidget *parent = 0);
	~speed_frame();

	void init_rw_labels();
	void read_speed_pid(task_type_t task_type, uint16_t sub_index, funcalltask_t callback = nullptr);
	void write_speed_pid(task_type_t task_type, uint16_t sub_index, double kp, funcalltask_t callback = nullptr);
	void write_vel_dir(task_type_t task_type, uint16_t sub_index, int16_t vel_dir_, funcalltask_t callback = nullptr);
	void write_vel_acc(task_type_t task_type, uint16_t sub_index, int32_t vel_acc_, funcalltask_t callback = nullptr);
	void write_vel_dec(task_type_t task_type, uint16_t sub_index, int32_t vel_dec_, funcalltask_t callback = nullptr);
	void write_halt_deceleration(task_type_t task_type, uint16_t sub_index, int32_t halt_deceleration_, funcalltask_t callback = nullptr);
	void set_speed_pid(double &kp, task_type_t task_type, speed_data &speed_data_);
	void on_refresh();
protected:
	void showEvent(QShowEvent *);
public slots:
	void on_saveKp0Btn_clicked();
	void on_saveKp1Btn_clicked();
	void on_saveKp2Btn_clicked();
	void on_saveKi0Btn_clicked();
	void on_saveKi1Btn_clicked();
	void on_saveKi2Btn_clicked();
	void on_velDir_saveBtn_clicked();
	void on_saveAccBtn_clicked();
	void on_saveDecBtn_clicked();
	void on_saveHaltDecBtn_clicked();
	void on_refreshBtn_clicked();
private:
	Ui::speed_frame ui;
};

#endif // SPEED_FRAME_H
