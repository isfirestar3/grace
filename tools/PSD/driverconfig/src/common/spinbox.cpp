#include "spinbox.h"

namespace shr
{
	namespace gui
	{
		SpinBox::SpinBox(QWidget *parent)
			: QSpinBox(parent)
		{
			InitUi();
		}

		SpinBox::~SpinBox()
		{

		}

		void SpinBox::InitUi()
		{
			int min = std::numeric_limits<int>::min();
			int max = std::numeric_limits<int>::max();
			setRange(min, max);
		}
	}
}
