#ifndef OTHERSETTINGWIDGET_H
#define OTHERSETTINGWIDGET_H

#include "ui_othersettingwidget.h"
#include <QtWidgets/QWidget>
#include <QtCore/QSettings>
#include <QtCore/QSharedPointer>

class OtherSettingWidget : public QWidget
{
	Q_OBJECT

public:
	OtherSettingWidget(QWidget *parent = nullptr);
	~OtherSettingWidget();

	void InitUi();
	void LoadRefreshTime(int &refreshTime);
	void SaveRefreshTime(const int &refreshTime);
public slots:
	void on_okBtn_clicked();
protected:
	void showEvent(QShowEvent *);
private:
	Ui::OtherSettingWidget ui;
	QSharedPointer<QSettings> m_settings;
};

#endif // OTHERSETTINGWIDGET_H
