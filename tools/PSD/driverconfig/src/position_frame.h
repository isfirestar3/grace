#ifndef POSITION_FRAME_H
#define POSITION_FRAME_H

#include "ui_position_frame.h"
#include "task_type_define.h"
#include <QWidget>
#include <cstdint>

class position_frame : public QWidget
{
	Q_OBJECT

public:
	position_frame(QWidget *parent = 0);
	~position_frame();

	void init_rw_labels();
	void read_position_loop(task_type_t task_type, uint16_t sub_index, funcalltask_t callback = nullptr);
	void write_homing(task_type_t task_type, uint16_t sub_index, uint8_t homing_, funcalltask_t callback = nullptr);
	void write_pos_ki(task_type_t task_type, uint16_t sub_index, int32_t pos_ki_, funcalltask_t callback = nullptr);
	void write_pos_kp(task_type_t task_type, uint16_t sub_index, int32_t pos_kp_, funcalltask_t callback = nullptr);
	void write_jerk(task_type_t task_type, uint16_t sub_index, float jerk_, funcalltask_t callback = nullptr);
	void write_acceleration(task_type_t task_type, uint16_t sub_index, float acceleration_, funcalltask_t callback = nullptr);
	void write_velocity(task_type_t task_type, uint16_t sub_index, float velocity_, funcalltask_t callback = nullptr);
	void write_kvff(task_type_t task_type, uint16_t sub_index, int32_t kvff_, funcalltask_t callback = nullptr);
	void write_kaff(task_type_t task_type, uint16_t sub_index, int32_t kaff_, funcalltask_t callback = nullptr);
	void on_refresh();
protected:
	void showEvent(QShowEvent *);
private slots:
	void on_saveHomingBtn_clicked();
	void on_savePosKiBtn_clicked();
	void on_savePosKpBtn_clicked();
	void on_saveJerkBtn_clicked();
	void on_saveAccelerationBtn_clicked();
	void on_saveVelocityBtn_clicked();
	void on_saveKvffBtn_clicked();
	void on_saveKaffBtn_clicked();
	void on_refreshBtn_clicked();
private:
	Ui::position_frame ui;
};

#endif // POSITION_FRAME_H
