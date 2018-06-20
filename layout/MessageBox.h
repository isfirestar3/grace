#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include <QtCore/QString>

class QWidget;

class MessageBox
{
public:
	enum class Type
	{
		MB_Critical,
		MB_Information,
		MB_Question,
		MB_Warning
	};

    enum StandardButton
    {
        NoButton = 0x00000000,
        Ok = 0x00000001,
        Cancel = 0x00000002
    };

public:
	static StandardButton Critical(QWidget *parent, const QString &title, const QString &text, int buttons = Ok | Cancel, StandardButton defaultButton = NoButton);
	static StandardButton Information(QWidget *parent, const QString &title, const QString &text, int buttons = Ok | Cancel, StandardButton defaultButton = NoButton);
	static StandardButton Question(QWidget *parent, const QString &title, const QString &text, int buttons = Ok | Cancel, StandardButton defaultButton = NoButton);
	static StandardButton Warning(QWidget *parent, const QString &title, const QString &text, int buttons = Ok | Cancel, StandardButton defaultButton = NoButton);
    static StandardButton Info(Type type, QWidget *parent, const QString &title, const QString &text, int buttons = Ok | Cancel, StandardButton defaultButton = NoButton);
};

#endif // MESSAGEBOX_H
