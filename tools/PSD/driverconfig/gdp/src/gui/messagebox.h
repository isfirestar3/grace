#ifndef MESSAGEBOX_H
#define MESSAGEBOX_H

#include "gdp_global.h"
#include <QMap>

class QWidget;
class QString;

namespace gdp
{
	namespace gui
	{
		class GDP_EXPORT MessageBoxExt
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
			static void setBtnText(StandardButton sb, const QString &text);
			static StandardButton critical(QWidget *parent, const QString &title, const QString &text, int buttons = Ok | Cancel, StandardButton defaultButton = NoButton);
			static StandardButton information(QWidget *parent, const QString &title, const QString &text, int buttons = Ok | Cancel, StandardButton defaultButton = NoButton);
			static StandardButton question(QWidget *parent, const QString &title, const QString &text, int buttons = Ok | Cancel, StandardButton defaultButton = NoButton);
			static StandardButton warning(QWidget *parent, const QString &title, const QString &text, int buttons = Ok | Cancel, StandardButton defaultButton = NoButton);
		private:
			static StandardButton info(Type type, QWidget *parent, const QString &title, const QString &text, int buttons = Ok | Cancel, StandardButton defaultButton = NoButton);

			static QMap<StandardButton, QString> btnTextsMap;
		};
	}
}

#endif // MESSAGEBOX_H
