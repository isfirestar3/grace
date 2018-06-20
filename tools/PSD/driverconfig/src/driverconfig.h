#ifndef DRIVERCONFIG_H
#define DRIVERCONFIG_H

#include <QtWidgets/QMainWindow>
#include "ui_driverconfig.h"
#include "task_type_define.h"
#include <memory>
#include <cstdint>

class driverconfig : public QMainWindow
{
	Q_OBJECT

public:
	driverconfig(QWidget *parent = 0);
	~driverconfig();

	void write_save(task_type_t task_type, uint16_t sub_index, uint32_t save_, funcalltask_t callback = nullptr);

	void set_btns_enabled(bool enabled);

	void move_to_center(QWidget *p_widget);
private:
	void initframe();

	void initslot();

private slots:

	void on_con_btn_clicked();

	void on_import_btn_clicked();

	void on_export_btn_clicked();

	void on_btn_control_clicked();

	void on_btn_motor_parameter_clicked();

	void on_btn_motor_diagnosis_clicked();

	void on_btn_driver_setting_clicked();

	void on_btn_dio_clicked();

	void on_btn_save_clicked();

	void on_btn_position_loop_clicked();

	void on_btn_speed_loop_clicked();

	void on_btn_current_loop_clicked();

	void on_btn_upload_clicked();

	void on_btn_download_clicked();

	void update_frame(int, QString);

	void update_save_btn_state(bool ret);
signals:
	void need_update_frame( int , QString );
private:
	Ui::driverconfigClass ui;
};

#endif // DRIVERCONFIG_H
