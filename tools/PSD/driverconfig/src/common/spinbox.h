#ifndef SPINBOX_H
#define SPINBOX_H

#include <QSpinBox>

namespace shr
{
	namespace gui
	{
		class SpinBox : public QSpinBox
		{
			Q_OBJECT

		public:
			SpinBox(QWidget *parent);
			~SpinBox();

			void InitUi();
		};
	}
}

#endif // SPINBOX_H
