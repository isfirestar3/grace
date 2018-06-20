#ifndef CURRENT_FRAME_H
#define CURRENT_FRAME_H

#include "ui_current_frame.h"
#include "task_type_define.h"
#include <QWidget>
#include <cstdint>

class current_frame : public QWidget
{
	Q_OBJECT

public:
	current_frame(QWidget *parent = 0);
	~current_frame();

	void init_rw_labels();
	void on_refresh();
	void read_current(task_type_t task_type, uint16_t sub_index, funcalltask_t callback = nullptr);
	void write_max_current(task_type_t task_type, uint16_t sub_index, uint8_t max_current_, funcalltask_t callback = nullptr);
	void write_con_current(task_type_t task_type, uint16_t sub_index, uint8_t con_current_, funcalltask_t callback = nullptr);
public slots:
	void on_max_current_saveBtn_clicked();
	void on_con_current_saveBtn_clicked();
	void on_peak_time_saveBtn_clicked();
	void on_refreshBtn_clicked();
protected:
	void showEvent(QShowEvent *);
private:
	Ui::current_frame ui;
};

#endif // CURRENT_FRAME_H
