#ifndef UPLOAD_BTNS_H
#define UPLOAD_BTNS_H

#include <QtWidgets\qwidget.h>
#include <QtWidgets\qtablewidget.h>
#include "ui_upload_btns.h"

class Upload_btns : public QWidget
{
	Q_OBJECT

public:
	Upload_btns(QWidget *parent = 0);
	~Upload_btns();
	//int row_;
	QTableWidgetItem*ip_item;
	Ui::upload_btns ui;
private slots:
	void on_open_clicked();
	void on_start_clicked();
	void on_delete__clicked();
signals:
	void openClicked(int);
	void startClicked(int);
	void deleteClicked(int);
private:
	
};

#endif // UPLOAD_BTNS_H
