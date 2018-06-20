#include "MessageBox.h"
#include <QtWidgets/QMessageBox>
#include <QtGui/QGuiApplication>

CMessageBox::StandardButton CMessageBox::Info(Type type, QWidget *parent, const QString &title, const QString &text, int buttons, CMessageBox::StandardButton defaultButton)
{
    QMessageBox msg(parent);
    msg.setWindowTitle(title);
    msg.setText(text);

	switch (type)
	{
		case CMessageBox::Type::MB_Critical:
		{
			msg.setIcon(QMessageBox::Critical);
		}
			break;
		case CMessageBox::Type::MB_Information:
		{
			msg.setIcon(QMessageBox::Information);
		}
			break;
		case CMessageBox::Type::MB_Question:
		{
			msg.setIcon(QMessageBox::Question);
		}
			break;
		case CMessageBox::Type::MB_Warning:
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
        okBtn = msg.addButton(IS_EN ? "Ok" : "确定", QMessageBox::ActionRole);
    }

    if(Cancel == (buttons & Cancel))
    {
		cancelBtn = msg.addButton(IS_EN ? "Cancel" : "取消", QMessageBox::ActionRole);
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

CMessageBox::StandardButton CMessageBox::Critical(QWidget *parent, const QString &title, const QString &text, int buttons /*= Ok | Cancel*/, StandardButton defaultButton /*= NoButton*/)
{
	return Info(Type::MB_Critical, parent, title, text, buttons, defaultButton);
}

CMessageBox::StandardButton CMessageBox::Information(QWidget *parent, const QString &title, const QString &text, int buttons /*= Ok | Cancel*/, StandardButton defaultButton /*= NoButton*/)
{
	return Info(Type::MB_Information, parent, title, text, buttons, defaultButton);
}

CMessageBox::StandardButton CMessageBox::Question(QWidget *parent, const QString &title, const QString &text, int buttons /*= Ok | Cancel*/, StandardButton defaultButton /*= NoButton*/)
{
	return Info(Type::MB_Question, parent, title, text, buttons, defaultButton);
}

CMessageBox::StandardButton CMessageBox::Warning(QWidget *parent, const QString &title, const QString &text, int buttons /*= Ok | Cancel*/, StandardButton defaultButton /*= NoButton*/)
{
	return Info(Type::MB_Warning, parent, title, text, buttons, defaultButton);
}
