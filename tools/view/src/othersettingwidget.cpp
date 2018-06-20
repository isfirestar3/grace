#include "othersettingwidget.h"
#include "dbg_vars_impls.h"
#include <climits>

namespace
{
	static const int MIN_REFRESH_TIME = 20;
	static const QString OTHER_SETTINGS_GROUP("OtherSettings");
	static const QString REFRESH_TIME("RefreshTime");
}

OtherSettingWidget::OtherSettingWidget(QWidget *parent)
: QWidget(parent),
m_settings{QSharedPointer<QSettings>::create(qApp->organizationName(), qApp->applicationName())}
{
	ui.setupUi(this);
	InitUi();
}

OtherSettingWidget::~OtherSettingWidget()
{

}

void OtherSettingWidget::InitUi()
{
	ui.refreshTimeLabel->setText(IS_EN ? "Refresh time:" : "刷新时间：");
	ui.okBtn->setText(IS_EN ? "Ok" : "保存");

	ui.refreshTimeSpinBox->setMinimum(MIN_REFRESH_TIME);
	ui.refreshTimeSpinBox->setRange(MIN_REFRESH_TIME, INT_MAX);

	int refreshTime = DEFAULT_REFRESH_TIME;
	LoadRefreshTime(refreshTime);
	nsp::toolkit::singleton<dbg_vars_impls>::instance()->set_refresh_time(refreshTime);
}

void OtherSettingWidget::LoadRefreshTime(int &refreshTime)
{
	if (m_settings.isNull())
	{
		return;
	}

	m_settings->beginGroup(OTHER_SETTINGS_GROUP);
	refreshTime = m_settings->value(REFRESH_TIME, DEFAULT_REFRESH_TIME).toInt();
	m_settings->endGroup();
}

void OtherSettingWidget::SaveRefreshTime(const int &refreshTime)
{
	if (m_settings.isNull())
	{
		return;
	}

	m_settings->beginGroup(OTHER_SETTINGS_GROUP);
	m_settings->setValue(REFRESH_TIME, refreshTime);
	m_settings->endGroup();
}

void OtherSettingWidget::on_okBtn_clicked()
{
	int refreshTime = ui.refreshTimeSpinBox->value();
	SaveRefreshTime(refreshTime);
	nsp::toolkit::singleton<dbg_vars_impls>::instance()->set_refresh_time(refreshTime);
}

void OtherSettingWidget::showEvent(QShowEvent *)
{
	int refreshTime = DEFAULT_REFRESH_TIME;
	LoadRefreshTime(refreshTime);
	ui.refreshTimeSpinBox->setValue(refreshTime);
}
