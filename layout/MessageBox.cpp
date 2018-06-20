#include "MessageBox.h"
#include <QtWidgets/QMessageBox>
#include <QtGui/QGuiApplication>

MessageBox::StandardButton MessageBox::Info(Type type, QWidget *parent, const QString &title, const QString &text, int buttons, MessageBox::StandardButton defaultButton)
{
	bool m_en = qApp->property("language").toInt() == 1;
    QMessageBox msg(parent);
    msg.setWindowTitle(title);
    msg.setText(text);

	switch (type)
	{
		case MessageBox::Type::MB_Critical:
		{
			msg.setIcon(QMessageBox::Critical);
		}
			break;
		case MessageBox::Type::MB_Information:
		{
			msg.setIcon(QMessageBox::Information);
		}
			break;
		case MessageBox::Type::MB_Question:
		{
			msg.setIcon(QMessageBox::Question);
		}
			break;
		case MessageBox::Type::MB_Warning:
		{
			msg.setIcon(QMessageBox::Warning);
		}
			break;
		default:
		{
			Q_ASSERT(false);
		}
			break;
	}

    QPushButton *okBtn = nullptr;
    QPushButton *cancelBtn = nullptr;

    if(Ok == (buttons & Ok))
    {
        okBtn = msg.addButton(m_en ? "Ok" : QStringLiteral("确定"), QMessageBox::ActionRole);
    }

    if(Cancel == (buttons & Cancel))
    {
		cancelBtn = msg.addButton(m_en ? "Cancel" : QStringLiteral("取消"), QMessageBox::ActionRole);
    }

    if(nullptr != okBtn && Ok == defaultButton)
    {
        msg.setDefaultButton(okBtn);
    }

    if(nullptr != cancelBtn && Cancel == defaultButton)
    {
        msg.setDefaultButton(cancelBtn);
    }

    msg.exec();

    if(okBtn == (QPushButton *)msg.clickedButton())
    {
        return Ok;
    }
    else if(cancelBtn == (QPushButton *)msg.clickedButton())
    {
        return Cancel;
    }

    return NoButton;
}

MessageBox::StandardButton MessageBox::Critical(QWidget *parent, const QString &title, const QString &text, int buttons /*= Ok | Cancel*/, StandardButton defaultButton /*= NoButton*/)
{
	return Info(Type::MB_Critical, parent, title, text, buttons, defaultButton);
}

MessageBox::StandardButton MessageBox::Information(QWidget *parent, const QString &title, const QString &text, int buttons /*= Ok | Cancel*/, StandardButton defaultButton /*= NoButton*/)
{
	return Info(Type::MB_Information, parent, title, text, buttons, defaultButton);
}

MessageBox::StandardButton MessageBox::Question(QWidget *parent, const QString &title, const QString &text, int buttons /*= Ok | Cancel*/, StandardButton defaultButton /*= NoButton*/)
{
	return Info(Type::MB_Question, parent, title, text, buttons, defaultButton);
}

MessageBox::StandardButton MessageBox::Warning(QWidget *parent, const QString &title, const QString &text, int buttons /*= Ok | Cancel*/, StandardButton defaultButton /*= NoButton*/)
{
	return Info(Type::MB_Warning, parent, title, text, buttons, defaultButton);
}
