#ifndef DOUBLESPINBOX_H
#define DOUBLESPINBOX_H

#include <QDoubleSpinBox>

namespace shr
{
	namespace gui
	{
		class DoubleSpinBox : public QDoubleSpinBox
		{
			Q_OBJECT

		public:
			DoubleSpinBox(QWidget *parent = nullptr);
			~DoubleSpinBox();

			void InitUi();
		};
	}
}

#endif // DOUBLESPINBOX_H
