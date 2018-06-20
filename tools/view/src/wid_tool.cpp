#include "wid_tool.h"

wid_tool::wid_tool(QWidget* parent) 
: QWidget(parent),
m_connectStateWidget{QSharedPointer<ConnectStateWidget>::create()}
{
	init_wid();
	init_tool();
	init_slots();
}

wid_tool::~wid_tool()
{

}

void wid_tool::init_wid()
{
	setWindowFlags(Qt::FramelessWindowHint);
	setAutoFillBackground(true);
	QPalette p = palette();
	p.setColor(backgroundRole(), QColor("#0173ca"));
	setPalette(p);

	//QPalette palette;
	//QPixmap back_pic(":/wid_tool/tool_skin");
	//setWindowFlags(Qt::FramelessWindowHint);
	//setAutoFillBackground(true);
	//palette.setBrush(QPalette::Window, QBrush(back_pic));
	//setPalette(palette);
}

void wid_tool::init_tool()
{
	if (m_connectStateWidget.isNull())
	{
		return;
	}

	QString minPixmapName(":/wid_sys/sys_min");
	QString maxPixmapName(":/wid_sys/sys_max");
	QString closePixmapName(":/wid_sys/sys_close");
	QPixmap minPixmap;

	if (!minPixmap.load(minPixmapName))
	{
		return;
	}

	QPixmap maxPixmap;

	if (!maxPixmap.load(maxPixmapName))
	{
		return;
	}

	QPixmap closePixmap;

	if (!closePixmap.load(closePixmapName))
	{
		return;
	}

	lay_main = new QVBoxLayout();
	__min_button = new PushButton;
	__max_button = new PushButton;
	__close_button = new PushButton;
	__min_button->SetPixmap(minPixmapName);
	__min_button->setFixedSize(QSize(minPixmap.width(), minPixmap.height()));
	__max_button->SetPixmap(maxPixmapName);
	__max_button->setFixedSize(QSize(maxPixmap.width(), maxPixmap.height()));
	__close_button->SetPixmap(closePixmapName);
	__close_button->setFixedSize(QSize(closePixmap.width(), closePixmap.height()));

	QHBoxLayout *title_layout = new QHBoxLayout();
	title_layout->addWidget(m_connectStateWidget.data());
	title_layout->addStretch(1);
	title_layout->addWidget(__min_button, 0, Qt::AlignTop | Qt::AlignRight);
	title_layout->addWidget(__max_button, 0, Qt::AlignTop | Qt::AlignRight);
	title_layout->addWidget(__close_button, 0, Qt::AlignTop | Qt::AlignRight);
	title_layout->setSpacing(0);
	title_layout->setContentsMargins(0, 0, 0, 0);
	lay_main->addLayout(title_layout);

	QStringList string_list;
	string_list << ":/wid_tool/monitor" << ":/wid_tool/synthesize"  << ":/wid_tool/setting";
	QStringList string_text;
	string_text << QStringLiteral("定向监控") << QStringLiteral("综合监控")  << QStringLiteral("参数配置");
	QHBoxLayout *button_layout = new QHBoxLayout();
	QSignalMapper *signal_mapper = new QSignalMapper(this);
	for (int i = 0; i < string_list.size(); i++)
	{
		ToolButton *tool_button = new ToolButton(string_list.at(i));
		tool_button->setText(string_text.at(i));
		__button_list.append(tool_button);
		connect(tool_button, SIGNAL(clicked()), signal_mapper, SLOT(map()));
		signal_mapper->setMapping(tool_button, QString::number(i, 10));
		button_layout->addWidget(tool_button, 0, Qt::AlignBottom);

		if (0 == i)
		{
			tool_button->setMousePress(true);
		}
	}
	button_layout->addStretch();
	connect(signal_mapper, SIGNAL(mapped(QString)), this, SLOT(slot_turn_page(QString)));
	button_layout->setContentsMargins(0, 0, 0, 0);
	
	lay_main->addLayout(button_layout);
	int left = style()->pixelMetric(QStyle::PM_LayoutLeftMargin);
	int bottom = style()->pixelMetric(QStyle::PM_LayoutBottomMargin);
	lay_main->setContentsMargins(left, 0, 0, bottom);
	setLayout(lay_main);
}

void wid_tool::mouseDoubleClickEvent(QMouseEvent * event)
{
	slot_max();
}

//void wid_tool::resizeEvent(QResizeEvent *event)
//{
//	QWidget::resizeEvent(event);
//	//QPalette palette;
//	//QPixmap back_pic(":/wid_tool/tool_skin");
//	//palette.setBrush(QPalette::Window, QBrush(back_pic.scaled(event->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation)));
//	//setPalette(palette);
//}

void wid_tool::slot_turn_page(QString current_page)
{
	bool ok;
	int current_index = current_page.toInt(&ok, 10);

	for (int i = 0; i<__button_list.count(); i++)
	{
		ToolButton *tool_button = __button_list.at(i);
		if (current_index == i)
		{
			tool_button->setMousePress(true);
		}
		else
		{
			tool_button->setMousePress(false);
		}
	}
	__reg_set_current_wid(current_index);

}

void wid_tool::set_current_wid(const std::function<void(int)>&func)
{
	__reg_set_current_wid = func;
}

void wid_tool::set_close_func(const std::function<void()>&func)
{
	__reg_close = func;
}
void wid_tool::set_max_func(const std::function<void()>&func)
{
	__reg_max = func;
}
void wid_tool::init_slots()
{
	connect(__min_button, SIGNAL(clicked()), this, SLOT(slot_min()));
	connect(__close_button, SIGNAL(clicked()), this, SLOT(slot_close()));
	connect(__max_button, SIGNAL(clicked()), this, SLOT(slot_max()));
}

void wid_tool::slot_close()
{
	__reg_close();
}
void wid_tool::set_min_func(const std::function<void()>&func)
{
	__reg_min = func;
}
void wid_tool::slot_min()
{
	__reg_min();
}
void wid_tool::set_restore_func(const std::function<void()>&func)
{
	__reg_restore = func;
}

bool __is_max_ = false;
void wid_tool::slot_max()
{
	if (nullptr == __max_button)
	{
		return;
	}

	if (!__is_max_)
	{
		__reg_max();
		__is_max_ = true;
		__max_button->SetPixmap(":/wid_sys/sys_restore");
	}
	else
	{
		__reg_restore();
		__is_max_ = false;
		__max_button->SetPixmap(":/wid_sys/sys_max");
	}
}