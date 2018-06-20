#ifndef FILE_SCHEDULE_H
#define FILE_SCHEDULE_H

#include <QDialog>
#include "ui_file_schedule.h"

class file_schedule : public QDialog
{
	Q_OBJECT

public:
	file_schedule(QWidget *parent = 0);
	~file_schedule();

	Ui::file_schedule ui;
private slots:
	void closeEvent(QCloseEvent * event);
};

#endif // FILE_SCHEDULE_H
