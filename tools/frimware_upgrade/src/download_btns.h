#ifndef DOWNLOAD_BTNS_H
#define DOWNLOAD_BTNS_H

#include <QtWidgets\qwidget.h>
#include "ui_download_btns.h"

class Download_btns : public QWidget
{
	Q_OBJECT

public:
	Download_btns(QWidget *parent = 0);
	~Download_btns();
	int row_;
		Ui::Download_btns ui;
private slots:
	void on_start_clicked();
	void on_restart_clicked();
	void on_reset_clicked();
signals:
	void startClicked(int);
	void restartClicked(int);
	void resetClicked(int);
private:

};

#endif // DOWNLOAD_BTNS_H
