#include "MessageBox.h"
#include <QMessageBox>
#include <QGuiApplication>

namespace gdp
{
	namespace gui
	{
		QMap<MessageBoxExt::StandardButton, QString> MessageBoxExt::btnTextsMap = QMap<StandardButton, QString>();

		MessageBoxExt::StandardButton MessageBoxExt::info(Type type, QWidget *parent, const QString &title, const QString &text, int buttons, MessageBoxExt::StandardButton defaultButton)
		{
			QMessageBox msg(parent);
			msg.setWindowTitle(title);
			msg.setText(text);

			switch (type)
			{
			case MessageBoxExt::Type::MB_Critical:
			{
				msg.setIcon(QMessageBox::Critical);
			}
				break;
			case MessageBoxExt::Type::MB_Information:
			{
				msg.setIcon(QMessageBox::Information);
			}
				break;
			case MessageBoxExt::Type::MB_Question:
			{
				msg.setIcon(QMessageBox::Question);
			}
				break;
			case MessageBoxExt::Type::MB_Warning:
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
			QString btnText;

			if (Ok == (buttons & Ok))
			{
				btnText = btnTextsMap.value(Ok, QObject::tr("Ok"));
				okBtn = msg.addButton(btnText, QMessageBox::ActionRole);
			}

			if (Cancel == (buttons & Cancel))
			{
				btnText = btnTextsMap.value(Cancel, QObject::tr("Cancel"));
				cancelBtn = msg.addButton(btnText, QMessageBox::ActionRole);
			}

			if (nullptr != okBtn && Ok == defaultButton)
			{
				msg.setDefaultButton(okBtn);
			}

			if (nullptr != cancelBtn && Cancel == defaultButton)
			{
				msg.setDefaultButton(cancelBtn);
			}

			msg.exec();

			if (okBtn == (QPushButton *)msg.clickedButton())
			{
				return Ok;
			}
			else if (cancelBtn == (QPushButton *)msg.clickedButton())
			{
				return Cancel;
			}

			return NoButton;
		}

		void MessageBoxExt::setBtnText(StandardButton sb, const QString &text)
		{
			btnTextsMap.insert(sb, text);
		}

		MessageBoxExt::StandardButton MessageBoxExt::critical(QWidget *parent, const QString &title, const QString &text, int buttons /*= Ok | Cancel*/, StandardButton defaultButton /*= NoButton*/)
		{
			return info(Type::MB_Critical, parent, title, text, buttons, defaultButton);
		}

		MessageBoxExt::StandardButton MessageBoxExt::information(QWidget *parent, const QString &title, const QString &text, int buttons /*= Ok | Cancel*/, StandardButton defaultButton /*= NoButton*/)
		{
			return info(Type::MB_Information, parent, title, text, buttons, defaultButton);
		}

		MessageBoxExt::StandardButton MessageBoxExt::question(QWidget *parent, const QString &title, const QString &text, int buttons /*= Ok | Cancel*/, StandardButton defaultButton /*= NoButton*/)
		{
			return info(Type::MB_Question, parent, title, text, buttons, defaultButton);
		}

		MessageBoxExt::StandardButton MessageBoxExt::warning(QWidget *parent, const QString &title, const QString &text, int buttons /*= Ok | Cancel*/, StandardButton defaultButton /*= NoButton*/)
		{
			return info(Type::MB_Warning, parent, title, text, buttons, defaultButton);
		}
	}
}