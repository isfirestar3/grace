#ifndef CONTROLDIALOGPRIVATE_H
#define CONTROLDIALOGPRIVATE_H

#include "controldialog.h"

class ControlDialogPrivate
{
	Q_DECLARE_PUBLIC(ControlDialog);
public:
	ControlDialogPrivate(ControlDialog *pControlDialog);
	~ControlDialogPrivate();

	void init();
private:
	ControlDialog *q_ptr{nullptr};
};

#endif // CONTROLDIALOGPRIVATE_H
