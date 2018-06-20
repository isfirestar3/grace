#include "doublespinbox.h"

namespace shr
{
	namespace gui
	{
		DoubleSpinBox::DoubleSpinBox(QWidget *parent)
			: QDoubleSpinBox(parent)
		{
			InitUi();
		}

		DoubleSpinBox::~DoubleSpinBox()
		{

		}

		void DoubleSpinBox::InitUi()
		{
			double min = -std::numeric_limits<double>::max();
			double max = std::numeric_limits<double>::max();
			setRange(min, max);
		}
	}
}
