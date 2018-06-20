#include "controldialog_p.h"

ControlDialogPrivate::ControlDialogPrivate(ControlDialog *pControlDialog)
:q_ptr{pControlDialog}
{

}

ControlDialogPrivate::~ControlDialogPrivate()
{

}

void ControlDialogPrivate::init()
{
	Q_Q(ControlDialog);

	if (nullptr == q)
	{
		return;
	}

	Qt::WindowFlags wf = q->windowFlags();
	wf &= ~Qt::WindowContextHelpButtonHint;
	q->setWindowFlags(wf);
}
